//==============================================================================
//	Neutron Solar System
//	(C) 2024 Hurleveur
//==============================================================================

#pragma once

// neutron headers
#include "import.h"

// engine headers
#include "nge_graphics.hh"
#include "nge_timing.hh"
#include "nge_math.hh"

// LearnOpenGL headers (TODO: get rid of this)
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>

// std headers
#include <map>
#include <span>
#include <istream>
#include <fstream>
#include <cmath>

class NeutronGame final : public nge::WindowEventHandler
{
	bool needs_to_stop; // does the game need to quit immediately?
	Camera camera;
	bool firstMouse;

	nge::timing::Seconds deltaTime;
	nge::timing::Seconds lastFrame;

	bool stop;
	float stopTimeout;

	// the objects and if they should be rendered
	std::map<Planet *, bool> objectList;

	Shader skybox_shader;
	Shader particle_shader;
	Shader planet_shader;

	// sphere planet model, which is reused for every planet
	nge::graphics::Model planet_model;

	Planet sun;
	Planet mercury;
	Planet earth;
	Planet moon;
	Planet mars;

	static constexpr u32 EARTH_MOON_MASS = 100;
	static constexpr u32 SUN_MASS = 100000000;

	void ProcessKeyPress(const u32 key_code, const u32 action) override
	{
		if (key_code == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			needs_to_stop = true;
		if (key_code == GLFW_KEY_W && action == GLFW_PRESS) //glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS
			camera.ProcessKeyboard(FORWARD, deltaTime);
		if (key_code == GLFW_KEY_S && action == GLFW_PRESS)
			camera.ProcessKeyboard(BACKWARD, deltaTime);
		if (key_code == GLFW_KEY_A && action == GLFW_PRESS)
			camera.ProcessKeyboard(LEFT, deltaTime);
		if (key_code == GLFW_KEY_D && action == GLFW_PRESS)
			camera.ProcessKeyboard(RIGHT, deltaTime);

		if (key_code == GLFW_KEY_T && action == GLFW_PRESS && stopTimeout <= 0.f)
		{
			stopTimeout = .2f;
			stop = !stop;
		}
	}

	void ProcessMouseMotion(const float x, const float y) override
	{
		camera.ProcessMouseMovement(x, y);
	}

	void ProcessViewportResize(const nge::Extent2D& dimensions) override
	{
//		camera.ProcessViewportResize(width, height);
	}

	// factory function to create a Model given a file path to an MDL file
	static nge::graphics::Model CreateModelFromFile(const std::string_view& file_path)
	{
		std::ifstream mdl_file(file_path.data());
		if (!mdl_file.is_open())
			throw std::runtime_error("Failed to open an MDL file for reading.");

		// read vertices
		u32 vertex_count;
		mdl_file.read((char *) &vertex_count, sizeof(u32));
		std::vector<nge::graphics::VertexFormat> vertices;
		vertices.resize(vertex_count);
		mdl_file.read((char *) vertices.data(), vertex_count * sizeof(nge::graphics::VertexFormat));

		// read corner indices
		u32 vertex_index_count;
		mdl_file.read((char *) &vertex_index_count, sizeof(u32));
		std::vector<u16> vertex_indices;
		vertex_indices.resize(vertex_index_count);
		mdl_file.read((char *) vertex_indices.data(), vertex_index_count * sizeof(u16));

		return {vertices, vertex_indices};
	}

public:
	NeutronGame():
		needs_to_stop(false),
		camera(nge::math::Vector3(50.0f, 0.0f, 50.0f)),
		firstMouse(true),
		deltaTime(0.0f),
		lastFrame(0.0f),
		stop(false),
		stopTimeout(0.0f),
		objectList(),
		skybox_shader("shaders/skybox.vs", "shaders/skybox.fs"),
		particle_shader("shaders/particle.vs", "shaders/particle.fs"),
		planet_shader("shaders/shader.vs", "shaders/shader.fs"),
		planet_model(CreateModelFromFile("models/sphere.mdl")),
		sun(SUN_MASS, 5, 0, 0, 0, 0, 0, 0, planet_shader, Planet::Type::Sun),
		mercury(90, .5, 1.5, -30, 0, 0.0004, 0.00015, 0, planet_shader, Planet::Type::Mercury),
		earth(100, 1, 50, 0, 0, 0.0001, 0.0003, 0, planet_shader, Planet::Type::Earth),
		moon(1, .2, 51.5, -1.5, 0, 0.0001, 0.0003 + 0.00008, 0, planet_shader, Planet::Type::Moon),
		mars(60, .8, 1.5, -80, 0, -0.0005, 0.00004, 0, planet_shader, Planet::Type::Mars)
	{
		makeSkybox(skybox_shader);
		makeParticles(particle_shader);

		objectList[&sun] = true;
		objectList[&mercury] = true;
		objectList[&earth] = true;
		objectList[&moon] = true;
		objectList[&mars] = true;
	}

	bool Tick(const float delta_time)
	{
		deltaTime = delta_time;

		if (needs_to_stop)
			return false;

		// to prevent double stopping time in two consecutive frames (which results in not doing anything)
		if (stopTimeout > 0.f)
			stopTimeout -= delta_time;

		// steps gravity and movement with physics, and do collision detection + handling
		Step(delta_time);

		// we don't need to clear GL_COLOR_BUFFER_BIT due to the skybox being in the background
		glClear(GL_DEPTH_BUFFER_BIT);

		// Shader properties for all planets and the sun
		planet_shader.use();
		planet_shader.setVec3("light.position", 0.f, 0.f, 0.f);
		planet_shader.setVec3("viewPos", camera.Position);
		// full on sunlight - will be reduced after the sun is drawn
		planet_shader.setVec3("light.ambient", 1.f, 1.f, 1.f);
		planet_shader.setVec3("light.diffuse", .7f, .7f, .7f);
		planet_shader.setVec3("light.specular", .5f, .5f, .5f);

		// view and projection matrix, with a depth sufficient so it doesn't crop objects - both are also multiplied here in advanced for optimisation
		const nge::math::Matrix4& view = camera.GetViewMatrix();
		const nge::math::Matrix4 projection = glm::perspective(glm::radians(camera.Zoom), (float) 1600 / (float) 1000, 0.1f, 1024.0f);
		const nge::math::Matrix4 viewProj = projection * view;
		planet_shader.setMat4("view_projection_matrix", viewProj);

		// render all objects (model is calculated in the draw function itself)
		planet_model.Set(); // TODO: encapsulate inside a PlanetRenderer class
		for (const auto& object: objectList)
		{
			if (object.second)
				object.first->SetShaderVariables(planet_shader, stop ? 0. : delta_time);
			planet_model.Draw();

			// only the sun needs to be super bright, and its drawn first
			planet_shader.setVec3("light.ambient", .2f, .2f, .2f);
		}
		planet_model.Unset(); // TODO: encapsulate inside a PlanetRenderer class

		particle_shader.use();
		particle_shader.setMat4("view_projection_matrix", viewProj);
		drawParticles(particle_shader, stop ? 0 : delta_time);

		// draw skybox as last (optimisation, all depth buffer have been +- filled now)
		drawSkybox(skybox_shader, view, projection);

		return true;
	}

	void Step(const nge::timing::Seconds time)
	{
		if (stop)
			return;

		// gravitational constant
		constexpr double GRAVITATIONAL = 6.674 / 100000000000;
		// apply gravity
		// auto is Planet, bool, where the bool states if the objects are active -
		// they get turned off when they have collided with the sun, in which case they are to be ignored
		for (const auto& object: objectList)
		{
			// all objects get pull from other objects - except themselves and disabled objects
			for (const auto& otherObject: objectList)
			{
				// the sun and other bigger objects wont be affected by the earth and the earth wont be by the moon or by other same weight planets - for simplicity, and optimisation
				if (!object.second || !otherObject.second || otherObject.first->mass <= object.first->mass)
					continue;
				double distance = object.first->DistanceFrom(*otherObject.first);
				// hack: the earth should have a pull much stronger on the moon, because its supposed to be much closer (but we wouldn't see anything if it was real scale)
				if (object.first->mass * otherObject.first->mass == EARTH_MOON_MASS)
					distance /= 100;

				const double pull = otherObject.first->mass * GRAVITATIONAL / (distance * distance) * time;
				object.first->vX += pull * ((otherObject.first->x > object.first->x) ? 1 : -1);
				object.first->vY += pull * ((otherObject.first->y > object.first->y) ? 1 : -1);
				object.first->vZ += pull * ((otherObject.first->z > object.first->z) ? 1 : -1);
			}
		}

		// check for collisions and handle them
		for (const auto& object: objectList)
		{
			if (!object.second)
				continue;
			for (const auto& otherObject: objectList)
			{
				if (!object.second || otherObject == object)
					continue;
				else if (object.first->DistanceFrom(*otherObject.first) < (
							 object.first->radius + otherObject.first->radius))
				{
					// a collision with the sun is fatal
					if (object.first->mass == SUN_MASS)
						objectList[otherObject.first] = false;
					else if (otherObject.first->mass == SUN_MASS)
						objectList[object.first] = false;
					// otherwise both objects move away
					else
					{
						// determine the strength of the collision
						const double factor = otherObject.first->mass * object.first->mass * std::sqrt(
											(object.first->vX - otherObject.first->vX) * (
												object.first->vX - otherObject.first->vX) +
											(object.first->vY - otherObject.first->vY) * (
												object.first->vY - otherObject.first->vY) +
											(object.first->vZ - otherObject.first->vZ) * (
												object.first->vZ - otherObject.first->vZ)
										);
						// make them move away from each other with the strength of the impact
						otherObject.first->vX += factor / otherObject.first->mass * (otherObject.first->vX > 0 ? -1 : 1);
						otherObject.first->vY += factor / otherObject.first->mass * (otherObject.first->vY > 0 ? -1 : 1);
						otherObject.first->vZ += factor / otherObject.first->mass * (otherObject.first->vZ > 0 ? -1 : 1);
						object.first->vX += factor / object.first->mass * (object.first->vX > 0 ? -1 : 1);
						object.first->vY += factor / object.first->mass * (object.first->vY > 0 ? -1 : 1);
						object.first->vZ += factor / object.first->mass * (object.first->vZ > 0 ? -1 : 1);
						// move them away enough so they're no longer in collision
						object.first->Tick(time);
						otherObject.first->Tick(time);
					}
				}
			}
		}

		// move all objects
		for (const auto& object: objectList)
			object.first->Tick(time);
	}
};
