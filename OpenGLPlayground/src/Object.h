#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>

class Object
{
public:
	struct Vertex {
		// Position
		glm::vec3 Position;
		// Normal
		glm::vec3 Normal;
		// TexCoords
		glm::vec2 TexCoords;
        // Tangent
        glm::vec3 Tangent;
        // Bitangent
        glm::vec3 Bitangent;
        Vertex(const glm::vec3& p, const glm::vec3& n, const glm::vec2& t, const glm::vec3& tan, const glm::vec3& bitan) {
            Position = p;
            Normal = n;
            TexCoords = t;
            Tangent = tan;
            Bitangent = bitan;
        }
	};

	struct Vertex_Index {
		int pos_idx;
		int normal_idx;
		int texcoord_idx;
	};

	struct Face_Index {
		Vertex_Index vertex[3];
        glm::vec3 tangent;
        glm::vec3 bitangent;
	};

	glm::vec3 max_bound = glm::vec3(INT_MIN);
	glm::vec3 min_bound = glm::vec3(INT_MAX);
    
    //vao vector
    std::vector<Vertex> vao_vertices;

	glm::vec4 obj_color = glm::vec4(0.7, 0.7, 0.7, 1.0);
	GLfloat shininess = 32.0f;

	GLuint vao, vbo;

public:
	Object(const std::string& obj_path) { 
		m_obj_path = obj_path;
		load_obj(m_obj_path);
	};

	~Object() {};
    
	void load_obj(const std::string& obj_path)
	{
		std::string suffix = obj_path.substr(obj_path.size() - 3);
		if (suffix == "obj") {
			vao_vertices.clear();
			indexed_faces.clear();
			ori_positions.clear();
			ori_normals.clear();
			ori_texcoords.clear();

			std::ifstream ifs;
			// Store original data vector
			try {
				ifs.open(obj_path);
				std::string one_line;
				while (getline(ifs, one_line)) {
					std::stringstream ss(one_line);
					std::string type;
					ss >> type;
					if (type == "v") {
						glm::vec3 vert_pos;
						ss >> vert_pos[0] >> vert_pos[1] >> vert_pos[2];
						ori_positions.push_back(vert_pos);
					}
					else if (type == "vt") {
						glm::vec2 tex_coord;
						ss >> tex_coord[0] >> tex_coord[1];
						ori_texcoords.push_back(tex_coord);
					}
					else if (type == "vn") {
						glm::vec3 vert_norm;
						ss >> vert_norm[0] >> vert_norm[1] >> vert_norm[2];
						ori_normals.push_back(vert_norm);
					}
					else if (type == "f") {
						Face_Index face_idx;
						// Here we only accept face number 3
						for (int i = 0; i < 3; i++) {
							std::string s_vertex;
							ss >> s_vertex;
							int pos_idx = -1;
							int tex_idx = -1;
							int norm_idx = -1;
							sscanf(s_vertex.c_str(), "%d/%d/%d", &pos_idx, &tex_idx, &norm_idx);
							// We have to use index -1 because the obj index starts at 1
							// Incorrect input will be set as -1
							face_idx.vertex[i].pos_idx = pos_idx > 0 ? pos_idx - 1 : -1;
							face_idx.vertex[i].texcoord_idx = tex_idx > 0 ? tex_idx - 1 : -1;
							face_idx.vertex[i].normal_idx = norm_idx > 0 ? norm_idx - 1 : -1;
						}
                        // calculate tangent/bitangent vectors
                        glm::vec3 tangent, bitangent;
                        glm::vec3 edge1 = ori_positions[face_idx.vertex[1].pos_idx] - ori_positions[face_idx.vertex[0].pos_idx];
                        glm::vec3 edge2 = ori_positions[face_idx.vertex[2].pos_idx] - ori_positions[face_idx.vertex[0].pos_idx];
                        glm::vec2 deltaUV1 = ori_texcoords[face_idx.vertex[1].texcoord_idx] - ori_texcoords[face_idx.vertex[0].texcoord_idx];
                        glm::vec2 deltaUV2 = ori_texcoords[face_idx.vertex[2].texcoord_idx] - ori_texcoords[face_idx.vertex[0].texcoord_idx];

                        GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

                        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
                        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
                        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
                        tangent = glm::normalize(tangent);

                        bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
                        bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
                        bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
                        bitangent = glm::normalize(bitangent);
                        face_idx.tangent = tangent;
                        face_idx.bitangent = bitangent;
						indexed_faces.push_back(face_idx);
					}
				}
			}
			catch (const std::exception&) {
				std::cout << "Error: Obj file cannot be read\n";
			}
            for (int i = 0; i < indexed_faces.size(); i++) {
                for (int j = 0; j < 3; j++) {
                    Vertex_Index vi = indexed_faces[i].vertex[j];
                    vao_vertices.push_back(Vertex(ori_positions[vi.pos_idx], ori_normals[vi.normal_idx], ori_texcoords[vi.texcoord_idx],
                                                  indexed_faces[i].tangent, indexed_faces[i].bitangent));
                }
            }
            indexed_faces.clear();
            ori_positions.clear();
            ori_normals.clear();
            ori_texcoords.clear();
		}
	};
    
private:
    std::string m_obj_path;

    // obj original data vector
    std::vector<glm::vec3> ori_positions;
    std::vector<glm::vec3> ori_normals;
    std::vector<glm::vec2> ori_texcoords;

    // obj face index vector
    std::vector<Face_Index> indexed_faces;
};
