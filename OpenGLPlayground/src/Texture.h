#pragma once

// NOTE: do not define STB_IMAGE_IMPLEMENTATION
//          it would conflict with nanovg.c (libnanogui.a)
// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Texture
{
public:
    std::string obj_name;
    GLuint texture;
    int width, height, channel;

public:
    Texture(const std::string& tex_path) {
        m_tex_path = tex_path;
        load_texture(m_tex_path);
    };

    ~Texture() {};

    void load_texture(const std::string& tex_path) {
        //texture
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        // Set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    // Set texture wrapping to GL_REPEAT (usually basic wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // Set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //load img
        unsigned char* image = stbi_load(tex_path.c_str(), &width, &height, 0, 0);
        assert(image);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
        //free&unbind
        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(image);
    }
    
private:
    std::string m_tex_path;
};
