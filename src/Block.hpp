#pragma once

#include <glm/glm.hpp>

#include "Collision.hpp"
#include "Shader.hpp"

class Block
{
public:

    glm::vec3 pos;

    Block(glm::vec3 pos);
    static void Render();

    AABB GetCollisionBox();

private:
    static const char *TEXTURE_PATH;
    static const float MESH[180];
    static const unsigned int MAX_BLOCK_COUNT;

    static bool isDataLoaded;
    static GLuint vao;
    static glm::mat4 *instanceData;
    static GLuint texture;
    static std::unique_ptr<Shader> shader;

    static void LoadData();
    
    bool IsInFrustum(glm::vec4 *frustumPlanes);
};