#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

const float SPEED = 2.5f;
const glm::vec3 WORLD_UP = glm::vec3(0.0f, 1.0f, 0.0f);
const float MOUSE_SENSITIVITY = 0.2f;
const float ZOOM_SENSITIVITY = 5.0f;

class Camera
{
public:

    enum Movement
    {
        FORWARDS,
        BACKWARDS,
        LEFT,
        RIGHT
    };

    glm::vec3 Pos = glm::vec3(0.0, 0.0, 0.0);

    Camera()
    {
        updateCameraVectors();
    }

    glm::mat4 GetViewMatrix()
    {
        // ATTENTION: The lookAt function with a fixed cameraUp will break if the 
        // camera's pitch is close to vertical, or if the camera rolls.
        return glm::lookAt(Pos, Pos + front, WORLD_UP);
    }

    glm::mat4 GetProjectionMatrix(float screenWidth, float screenHeight)
    {
        return glm::perspective(glm::radians(fov), screenWidth / screenHeight, 0.1f, 100.0f);
    }

    void ProcessKeyboard(Movement direction, float deltaTime)
    {
        switch (direction)
        {
        case FORWARDS:
            Pos += SPEED * deltaTime * front;
            break;
        case BACKWARDS:
            Pos -= SPEED * deltaTime * front;
            break;
        case LEFT:
            Pos -= SPEED * deltaTime * right;
            break;
        case RIGHT:
            Pos += SPEED * deltaTime * right;
            break;
        default:
            break;
        }
    }

    void ProcessMouseMovement(float xOffset, float yOffset)
    {
        xOffset *= MOUSE_SENSITIVITY;
        yOffset *= MOUSE_SENSITIVITY;

        yaw   += xOffset;
        pitch -= yOffset;

        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.9f;

        updateCameraVectors();
    }

    void ProcessMouseScroll(float yOffset)
    {
        fov -= (float)yOffset * ZOOM_SENSITIVITY;
        if (fov < 1.0f)
            fov = 1.0f;
        if (fov > 45.0f)
            fov = 45.0f; 
    }

private:

    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 right;
    float fov = 45.0f;
    float yaw = -90.0f;
    float pitch = 0.0f;

    void updateCameraVectors()
    {
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(front);

        right = glm::normalize(glm::cross(front, WORLD_UP));
    }
};
