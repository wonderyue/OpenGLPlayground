#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
        Vertex(const glm::vec3& p, const glm::vec3& n, const glm::vec2& t) {
            Position = p;
            Normal = n;
            TexCoords = t;
        }
	};

	struct Vertex_Index {
		int pos_idx;
		int normal_idx;
		int texcoord_idx;
	};

	struct Face_Index {
		Vertex_Index vertex[3];
	};

	// veo and vao vector
	std::vector<Vertex> vao_vertices;
	std::vector<unsigned int> veo_indices;

	// obj original data vector
	std::vector<glm::vec3> ori_positions;
	std::vector<glm::vec3> ori_normals;
	std::vector<glm::vec2> ori_texcoords;

	// obj face index vector
	std::vector<Face_Index> indexed_faces;

	glm::vec3 obj_center;

	glm::vec3 max_bound = glm::vec3(INT_MIN);
	glm::vec3 min_bound = glm::vec3(INT_MAX);

	glm::vec4 obj_color = glm::vec4(0.7, 0.7, 0.7, 1.0);
	GLfloat shininess = 32.0f;

	std::string m_obj_path;
	std::string obj_name;

	GLuint vao, vbo, ebo;

public:
	Object(std::string obj_path) { 
		m_obj_path = obj_path;
		load_obj(m_obj_path);
	};

	~Object() {};

	void load_obj(std::string obj_path)
	{
		int path_str_length = obj_path.size();
		std::string suffix = obj_path.substr(path_str_length - 3, path_str_length);

		if (suffix == "obj") {
			vao_vertices.clear();
			veo_indices.clear();
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
						indexed_faces.push_back(face_idx);
					}
				}
			}
			catch (const std::exception&) {
				std::cout << "Error: Obj file cannot be read\n";
			}
            // indices
            std::vector<Vertex_Index> unique_indexed_vertices;
            for (int i = 0; i < indexed_faces.size(); i++) {
                for (int vi = 0; vi < 3; vi++) {
                    Vertex_Index vertex_index = indexed_faces[i].vertex[vi];
                    int curIndex = unique_indexed_vertices.size();
                    for (int j = 0; j < curIndex; j++) {
                        if (unique_indexed_vertices[j].pos_idx == vertex_index.pos_idx &&
                            unique_indexed_vertices[j].normal_idx == vertex_index.normal_idx &&
                            unique_indexed_vertices[j].texcoord_idx == vertex_index.texcoord_idx)
                        curIndex = j;
                        break;
                    }
                    if (curIndex == unique_indexed_vertices.size()) {
                        unique_indexed_vertices.push_back(vertex_index);
                    }
                    veo_indices.push_back(curIndex);
                }
            }
            // vertices
            for (Vertex_Index vi : unique_indexed_vertices) {
                vao_vertices.push_back(Vertex(ori_positions[vi.pos_idx], ori_normals[vi.normal_idx], ori_texcoords[vi.texcoord_idx]));
            }
		}
	};
};
