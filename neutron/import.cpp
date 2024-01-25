#include "import.h"

#include "textures.h"
#include "planet.h"
#include "particle_generator.h"

#include <glad/glad.h>
#include <learnopengl/shader.h>

// nge headers
#include "nge_math.hh"

constexpr float skyboxVertices[] =
{
	// positions
	-1.0f, 1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,

	-1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,

	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,

	-1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, 1.0f
};
unsigned int skyboxVAO, skyboxVBO;
unsigned int cubemapTexture;

ParticleGenerator* Particles;


void makeSkybox(const Shader& skyboxShader)
{
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
	const std::initializer_list<const std::string_view> faces
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

void drawSkybox(const Shader& skyboxShader, const mat4& view, const mat4& projection)
{
	skyboxShader.use();
	skyboxShader.setMat4("view_projection_matrix", mat3(projection * view));
	// skybox cube
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void makeParticles(const Shader& particleShader)
{
	particleShader.use();
	particleShader.setInt("particle", 0);
	// make 100 of them to start with
	Particles = new ParticleGenerator(generateMipmappedTexture("textures/particles.png"), 200);
}

void drawParticles(const Shader& particleShader, const nge::timing::Seconds delta_time)
{
	if (delta_time)
		Particles->Update(delta_time, 4);
	Particles->Draw(particleShader);
}


// Planet (part that would be common to multiple space objects first)
void Planet::Tick(double time)
{
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


Planet::Planet(int mass, float radius, double posX, double posY, double posZ, double speedX, double speedY,
               double speedZ, Shader& planetShader, Type type) : mass(mass), radius(radius), x(posX), y(posY), z(posZ),
                                                                 vX(speedX), vY(speedY), vZ(speedZ)
{
	makePlanet(planetShader, type);
};

void Planet::makePlanet(const Shader& planetShader, Type type)
{
	std::string textureFile;
	std::string name;
	switch (type)
	{
		case Type::Sun:
			textureFile = "textures/planets/sun/";
			name = "sun.jpg";
			break;
		case Type::Moon:
			textureFile = "textures/planets/moon/";
			name = "moon.jpg";
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

	planetShader.use();
	//planetShader.setInt(texture, 0);
	planetShader.setInt("material.diffuse", 0);
	planetShader.setInt("material.normal", 1);
	planetShader.setInt("material.specular", 2);
}


void Planet::SetShaderVariables(const Shader& planetShader, const nge::timing::Seconds time)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, planetTextureID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalMapID);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, specMapID);

	// identity matrix
	mat4 model = mat4(1.0f);

	// uncomment for fun mode (super rotations, but physics dont make much sense bc it isnt the true object positions)
	//model = rotate(model, rotation.x / 2, vec3(1.f, 0.f, 0.f));
	//model = rotate(model, rotation.y / 2, vec3(0.f, 1.f, 0.f));
	//model = rotate(model, rotation.z / 2, vec3(0.f, 0.f, 1.f));

	model = translate(model, vec3(this->x, this->y, this->z));
	model = scale(model, vec3(radius));

	if (time)
		rotation += vec3(this->vX / 2.f, this->vY / 2.f, this->vZ / 2.f);
	model = rotate(model, rotation.x, vec3(1.f, 0.f, 0.f));
	model = rotate(model, rotation.y, vec3(0.f, 1.f, 0.f));
	model = rotate(model, rotation.z, vec3(0.f, 0.f, 1.f));
	planetShader.setMat4("model_to_world_matrix", model);
}
