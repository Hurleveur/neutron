//==============================================================================
//	Neutron Solar System
//	(C) 2024 Hurleveur
//==============================================================================

#pragma once

// neutron headers
#include "import.hh"

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

	bool w_key_pressed;
	bool a_key_pressed;
	bool s_key_pressed;
	bool d_key_pressed;

	nge::timing::Seconds lastFrame;

	bool stop;
	float stopTimeout;

	std::vector<Planet> planets;

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
		if (key_code == GLFW_KEY_W)
			w_key_pressed = action;
		if (key_code == GLFW_KEY_A)
			a_key_pressed = action;
		if (key_code == GLFW_KEY_S)
			s_key_pressed = action;
		if (key_code == GLFW_KEY_D)
			d_key_pressed = action;

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
		std::ifstream mdl_file(file_path.data(), std::ios::binary);
		if (!mdl_file.is_open())
			throw std::runtime_error("Failed to open an MDL file for reading.");

		// read vertices
		u32 vertex_count;
		mdl_file.read((char *) &vertex_count, sizeof(u32));
		assert(vertex_count < 10000); // sanity check
		std::vector<nge::graphics::VertexFormat> vertices;
		vertices.resize(vertex_count);
		mdl_file.read((char *) vertices.data(), vertex_count * sizeof(nge::graphics::VertexFormat));

		// read corner indices
		u32 vertex_index_count;
		mdl_file.read((char *) &vertex_index_count, sizeof(u32));
		assert(vertex_index_count < 10000); // sanity check
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
		w_key_pressed(false),
		a_key_pressed(false),
		s_key_pressed(false),
		d_key_pressed(false),
		lastFrame(0.0f),
		stop(false),
		stopTimeout(0.0f),
		planets(),
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

		planets.emplace_back(sun);
		planets.emplace_back(mercury);
		planets.emplace_back(earth);
		planets.emplace_back(moon);
		planets.emplace_back(mars);
	}

	bool Tick(const float delta_time)
	{
		// process pending keyboard input
		if (w_key_pressed)
			camera.ProcessKeyboard(FORWARD, delta_time);
		if (a_key_pressed)
			camera.ProcessKeyboard(LEFT, delta_time);
		if (s_key_pressed)
			camera.ProcessKeyboard(BACKWARD, delta_time);
		if (d_key_pressed)
			camera.ProcessKeyboard(RIGHT, delta_time);

		// do we need to stop immediately?
		if (needs_to_stop)
			return false;

		// to prevent double stopping time in two consecutive frames (which results in not doing anything)
		if (stopTimeout > 0.f)
			stopTimeout -= delta_time;

		// steps gravity and movement with physics, and do collision detection + handling
		Step(delta_time);

		// we don't need to clear GL_COLOR_BUFFER_BIT due to the skybox being in the background
		glClear(GL_DEPTH_BUFFER_BIT);

		// shader properties for all planets and the sun
		planet_shader.use();
		planet_shader.setVec3("light.position", 0.f, 0.f, 0.f);
		planet_shader.setVec3("viewPos", camera.Position);
		// full on sunlight - will be reduced after the sun is drawn
		planet_shader.setVec3("light.ambient", .8f, .8f, .8f);
		planet_shader.setVec3("light.diffuse", .7f, .7f, .7f);
		planet_shader.setVec3("light.specular", .5f, .5f, .5f);

		// view and projection matrix, with a depth sufficient so it doesn't crop objects - both are also multiplied here in advanced for optimisation
		const nge::math::Matrix4& view = camera.GetViewMatrix();
		const nge::math::Matrix4 projection = glm::perspective(glm::radians(camera.Zoom), (float) 1600 / (float) 1000, 0.1f, 1024.0f);
		const nge::math::Matrix4 viewProj = projection * view;

		glDepthFunc(GL_LEQUAL);
		glEnable(GL_DEPTH_TEST);

		// render all objects (model is calculated in the draw function itself)
		planet_shader.setMat4("view_projection_matrix", viewProj);
		planet_model.Set(); // TODO: encapsulate inside a PlanetRenderer class
		for (auto& planet: planets)
		{
			planet.SetShaderVariables(planet_shader, stop ? 0.0f : delta_time);
			planet_model.Draw();

			// only the sun needs to be super bright, and its drawn first
			planet_shader.setVec3("light.ambient", .1f, .1f, .1f);
		}
		planet_model.Unset(); // TODO: encapsulate inside a PlanetRenderer class

		drawSkybox(skybox_shader, view, projection);

		// draw particles
		particle_shader.use();
		particle_shader.setMat4("view_projection_matrix", viewProj);
		glDisable(GL_DEPTH_TEST);
		drawParticles(particle_shader, stop ? 0.0f : delta_time);
		glEnable(GL_DEPTH_TEST);

		return true;
	}

	void Step(const nge::timing::Seconds time)
	{
		if (stop)
			return;

		// gravitational constant
		constexpr float GRAVITATIONAL = 6.674 / 100000000000;
		// apply gravity
		// auto is Planet, bool, where the bool states if the objects are active -
		// they get turned off when they have collided with the sun, in which case they are to be ignored
		for (auto& planet_a: planets)
		{
			// all objects get pull from other objects - except themselves and disabled objects
			for (const auto& planet_b : planets)
			{
				// the sun and other bigger objects wont be affected by the earth and the earth wont be by the moon or by other same weight planets - for simplicity, and optimisation
				if (planet_b.mass <= planet_a.mass)
					continue;
				double distance = planet_a.DistanceFrom(planet_b);
				// hack: the earth should have a pull much stronger on the moon, because its supposed to be much closer (but we wouldn't see anything if it was real scale)
				if (planet_a.mass * planet_b.mass == EARTH_MOON_MASS)
					distance /= 100;

				const float pull = planet_b.mass * GRAVITATIONAL / (distance * distance) * time;
				planet_a.vX += pull * ((planet_b.x > planet_a.x) ? 1 : -1);
				planet_a.vY += pull * ((planet_b.y > planet_a.y) ? 1 : -1);
				planet_a.vZ += pull * ((planet_b.z > planet_a.z) ? 1 : -1);
			}
		}

		// check for collisions and handle them
		for (auto& planet_a: planets)
		{
			for (auto& planet_b: planets)
			{
				if (&planet_a == &planet_b)
					continue;

				if (planet_a.DistanceFrom(planet_b) < (planet_a.radius + planet_b.radius))
				{
					// a collision with the sun is fatal
//					if (planet_a.mass == SUN_MASS)
//						objectList[planet_b] = false;
//					else if (planet_b.mass == SUN_MASS)
//						objectList[planet_a] = false;
					// otherwise both objects move away
//					else
					{
						// determine the strength of the collision
						const double factor = planet_b.mass * planet_a.mass * std::sqrt(
											(planet_a.vX - planet_b.vX) * (
												planet_a.vX - planet_b.vX) +
											(planet_a.vY - planet_b.vY) * (
												planet_a.vY - planet_b.vY) +
											(planet_a.vZ - planet_b.vZ) * (
												planet_a.vZ - planet_b.vZ)
										);
						// make them move away from each other with the strength of the impact
						planet_b.vX += factor / planet_b.mass * (planet_b.vX > 0 ? -1 : 1);
						planet_b.vY += factor / planet_b.mass * (planet_b.vY > 0 ? -1 : 1);
						planet_b.vZ += factor / planet_b.mass * (planet_b.vZ > 0 ? -1 : 1);
						planet_a.vX += factor / planet_a.mass * (planet_a.vX > 0 ? -1 : 1);
						planet_a.vY += factor / planet_a.mass * (planet_a.vY > 0 ? -1 : 1);
						planet_a.vZ += factor / planet_a.mass * (planet_a.vZ > 0 ? -1 : 1);
						// move them away enough so they're no longer in collision
						planet_a.Tick(time);
						planet_b.Tick(time);
					}
				}
			}
		}

		// update all planets
		for (auto& planet : planets)
			planet.Tick(time);
	}
};
