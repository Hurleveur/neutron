/*******************************************************************
** This code is part of Breakout.
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

ParticleGenerator::ParticleGenerator(Shader shader, GLuint texture, unsigned int amount)
    : shader(shader), texture(texture), amount(amount)
{
    this->init();
}

void ParticleGenerator::Update(float delta, unsigned int newParticles, vec3 offset)
{
    // printf("ok\n");
    // add new particles
    for (unsigned int i = 0; i < newParticles; ++i) {
        int unusedParticle = this->firstUnusedParticle();
        this->respawnParticle(this->particles[unusedParticle], offset);
    }
    for (unsigned int i = 0; i < this->amount; ++i) {
        Particle &p = this->particles[i];
        p.Life -= delta;
        if (p.Life > 0.0f)  {	                    // move particle while alive
            p.Position -= p.Velocity * delta;
            p.Color.a -= delta * 2.5f;
            if (p.Life < 0.1f) { p.Scale = 0.01f; } // reduce size before despawn
        }
    }
}

// render all particles
void ParticleGenerator::Draw(mat4 view, mat4 projection)
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    this->shader.use();
    for (Particle particle : this->particles)
    {
        if (particle.Life > 0.0f) {
            //printf("Particle: %f\n", particle.Life);
            this->shader.setVec3("offset", particle.Position);
            this->shader.setMat4("view", view);
            this->shader.setMat4("projection", projection);
            this->shader.setFloat("scale", particle.Scale);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, this->texture);
            glBindVertexArray(this->VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }
    }
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
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(this->VAO);
    // fill mesh buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
    // set mesh attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    // create this->amount default particle instances
    for (unsigned int i = 0; i < this->amount; ++i)
        this->particles.push_back(Particle());
}

// stores the index of the last particle used (for quick access to next dead particle)
unsigned int lastUsedParticle = 0;
unsigned int ParticleGenerator::firstUnusedParticle()
{
    // first search from last used particle, this will usually return almost instantly
    for (unsigned int i = lastUsedParticle; i < this->amount; ++i){
        if (this->particles[i].Life <= 0.0f){
            lastUsedParticle = i;
            return i;
        }
    }
    // otherwise, do a linear search
    for (unsigned int i = 0; i < lastUsedParticle; ++i){
        if (this->particles[i].Life <= 0.0f){
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
    float random = (rand() % 100)/100.0f;
    float rColor = 0.5f + ((rand() % 100) / 100.0f);
    particle.Position = glm::vec3((rand() % 100)/100.0f + offset.x, (rand() % 100)/100.0f + offset.y, (rand() % 100)/100.0f + offset.z);
    particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
    particle.Life = 1.0f;
    particle.Scale = 0.02f;
    particle.Velocity = glm::vec3((rand() % 20) - 10, (rand() % 20) - 10, (rand() % 20) - 10);
}