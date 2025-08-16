#include <chrono>
#include <stb_image.h>
#include <vector>

#include "Block.hpp"

#include "Camera.hpp"
#include "Debug.hpp"
#include "GameState.hpp"

const char *Block::TEXTURE_PATH = "resources/block.png";

const float Block::MESH[180] = {
    // Front face (z = -0.5)
    -0.5f, -0.5f, -0.5f,  0.25f, 0.33f,
    0.5f,  0.5f, -0.5f,  0.5f,  0.66f,
    0.5f, -0.5f, -0.5f,  0.5f,  0.33f,
    0.5f,  0.5f, -0.5f,  0.5f,  0.66f,
    -0.5f, -0.5f, -0.5f,  0.25f, 0.33f,
    -0.5f,  0.5f, -0.5f,  0.25f, 0.66f,

    // Back face (z = +0.5)
    -0.5f, -0.5f,  0.5f,  0.25f, 0.33f,
    0.5f, -0.5f,  0.5f,  0.5f,  0.33f,
    0.5f,  0.5f,  0.5f,  0.5f,  0.66f,
    0.5f,  0.5f,  0.5f,  0.5f,  0.66f,
    -0.5f,  0.5f,  0.5f,  0.25f, 0.66f,
    -0.5f, -0.5f,  0.5f,  0.25f, 0.33f,

    // Left face (x = -0.5)
    -0.5f,  0.5f,  0.5f,  0.75f, 0.66f,
    -0.5f,  0.5f, -0.5f,  0.5f,  0.66f,
    -0.5f, -0.5f, -0.5f,  0.5f,  0.33f,
    -0.5f, -0.5f, -0.5f,  0.5f,  0.33f,
    -0.5f, -0.5f,  0.5f,  0.75f, 0.33f,
    -0.5f,  0.5f,  0.5f,  0.75f, 0.66f,

    // Right face (x = +0.5)
    0.5f, -0.5f,  0.5f,  0.75f, 0.33f,
    0.5f, -0.5f, -0.5f,  0.5f,  0.33f,
    0.5f,  0.5f, -0.5f,  0.5f,  0.66f,
    0.5f,  0.5f, -0.5f,  0.5f,  0.66f,
    0.5f,  0.5f,  0.5f,  0.75f, 0.66f,
    0.5f, -0.5f,  0.5f,  0.75f, 0.33f,

    // Bottom face (y = -0.5)
    -0.5f, -0.5f, -0.5f,  0.5f,  0.0f,
    0.5f, -0.5f, -0.5f,  0.25f, 0.0f,
    0.5f, -0.5f,  0.5f,  0.25f, 0.33f,
    0.5f, -0.5f,  0.5f,  0.25f, 0.33f,
    -0.5f, -0.5f,  0.5f,  0.5f,  0.33f,
    -0.5f, -0.5f, -0.5f,  0.5f,  0.0f,

    // Top face (y = +0.5)
    -0.5f,  0.5f, -0.5f,  0.5f,  1.0f,
    0.5f,  0.5f,  0.5f,  0.25f, 0.66f,
    0.5f,  0.5f, -0.5f,  0.25f, 1.0f,
    0.5f,  0.5f,  0.5f,  0.25f, 0.66f,
    -0.5f,  0.5f, -0.5f,  0.5f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.5f,  0.66f
};

const unsigned int Block::MAX_BLOCK_COUNT = 100000;

bool Block::isDataLoaded = false;
GLuint Block::vao;
glm::mat4 *Block::instanceData;
GLuint Block::texture;
std::unique_ptr<Shader> Block::shader;

Block::Block(glm::vec3 pos) : 
    pos(pos)
{
    if (!isDataLoaded)
    {
        isDataLoaded = true;
        LoadData();
    }
}

void Block::Render()
{

    auto start = std::chrono::high_resolution_clock::now();

    shader->Use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glBindVertexArray(vao);

    shader->SetMat4("view", camera->GetViewMatrix());
    shader->SetMat4("projection", camera->GetProjectionMatrix());

    assert(blocks.size() <= MAX_BLOCK_COUNT);

    size_t i = 0;
    glm::vec4 *frustumPlanes = camera->GetFrustumPlanes();
    for (auto &pair : blocks) {
        if (pair.second->IsInFrustum(frustumPlanes))
        {
            glm::mat4 model(1.0f);
            model = glm::translate(model, pair.second->pos);
            instanceData[i++] = model;
        }
    }
    delete[] frustumPlanes;

    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, (GLsizei)i);

    auto finish = std::chrono::high_resolution_clock::now();
    std::stringstream logMsg;
    logMsg << "BLOCK render in us=" << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() << " blocks rendered=" << i;
    DebugLog(logMsg.str());
}

AABB Block::GetCollisionBox()
{
    AABB box;
    box.min = pos + glm::vec3(-0.5f, -0.5f, -0.5f);
    box.max = pos + glm::vec3( 0.5f,  0.5f,  0.5f);
    return box;
}

void Block::LoadData()
{
    shader = std::unique_ptr<Shader>(new Shader("src/shaders/block.vs", "src/shaders/block.fs"));


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


    GLuint instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    GLsizeiptr instVBOBufferSize = MAX_BLOCK_COUNT * sizeof(glm::mat4);
    glBufferStorage(
        GL_ARRAY_BUFFER,
        instVBOBufferSize,
        nullptr,
        GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT
    );
    instanceData = (glm::mat4*) glMapBufferRange(
        GL_ARRAY_BUFFER,
        0,
        instVBOBufferSize,
        GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT
    );

    // instance model
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(3 + i);
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * sizeof(glm::vec4)));
        glVertexAttribDivisor(3 + i, 1);
    }

    glBindVertexArray(0);


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

    shader->Use();
    shader->SetInt("texture1", 0); // texture bank (0 = GL_TEXTURE0)

    DebugLog("BLOCK finished loading data.");
}

bool Block::IsInFrustum(glm::vec4 *frustumPlanes)
{
    for (int i = 0; i < 6; i++) {
        glm::vec3 normal = glm::vec3(frustumPlanes[i]);
        float d = frustumPlanes[i].w;

        AABB collisionBox = GetCollisionBox();

        glm::vec3 positiveVertex = collisionBox.min;

        if (normal.x >= 0) positiveVertex.x = collisionBox.max.x;
        if (normal.y >= 0) positiveVertex.y = collisionBox.max.y;
        if (normal.z >= 0) positiveVertex.z = collisionBox.max.z;

        if (glm::dot(normal, positiveVertex) + d < 0)
            return false;
    }

    return true;
}
