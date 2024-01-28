//==============================================================================
//	NGE - Neutron Game Engine
//	(C) 2024 Moczulski Alan
//==============================================================================

#pragma once

// our headers
#include "nge_math.hh"
#include "nge_timing.hh"
#include "types.hh"

// std headers
#include <vector> // TEMPORARY, until we get nge::memory working

namespace nge::physics
{
	using Kilogram = float;
	using Meter = float;

#if 0
	class AABBCollisionComponent
	{
		friend class Simulation;

		const math::Vector3 min, max;

	public:
		constexpr AABBCollisionComponent(const math::Vector3& min, const math::Vector3& max):
			min(min), max(max)
		{}

		static bool AreColliding(const AABBCollisionComponent& a, const AABBCollisionComponent& b)
		{
			return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
					(a.min.y <= b.max.y && a.max.y >= b.min.y) &&
					(a.min.z <= b.max.z && a.max.z >= b.min.z);
		}
	};
#endif

	class Body
	{
		math::Vector3 location, velocity, force;
		// TODO: inertia?
		Kilogram inv_mass;

		float radius;

	public:
		constexpr Body(const math::Vector3& location, const Kilogram mass, const Meter sphere_radius):
			location(location), velocity(), force(), inv_mass(1.0f / mass), radius(sphere_radius)
		{}

		const math::Vector3& GetLocation() const {return location;}
//		float GetCollisionRadius() const {return collision.radius;}

		void Integrate(const timing::Seconds time_step)
		{
			const math::Vector3 body_acceleration = force * inv_mass;

			// perform Euler integration (more error-prone, but fastest)
			velocity += body_acceleration * time_step;
			location += velocity * time_step;

			force = math::Vector3(0.0f);
		}

		bool IsCollidingWith(const Body& other) const
		{
			const math::Vector3 line_connecting_centers = location - other.location;
			return radius + other.radius >= glm::length(line_connecting_centers);
		}

		math::Vector3 ComputeContactPoint(const Body& other) const
		{
			const math::Vector3 line_connecting_centers = other.location - location;
			glm::normalize(line_connecting_centers);
			return location + line_connecting_centers * radius;
		}

		math::Vector3 ComputeContactNormal(const Body& other) const
		{
			const math::Vector3 line_connecting_centers = other.location - location;
			return glm::normalize(line_connecting_centers);
		}

		constexpr float COEFFICIENT_OF_RESTITUTION = 1.0f; // 1=perfectly elastic, no loss of energy
		void PerformCollisionResponse(Body& other)
		{
			const math::Vector3 collision_normal = ComputeContactNormal(other);
			const math::Vector3 relative_velocity = other.velocity - velocity;

			// calculate the amount of impulse
			const float velocity_along_normal = dot(relative_velocity, collision_normal);
			if (velocity_along_normal > 0.0f)
				return; // they are moving away from each other

			const float impulse_scale = -(1.0f + COEFFICIENT_OF_RESTITUTION) * velocity_along_normal /
				(inv_mass + other.inv_mass);

			// apply impulse
			const math::Vector3 impulse = impulse_scale * collision_normal;
			velocity -= inv_mass * impulse;
			other.velocity += other.inv_mass * impulse;
		}
	};

	class Simulation
	{
		std::vector<Body> spherical_bodies;
		std::vector<std::pair<Body, Body>> potential_collisions;

		// perform a broad phase collision prepass
		void UpdatePotentialCollisions()
		{
			// TODO: to reduce the number of pairs to check, we could implement spatial partitioning
			// using a grid or an octree
			for (const auto& body_a : spherical_bodies)
			{
				for (const auto& body_b : spherical_bodies)
				{
					// skip same body
					if (&body_a == &body_b)
						continue;

					if (body_a.IsCollidingWith(body_b))
						potential_collisions.emplace_back(body_a, body_b);
				}
			}
		}

		// perform the actual dynamics
		void UpdateDynamics(const timing::Seconds time_step)
		{
			// update every body state
			for (auto& body : spherical_bodies)
			{
				body.Integrate(time_step);
			}
		}

		void PerformCollisionResponse(const timing::Seconds time_step)
		{
			for (auto& pair : potential_collisions)
			{
				auto& body_a = pair.first;
				auto& body_b = pair.second;
				body_a.PerformCollisionResponse(body_b);
			}
		}

	public:
		void Reset()
		{
			spherical_bodies.clear();
			potential_collisions.clear();
		}

		ID AddSphericalBody(const math::Vector3& location, const Kilogram mass, const Meter radius)
		{
			const ID id = spherical_bodies.size();
			spherical_bodies.emplace_back(location, mass, radius);
			return id;
		}

		void Tick(const timing::Seconds time_step = 1.0f/30.0f)
		{
			UpdatePotentialCollisions();
			UpdateDynamics(time_step);
			PerformCollisionResponse(time_step);
		}

		const math::Vector3& GetBodyLocation(const ID id) const {return spherical_bodies[id].GetLocation();}
	};
}
