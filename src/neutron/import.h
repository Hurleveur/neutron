#include <glm/glm.hpp>
#include <vector>
#include <cmath>
#include <glad/glad.h>
#include <glm/glm.hpp>

#pragma once
class Shader;
// Skybox management.
void makeSkybox(Shader &skyboxShader);
void drawSkybox(Shader &skyboxShader, glm::mat4 &view, glm::mat4 &projection);
// Particles management.
void makeParticles(Shader &particleShader);
void drawParticles(Shader &particleShader, float deltaTime);

// For each planet, that have corresponding textures.
enum Planets {
    Sun,
    Earth,
    Moon,
	Mars,
	Mercury,
};


class Planet {
public:
	Planet(int mass, float radius, double posX, double posY, double posZ, double speedX, double speedY, double speedZ, Shader& planetShader, int image);

	void Tick(double time);
	double DistanceFrom(const Planet& object) const;
	void makePlanet(Shader& planetShader, int image);
	void draw(Shader& planetShader, double time);

public:
	int mass = 100;
	float radius = 1.f;
	double x;
	double y;
	double z;
	double vX;
	double vY;
	double vZ;
	glm::vec3 rotation = {1.f, 1.f, 1.f};

	GLuint VAO, VBO[4], planetTextureID, normalMapID, specMapID;
};
