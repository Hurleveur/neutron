// these are used everywhere so only include them once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <learnopengl/camera.h>

#include <iostream>

#ifndef CONSTANTS
#define CONSTANTS

// screen dimensions
constexpr unsigned SCR_WIDTH = 800;
constexpr unsigned SCR_HEIGHT = 600;

float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

#endif