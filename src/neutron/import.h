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

enum Planets {
    Sun,
    Earth,
    Moon,
};


class SpaceObject {
public:
	SpaceObject(int mass, float radius, double posX, double posY, double posZ, double speedX, double speedY, double speedZ);

	void Tick(double time);
	double DistanceFrom(const SpaceObject& object) const;

public:
	int mass = 1000;
	float radius;
	double x;
	double y;
	double z;
	double vX;
	double vY;
	double vZ;

	static std::vector<SpaceObject*> objectList;
	static SpaceObject* biggestMass;
};


class Planet : public SpaceObject {
public:
    Planet(int mass, float radius, double posX, double posY, double posZ, double speedX, double speedY, double speedZ, Shader &planetShader, int image);
    void makePlanet(Shader& planetShader, int image);
    void draw(Shader& planetShader, Shader& lightningShader, glm::mat4& view, glm::mat4& projection, Camera camera, bool star = false);

public:
	// Generate mipmapped texture
// create vao and vbo
	GLuint VAO, VBO[4], planetTextureID, normalMapID;
	std::vector<float> vertices, normals, texCoords;
	std::vector<unsigned int> indices;
};