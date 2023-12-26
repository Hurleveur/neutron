#include <glm/glm.hpp>

#pragma once
class Shader;
void makeSkybox(Shader &skyboxShader);

void drawSkybox(Shader &skyboxShader, glm::mat4 view, glm::mat4 projection);
