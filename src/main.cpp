#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "Game.hpp"
#include "GameState.hpp"

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ToggleWireframeMode();

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
    glfwSetCursorPosCallback(window, MouseMoveCallback);
    glfwSetMouseButtonCallback(window, MouseClickCallback);
    glfwSetScrollCallback(window, ScrollWheelCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    //

    StartGame();

    //
    
    while (!glfwWindowShouldClose(window)) {
        
        double currentTime = glfwGetTime();
        deltaTime = (float)(currentTime - lastFrame);
        lastFrame = currentTime;

        //

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

        ProcessInput(window);
        
        UpdateGame();

        //

        glClearColor(0.3f, 0.62f, 0.89f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        RenderGame();

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
