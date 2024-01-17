#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

// This file creates and then manages objects, with the help of particle_generator.h in the case of particles, and planet.h to generate spheres.
// texure.h is also used to import textures.

class Shader;
// Skybox management.
void makeSkybox(Shader &skyboxShader);
void drawSkybox(Shader &skyboxShader, const glm::mat4 &view, const glm::mat4 &projection);
// Particles management.
void makeParticles(Shader &particleShader);
void drawParticles(Shader &particleShader, float deltaTime);

class Planet {
public:
	// For each planet, that have corresponding textures.
	enum class Type
	{
		Sun,
		Earth,
		Moon,
		Mars,
		Mercury,
	};

	Planet(int mass, float radius, double posX, double posY, double posZ, double speedX, double speedY, double speedZ, Shader& planetShader, Type type);

	void Tick(double time);
	float DistanceFrom(const Planet& object) const;
	void makePlanet(const Shader& planetShader, Type type);
	void draw(const Shader& planetShader, double time);

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
