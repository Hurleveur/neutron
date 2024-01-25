#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "import.h"

// std headers
#include <vector>

// nge headers
#include "nge_math.hh"

class Shader;

// Represents a single particle and its state
struct Particle
{
	nge::math::Vector3 position, velocity;
	nge::math::Vector4 color;
	float life;

	Particle():
		position(), velocity(), color(1.0f), life(0.0f)
	{}
};

// ParticleGenerator acts as a container for rendering a large number of
// particles by repeatedly spawning and updating particles and killing
// them after a given amount of time.
class ParticleGenerator
{
	// state
	std::vector<Particle> particles;
	unsigned int amount;
	// render state
	GLuint texture;
	unsigned int VAO;

	// initializes buffer and vertex attributes
	void init();

	// returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
	unsigned int firstUnusedParticle();

	// respawns particle
	void respawnParticle(Particle& particle, nge::math::Vector3 offset = nge::math::Vector3(0.0f, 0.0f, 0.0f));

public:
	ParticleGenerator(GLuint texture, unsigned int amount);

	// update all particles
	void Update(float dt, unsigned int newParticles, nge::math::Vector3 offset = nge::math::Vector3(-5.0f, -5.0f, -5.0f));

	// render all particles
	void Draw(const Shader& shader) const;
};
