#include <glad/glad.h>
#include <GLFW/glfw3.h>
#pragma once

// These functions are not specic to this one game and just help setting an interactive window up.

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
GLFWwindow* init();
