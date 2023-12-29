#include "textures.h"
#include "import.h"
#include <learnopengl/filesystem.h>
#include <iostream>
#include <glad/glad.h>
#include <learnopengl/shader.h>
#include "planet.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

constexpr float skyboxVertices[] = {
    // positions
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};
unsigned int skyboxVAO, skyboxVBO;
unsigned int cubemapTexture;



void makeSkybox(Shader &skyboxShader)
{
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    std::vector<std::string> faces
    {
        FileSystem::getPath("resources/textures/skybox/skybox_left.png"),
        FileSystem::getPath("resources/textures/skybox/skybox_right.png"),
        FileSystem::getPath("resources/textures/skybox/skybox_up.png"),
        FileSystem::getPath("resources/textures/skybox/skybox_down.png"),
        FileSystem::getPath("resources/textures/skybox/skybox_front.png"),
        FileSystem::getPath("resources/textures/skybox/skybox_back.png")
    };
    cubemapTexture = loadCubemap(faces);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);
}

void drawSkybox(Shader &skyboxShader, glm::mat4 &view, glm::mat4 &projection)
{
    // draw skybox as last
    glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
    skyboxShader.use();
    skyboxShader.setMat4("view", glm::mat3(view));
    skyboxShader.setMat4("projection", projection);
    // skybox cube
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // set depth function back to default
}

SpaceObject* SpaceObject::biggestMass = nullptr;

SpaceObject::SpaceObject(int mass, float radius, double posX, double posY, double posZ, double speedX, double speedY, double speedZ) :
    mass(mass), radius(radius), x(posX), y(posY), z(posZ), vX(speedX), vY(speedY), vZ(speedZ) {
    if (!biggestMass || this->mass > biggestMass->mass)
        biggestMass = this;
}



void SpaceObject::Tick(double time) {
    x += vX;
    y += vY;
    z += vZ;
}


double SpaceObject::DistanceFrom(const SpaceObject& object) const
{
    return std::sqrt(
        (object.x - x) * (object.x - x) +
        (object.y - y) * (object.y - y) +
        (object.z - z) * (object.z - z)
    );
}

Planet::Planet(int mass, float radius, double posX, double posY, double posZ, double speedX, double speedY, double speedZ, Shader& planetShader, const char *image) :
    SpaceObject(mass, radius, posX, posY, posZ, speedX, speedY, speedZ) {
    this->makePlanet(planetShader, image);
};

void Planet::makePlanet(Shader& planetShader, const char *image)
{
    int sectorCount = 36;
    int stackCount = 18;

    generateSphere(1.0f, sectorCount, stackCount, vertices, normals, texCoords, indices);
    planetTextureID = generateMipmappedTexture(FileSystem::getPath(image).c_str());
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(4, VBO);

    // Bind and set vertex data (position, normal, and texture coordinates)
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(float), texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    planetShader.use();
    planetShader.setInt(image, 0);
}


void Planet::draw(Shader &planetShader, glm::mat4& view, glm::mat4& projection, glm::vec3& camera, bool star)
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    // planet
    planetShader.use();
    planetShader.setMat4("view", view);
    planetShader.setMat4("projection", projection);
    planetShader.setFloat("scale", radius);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(this->x, this->y, this->z));
    if(this->vX + this->vY + this->vZ)
        model = glm::rotate(model, 360.f, glm::vec3(this->vX, this->vY, this->vZ));
    //model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
    planetShader.setMat4("model", model);

    if (star)
    {
        // light properties
        planetShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        planetShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        planetShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        planetShader.setFloat("light.constant", 1.0f);
        planetShader.setFloat("light.linear", 0.007f);
        planetShader.setFloat("light.quadratic", 0.0002f);
        planetShader.setVec3("light.position", glm::vec3(this->x, this->y, this->z));

        planetShader.setVec3("viewPos", camera);
        // material properties
        planetShader.setInt("material.diffuse", 0);
        planetShader.setInt("material.specular", 1);
        planetShader.setFloat("material.shininess", 50.0f);
    }

    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, planetTextureID);
    // Render the sphere
    glDrawElements(GL_TRIANGLES, (unsigned int)indices.size(), GL_UNSIGNED_INT, indices.data());
    glBindVertexArray(0);
}
