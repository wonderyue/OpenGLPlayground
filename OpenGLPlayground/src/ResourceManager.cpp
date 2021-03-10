#include "ResourceManager.h"
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>


const std::vector<ResourceManager::ModelData> model_type_2_data = {
    ResourceManager::ModelData("model/cube.obj", glm::vec3(-1.0f, 0.0f,  3.0f), glm::vec3(0.0f, -90.0f,  90.0f)),
    ResourceManager::ModelData("model/rock.obj", glm::vec3(0.0f, 0.0f,  6.75f), glm::vec3(0.0f, -90.0f,  90.0f)),
    ResourceManager::ModelData("model/cyborg.obj", glm::vec3(0.0f, 2.0f,  5.0f), glm::vec3(0.0f, -90.0f,  90.0f)),
    ResourceManager::ModelData("model/two_cubes.obj", glm::vec3(-3.2f, 0.15f, 4.37f), glm::vec3(0.0f, -70.0f,  90.0f)),
};

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
    for (auto model : m_models) {
        delete model.second;
    }
    m_models.clear();
}

Object* ResourceManager::load(std::string path)
{
    Object* obj = new Object(path);
    glGenVertexArrays(1, &obj->vao);
    glGenBuffers(1, &obj->vbo);
    glGenBuffers(1, &obj->ebo);
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(obj->vao);
    // Bind vertices to Array Buffer
    glBindBuffer(GL_ARRAY_BUFFER, obj->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Object::Vertex) * obj->vao_vertices.size(), &obj->vao_vertices[0], GL_STATIC_DRAW);
    // Bind indices to Element Array Buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * obj->veo_indices.size(), &obj->veo_indices[0], GL_STATIC_DRAW);
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Object::Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Object::Vertex), (GLvoid*)offsetof(Object::Vertex, Normal));
    glEnableVertexAttribArray(1);
    // Texture Coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Object::Vertex), (GLvoid*)offsetof(Object::Vertex, TexCoords));
    glEnableVertexAttribArray(2);
    // Unbind VAO
    glBindVertexArray(0);
    return obj;
}

Object* ResourceManager::get_model(const std::string& path)
{
    if (m_models.find(path) == m_models.end()) {
        m_models[path] = load(path);
    }
    return m_models[path];
}

Object* ResourceManager::get_model(model_type type)
{
    return get_model(model_type_2_data[type].path);
}

const ResourceManager::ModelData& ResourceManager::get_model_data(model_type type)
{
    return model_type_2_data[type];
}
