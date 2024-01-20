/*******************************************************************
** This code has been adapted from 2D to 3D and then modified from the LearnOpenGL tutorials's Breakout game.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "particle_generator.h"
#include "import.h"
#include <learnopengl/shader_m.h>

using namespace glm;
using namespace std;

ParticleGenerator::ParticleGenerator(GLuint texture, unsigned int amount)
    : texture(texture), amount(amount)
{
    init();
}

void ParticleGenerator::Update(float delta, unsigned int newParticles, vec3 offset)
{
    for (unsigned int i = 0; i < newParticles; ++i) {
        int unusedParticle = firstUnusedParticle();
        respawnParticle(particles[unusedParticle], offset);
    }
    for (unsigned int i = 0; i < amount; ++i) {
        Particle &p = particles[i];
        p.life -= delta;
        if (p.life > 0.0f)  { // move particle while alive
            p.velocity *= (1 - delta);
            p.position -= p.velocity * delta;
            //p.color.x -= delta * 2.5f;
        }
    }
}

// render all particles
void ParticleGenerator::Draw(Shader &shader)
{
    // make them spottable even behind other objects like the sun
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    shader.use();
    for (const Particle &particle : particles)
    {
        if (particle.life > 0.0f) {
            shader.setVec3("offset", particle.position);
            // make the size proportionate to the time left
            shader.setFloat("scale", particle.life / 8);
            //shader.setVec4("color", particle.color);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }
    }
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}

void ParticleGenerator::init()
{
    // set up mesh and attribute properties
    unsigned int VBO;
    float particle_quad[] = {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    // fill mesh buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
    // set mesh attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    // create amount default particle instances
    for (unsigned int i = 0; i < amount; ++i)
        particles.push_back(Particle());
}

// stores the index of the last particle used (for quick access to next dead particle)
unsigned int lastUsedParticle = 0;
unsigned int ParticleGenerator::firstUnusedParticle()
{
    // first search from last used particle, this will usually return almost instantly
    for (unsigned int i = lastUsedParticle; i < amount; ++i){
        if (particles[i].life <= 0.0f){
            lastUsedParticle = i;
            return i;
        }
    }
    // otherwise, do a linear search
    for (unsigned int i = 0; i < lastUsedParticle; ++i){
        if (particles[i].life <= 0.0f){
            lastUsedParticle = i;
            return i;
        }
    }
    // all particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
    lastUsedParticle = 0;
    return 0;
}

void ParticleGenerator::respawnParticle(Particle &particle, glm::vec3 offset)
{
    // we don't need the color, but they work
    //float rColor = ((rand() % 100) / 100.0f) - 0.3f;
    particle.position = glm::vec3((rand() % 100)/10.0f + offset.x, (rand() % 100)/10.0f + offset.y, (rand() % 100)/10.0f + offset.z);
    //particle.color = glm::vec4(rColor, rColor > 0.75f ? rColor / 2.f : 0.f, 0, 1.f);
    particle.life = 1.0f;
    particle.velocity = glm::vec3((rand() % 20) - 10, (rand() % 20) - 10, (rand() % 20) - 10) / 2.f;
}
