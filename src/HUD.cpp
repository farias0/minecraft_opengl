#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <sstream>
#include <stb_image.h>

#include "HUD.hpp"

#include "Debug.hpp"
#include "GameState.hpp"
#include "Shader.hpp"
#include "Player.hpp"

const char *HUD::CROSSHAIR_TEXTURE_PATH = "resources/crosshair.png";

const float HUD::CROSSHAIR_SIZE = 32.0f;

const float HUD::MESH[32] = {
    -CROSSHAIR_SIZE/2, -CROSSHAIR_SIZE/2,  0.0f, 0.0f,
     CROSSHAIR_SIZE/2, -CROSSHAIR_SIZE/2,  1.0f, 0.0f,
     CROSSHAIR_SIZE/2,  CROSSHAIR_SIZE/2,  1.0f, 1.0f,
     CROSSHAIR_SIZE/2,  CROSSHAIR_SIZE/2,  1.0f, 1.0f,
    -CROSSHAIR_SIZE/2,  CROSSHAIR_SIZE/2,  0.0f, 1.0f,
    -CROSSHAIR_SIZE/2, -CROSSHAIR_SIZE/2,  0.0f, 0.0f
};

GLuint HUD::vao;
GLuint HUD::texture;
std::unique_ptr<Shader> HUD::shader;

HUD::HUD()
{
    shader = std::unique_ptr<Shader>(new Shader("src/shaders/hud.vs", "src/shaders/hud.fs"));

    GLuint vbo;
    glGenVertexArrays(1, &vao);    
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(MESH), MESH, GL_STATIC_DRAW);
    
    // pos
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texcoord
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);


    int width, height, nrChannels;
    unsigned char *texData;
    stbi_set_flip_vertically_on_load(true);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    texData = stbi_load(CROSSHAIR_TEXTURE_PATH, &width, &height, &nrChannels, 4); 
    if (texData) {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 2)
            format = GL_RGBA;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        else {
            std::ostringstream oss;
            oss << "Texture with invalid number of channels: " << CROSSHAIR_TEXTURE_PATH << ", n=" << nrChannels;
            DebugLog(oss.str());
            throw std::runtime_error(oss.str());
        }
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, texData);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture " << CROSSHAIR_TEXTURE_PATH << std::endl;
    }
    stbi_image_free(texData);
}

void HUD::Render()
{
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shader->Use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(SCR_WIDTH/2.0f, SCR_HEIGHT/2.0f, 0.0f));
    if (player->IsFlying) model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    shader->SetMat4("model", model);

    glm::mat4 projection = glm::ortho(0.0f, (float)SCR_WIDTH, 0.0f, (float)SCR_HEIGHT);
    shader->SetMat4("projection", projection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    shader->SetInt("crosshairTexture", 0);
    
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}