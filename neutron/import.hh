#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

// nge headers
#include "nge_timing.hh"

// This file creates and then manages objects, with the help of particle_generator.h in the case of particles, and planet.h to generate spheres.
// texure.h is also used to import textures.

class Shader;
// Skybox management.
void makeSkybox(const Shader &skyboxShader);
void drawSkybox(const Shader &skyboxShader, const glm::mat4 &view, const glm::mat4 &projection);
// Particles management.
void makeParticles(const Shader &particleShader);
void drawParticles(const Shader &particleShader, const nge::timing::Seconds deltaTime);

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
	void SetShaderVariables(const Shader& planetShader, const nge::timing::Seconds time);

public:
	int mass = 100;
	float radius = 1.f;
	double x;
	double y;
	double z;
	double vX;
	double vY;
	double vZ;
	glm::vec3 rotation = {0.f, 0.f, 0.f};

	GLuint planetTextureID, normalMapID, specMapID;
};
