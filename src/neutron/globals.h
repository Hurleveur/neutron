// these are used everywhere so only include them once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <learnopengl/camera.h>
#include <iostream>

#include <vector>
#include <string>


#pragma once

// screen dimensions
constexpr unsigned SCR_WIDTH = 800;
constexpr unsigned SCR_HEIGHT = 600;

static float lastX = (float)SCR_WIDTH / 2.0;
static float lastY = (float)SCR_HEIGHT / 2.0;

// timing
static float deltaTime = 0.0f;
static float lastFrame = 0.0f;
static Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
