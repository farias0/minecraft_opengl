#pragma once

#include <glm/glm.hpp>
#include <stb_image.h>

#include "Camera.hpp"
#include "GameState.hpp"
#include "Shader.hpp"

class Cube
{
public:

    glm::vec3 pos;

    Cube(glm::vec3 pos) : 
        pos(pos),
        shader("src/shaders/triangle.vs", "src/shaders/triangle.fs")
    {
        GLuint vbo;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(MESH), MESH, GL_STATIC_DRAW);
        
        // pos
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texcoord
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        int width, height, nrChannels;
        unsigned char *texData;
        stbi_set_flip_vertically_on_load(true);
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        texData = stbi_load(TEXTURE_PATH, &width, &height, &nrChannels, 0); 
        if (texData) {
            GLenum format;
            if (nrChannels == 1)
                format = GL_RED;
            else if (nrChannels == 3)
                format = GL_RGB;
            else if (nrChannels == 4)
                format = GL_RGBA;
            else {
                std::ostringstream oss;
                oss << "Texture with invalid number of channels: " << TEXTURE_PATH << ", n=" << nrChannels;
                throw std::runtime_error(oss.str());
            }
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, texData);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            std::cout << "Failed to load texture " << TEXTURE_PATH << std::endl;
        }
        stbi_image_free(texData);

        shader.Use();
        shader.SetInt("texture1", 0); // texture bank (0 = GL_TEXTURE0)
    }

    void Render()
    {
        shader.Use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glBindVertexArray(vao);

        glm::mat4 view = camera->GetViewMatrix();
        glm::mat4 projection = camera->GetProjectionMatrix();

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, pos);
        
        shader.SetMat4("model", model);
        shader.SetMat4("view", view);
        shader.SetMat4("projection", projection);
        
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

private:
    GLuint vao;
    GLuint texture;
    Shader shader;
    
    const char *TEXTURE_PATH = "resources/block.png";
    const float MESH[180] = {
        // Front face
        -0.5f, -0.5f, -0.5f,  0.25f, 0.33f,
         0.5f, -0.5f, -0.5f,  0.5f,  0.33f,
         0.5f,  0.5f, -0.5f,  0.5f,  0.66f,
         0.5f,  0.5f, -0.5f,  0.5f,  0.66f,
        -0.5f,  0.5f, -0.5f,  0.25f, 0.66f,
        -0.5f, -0.5f, -0.5f,  0.25f, 0.33f,

        // Back face
        -0.5f, -0.5f,  0.5f,  0.25f, 0.33f,
         0.5f, -0.5f,  0.5f,  0.5f,  0.33f,
         0.5f,  0.5f,  0.5f,  0.5f,  0.66f,
         0.5f,  0.5f,  0.5f,  0.5f,  0.66f,
        -0.5f,  0.5f,  0.5f,  0.25f, 0.66f,
        -0.5f, -0.5f,  0.5f,  0.25f, 0.33f,

        // Left face
        -0.5f,  0.5f,  0.5f,  0.75f, 0.66f,
        -0.5f,  0.5f, -0.5f,  0.5f,  0.66f,
        -0.5f, -0.5f, -0.5f,  0.5f,  0.33f,
        -0.5f, -0.5f, -0.5f,  0.5f,  0.33f,
        -0.5f, -0.5f,  0.5f,  0.75f, 0.33f,
        -0.5f,  0.5f,  0.5f,  0.75f, 0.66f,


        // Right face
        0.5f,  0.5f,  0.5f,  0.75f, 0.66f,
        0.5f,  0.5f, -0.5f,  0.5f,  0.66f,
        0.5f, -0.5f, -0.5f,  0.5f,  0.33f,
        0.5f, -0.5f, -0.5f,  0.5f,  0.33f,
        0.5f, -0.5f,  0.5f,  0.75f, 0.33f,
        0.5f,  0.5f,  0.5f,  0.75f, 0.66f,

        // Bottom face
        -0.5f, -0.5f, -0.5f,  0.5f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.25f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.25f, 0.33f,
         0.5f, -0.5f,  0.5f,  0.25f, 0.33f,
        -0.5f, -0.5f,  0.5f,  0.5f,  0.33f,
        -0.5f, -0.5f, -0.5f,  0.5f,  0.0f,

        // Top face
        -0.5f,  0.5f, -0.5f,  0.5f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.25f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.25f, 0.66f,
         0.5f,  0.5f,  0.5f,  0.25f, 0.66f,
        -0.5f,  0.5f,  0.5f,  0.5f,  0.66f,
        -0.5f,  0.5f, -0.5f,  0.5f,  1.0f
    };
};