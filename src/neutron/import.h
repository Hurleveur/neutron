#include <glm/glm.hpp>
#include <vector>
#include <cmath>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#pragma once
class Shader;
void makeSkybox(Shader &skyboxShader);

void drawSkybox(Shader &skyboxShader, glm::mat4 &view, glm::mat4 &projection);


class SpaceObject {
public:
	SpaceObject(int mass, float radius, int posX, int posY, int posZ, int speedX, int speedY, int speedZ);

	void Tick(double time);
	double DistanceFrom(const SpaceObject& object) const;

public:
	int mass = 1000;
	float radius = 1;
	int x;
	int y;
	int z;
	int vX;
	int vY;
	int vZ;

	static std::vector<SpaceObject*> objectList;
	static SpaceObject* biggestMass;
};


class Planet : public SpaceObject {
public:
    Planet(int mass, float radius, int posX, int posY, int posZ, int speedX, int speedY, int speedZ, Shader &planetShader, const char *image);
    void makePlanet(Shader& planetShader, const char *image);
    void draw(Shader& planetShader, glm::mat4& view, glm::mat4& projection);

public:
	// Generate mipmapped texture
// create vao and vbo
	GLuint VAO, VBO[4], planetTextureID;
	std::vector<float> vertices, normals, texCoords;
	std::vector<unsigned int> indices;
};