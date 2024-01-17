#include "textures.h"
#include "import.h"
#include "planet.h"
#include "particle_generator.h"

#include <glad/glad.h>
#include <learnopengl/shader.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

// TODO: put this inside a PlanetFactory class, instead of this being in global scope, which is BAAAAD :(
int sectorCount = 36;
int stackCount = 18;
std::vector<glm::vec3> vertices;
std::vector<glm::vec3> normals;
std::vector<glm::vec2> texCoords;
std::vector<glm::vec3> tangents;
std::vector<Triangle> triangles;

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

ParticleGenerator *Particles;


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
        "textures/skybox/skybox_left.png",
        "textures/skybox/skybox_right.png",
        "textures/skybox/skybox_up.png",
        "textures/skybox/skybox_down.png",
        "textures/skybox/skybox_front.png",
        "textures/skybox/skybox_back.png"
    };
    cubemapTexture = loadCubemap(faces);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);
}

void drawSkybox(Shader &skyboxShader, const mat4 &view, const mat4 &projection)
{
    glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
    skyboxShader.use();
    skyboxShader.setMat4("view", mat3(view));
    skyboxShader.setMat4("projection", projection);
    // skybox cube
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS); // set depth function back to default
}



void makeParticles(Shader& particleShader) {
    particleShader.use();
    particleShader.setInt("particle", 0);
    // make 100 of them to start with
    Particles = new ParticleGenerator(generateMipmappedTexture("textures/particles.png"), 200);
}

void drawParticles(Shader& particleShader, float deltaTime) {
    if(deltaTime)
        Particles->Update(deltaTime, 4);
    Particles->Draw(particleShader);
}



// Planet (part that would be common to multiple space objects first)
void Planet::Tick(double time) {
    x += vX;
    y += vY;
    z += vZ;
}


float Planet::DistanceFrom(const Planet& object) const
{
    return std::sqrt(
        std::fabs( // TODO: does this actually do anything?
            (object.x - x) * (object.x - x) +
            (object.y - y) * (object.y - y) +
            (object.z - z) * (object.z - z)
        )
    );
}


Planet::Planet(int mass, float radius, double posX, double posY, double posZ, double speedX, double speedY, double speedZ, Shader& planetShader, Type type) :
    mass(mass), radius(radius), x(posX), y(posY), z(posZ), vX(speedX), vY(speedY), vZ(speedZ)
{
    makePlanet(planetShader, type);
};

void Planet::makePlanet(const Shader& planetShader, Type type)
{
    std::string textureFile;
    std::string name;
    switch (type) {
        case Type::Sun:
            textureFile = "textures/planets/sun/";
            name = "sun.jpg";
            break;
        case Type::Moon:
            textureFile = "textures/planets/moon/";
            name = "moon.bmp";
            break;
        case Type::Mercury:
            textureFile = "textures/planets/mercury/";
            name = "mercury.png";
            break;
        case Type::Mars:
            textureFile = "textures/planets/mars/";
            name = "mars.png";
            break;
        case Type::Earth:
        default:
            textureFile = "textures/planets/earth/";
            name = "earth.jpg";
            break;
    }
    planetTextureID = generateMipmappedTexture(textureFile + name);
    normalMapID = loadNormalMap(textureFile + "norm.png");
    specMapID = loadNormalMap(textureFile + "spec.png");

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // generate a sphere once and then use it for all VBO and VAO
    if (vertices.empty())
        generateSphere(1.0f, sectorCount, stackCount, vertices, normals, texCoords, triangles, tangents);

    glGenBuffers(4, VBO);

    // Bind and set vertex data (position, normal, texture coordinates and tangents)
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(glm::vec3), tangents.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(3);
    glBindVertexArray(0);

    planetShader.use();
    //planetShader.setInt(texture, 0);
    planetShader.setInt("material.diffuse", 0);
    planetShader.setInt("material.normal", 1);
    planetShader.setInt("material.specular", 2);
}


void Planet::draw(const Shader& planetShader, double time)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, planetTextureID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalMapID);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, specMapID);
    glBindVertexArray(VAO);

    // identity matrix
    mat4 model = mat4(1.0f);

    // uncomment for fun mode (super rotations, but physics dont make much sense bc it isnt the true object positions)
    //model = rotate(model, rotation.x / 2, vec3(1.f, 0.f, 0.f));
    //model = rotate(model, rotation.y / 2, vec3(0.f, 1.f, 0.f));
    //model = rotate(model, rotation.z / 2, vec3(0.f, 0.f, 1.f));

    model = translate(model, vec3(this->x, this->y, this->z));
    model = scale(model, vec3(radius));

    if(time)
        rotation += vec3(this->vX / 2.f, this->vY / 2.f, this->vZ / 2.f);
    model = rotate(model, rotation.x, vec3(1.f, 0.f, 0.f));
    model = rotate(model, rotation.y, vec3(0.f, 1.f, 0.f));
    model = rotate(model, rotation.z, vec3(0.f, 0.f, 1.f));
    

    planetShader.setMat4("model", model);
    // Render the sphere
    glDrawElements(GL_TRIANGLES, (uint32_t)(triangles.size() * sizeof(Triangle)), GL_UNSIGNED_INT, triangles.data());
}
