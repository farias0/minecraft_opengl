#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GameState.hpp"
#include "Player.hpp"

const float MOVE_SPEED = 7.0f;
const glm::vec3 WORLD_UP = glm::vec3(0.0f, 1.0f, 0.0f);
const float MOUSE_SENSITIVITY = 0.1f;
const float ZOOM_SENSITIVITY = 5.0f;
const float FOV_DEFAULT = 60.0f;

class Camera
{
public:
    glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 Right;

    Camera()
    {
        UpdateCameraVectors();
    }

    glm::vec3 LookingAt()
    {
        return GetPos() + Front;
    }

    glm::mat4 GetViewMatrix()
    {
        // ATTENTION: The lookAt function with a fixed cameraUp will break if the 
        // camera's pitch is close to vertical, or if the camera rolls.
        return glm::lookAt(GetPos(), LookingAt(), WORLD_UP);
    }

    glm::mat4 GetProjectionMatrix()
    {
        return glm::perspective(glm::radians(fov), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);
    }

    void ProcessMouseMovement(float xOffset, float yOffset)
    {
        xOffset *= MOUSE_SENSITIVITY;
        yOffset *= MOUSE_SENSITIVITY;

        yaw   += xOffset;
        pitch -= yOffset;

        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.9f;

        UpdateCameraVectors();
    }

    void ProcessMouseScroll(float yOffset)
    {
        fov -= (float)yOffset * ZOOM_SENSITIVITY;
        if (fov < 1.0f)
            fov = 1.0f;
        if (fov > FOV_DEFAULT)
            fov = FOV_DEFAULT; 
    }

private:
    float fov = FOV_DEFAULT;
    float yaw = -90.0f;
    float pitch = 0.0f;

    glm::vec3 GetPos()
    {
        return player->Pos;
    }

    void UpdateCameraVectors()
    {
        Front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        Front.y = sin(glm::radians(pitch));
        Front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        Front = glm::normalize(Front);

        Right = glm::normalize(glm::cross(Front, WORLD_UP));
    }
};
