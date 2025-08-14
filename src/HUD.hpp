#pragma once

#include <glad/glad.h>

#include "Shader.hpp"

class HUD
{
public:
    
    HUD();
    void Render();

private:

    static const char *CROSSHAIR_TEXTURE_PATH;
    static const float CROSSHAIR_SIZE;
    static const float MESH[32];

    static GLuint vao;
    static GLuint texture;
    static std::unique_ptr<Shader> shader;
};