#include <glm/glm.hpp>
#include <vector>
#include <cmath>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <learnopengl/camera.h>

#pragma once
class Shader;
void makeSkybox(Shader &skyboxShader);
void drawSkybox(Shader &skyboxShader, glm::mat4 &view, glm::mat4 &projection);
void makeParticles(Shader &particleShader);
void drawParticles(Shader &particleShader, float deltaTime, glm::mat4 view, glm::mat4 projection);

enum Planets {
    Sun,
    Earth,
    Moon,
	Mars,
	Mercury,
};


class SpaceObject {
public:
	SpaceObject(int mass, float radius, double posX, double posY, double posZ, double speedX, double speedY, double speedZ);

	void Tick(double time);
	double DistanceFrom(const SpaceObject& object) const;

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

	static std::vector<SpaceObject*> objectList;
};

class Planet : public SpaceObject {
public:
    Planet(int mass, float radius, double posX, double posY, double posZ, double speedX, double speedY, double speedZ, Shader &planetShader, int image);
    void makePlanet(Shader& planetShader, int image);
    void draw(Shader& planetShader);
public:
	GLuint VAO, VBO[4], planetTextureID, normalMapID, specMapID;
	std::vector<float> vertices, normals, texCoords, tangents;
	std::vector<unsigned int> indices;
};
