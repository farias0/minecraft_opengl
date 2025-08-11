#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <stb_image.h>

#include "Camera.hpp"
#include "Cube.hpp"
#include "Game.hpp"
#include "Shader.hpp"

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ProcessInput(GLFWwindow *window);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset);

int main() {

    GLFWwindow* window;

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
                
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Minecraft with OpenGL", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetScrollCallback(window, ScrollCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    //

    camera = std::unique_ptr<Camera>(new Camera());

    Cube cube = Cube(glm::vec3(0, -2, -5));

    //
    
    while (!glfwWindowShouldClose(window)) {
        
        double currentTime = glfwGetTime();
        deltaTime = (float)(currentTime - lastFrame);
        lastFrame = currentTime;

        ProcessInput(window);
        
        //

        glClearColor(0.3f, 0.62f, 0.89f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        cube.Render();

        glfwSwapBuffers(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //

        glfwPollEvents();
    }

    //

    glfwTerminate();
    return 0;
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    window;
    glViewport(0, 0, width, height);
}

void ToggleWireframeMode() {
    static bool wireframeMode = false;
    wireframeMode = !wireframeMode;
    glPolygonMode(GL_FRONT_AND_BACK, wireframeMode ? GL_LINE : GL_FILL);
}

void ProcessInput(GLFWwindow *window) {
    
    static bool isHoldingWireframe = false;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS && !isHoldingWireframe) {
        isHoldingWireframe = true;
        ToggleWireframeMode();
    }
    else if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_RELEASE) {
        isHoldingWireframe = false;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera->ProcessKeyboard(Camera::Movement::FORWARDS);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera->ProcessKeyboard(Camera::Movement::BACKWARDS);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->ProcessKeyboard(Camera::Movement::LEFT);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
       camera->ProcessKeyboard(Camera::Movement::RIGHT);
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
    window;

    static double lastX = xPos, lastY = yPos;
    float xOffset = (float)(xPos - lastX) * MOUSE_SENSITIVITY;
    float yOffset = (float)(yPos - lastY) * MOUSE_SENSITIVITY;
    lastX = xPos;
    lastY = yPos;

    camera->ProcessMouseMovement(xOffset, yOffset);
}

void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
    window; xOffset;
    camera->ProcessMouseScroll((float)yOffset);
}
