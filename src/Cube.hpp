#pragma once

#include <glm/glm.hpp>

#include "Shader.hpp"

class Cube
{
public:

    glm::vec3 pos;

    Cube(glm::vec3 pos);
    void Render();

private:
    static const char *TEXTURE_PATH;
    static const float MESH[180];

    static bool isDataLoaded;
    static GLuint vao;
    static GLuint texture;
    static std::unique_ptr<Shader> shader;

    void LoadData();
};