/*******************************************************************
** This code has been adapted from 2D to 3D and then modified from the LearnOpenGL tutorials's Breakout game.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "import.h"

class Shader;

// Represents a single particle and its state
struct Particle {
    glm::vec3 position, velocity;
    glm::vec4 color;
    float life = 0.f;

    Particle() : position(0.0f), velocity(0.0f), color(1.0f) { }
};


// ParticleGenerator acts as a container for rendering a large number of
// particles by repeatedly spawning and updating particles and killing
// them after a given amount of time.
class ParticleGenerator
{
public:
    // constructor
    ParticleGenerator(GLuint texture, unsigned int amount);
    // update all particles
    void Update(float dt, unsigned int newParticles, glm::vec3 offset = glm::vec3(-5.0f, -5.0f, -5.0f));
    // render all particles
    void Draw(Shader &shader);
private:
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
    void respawnParticle(Particle &particle, glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f));
};

#endif