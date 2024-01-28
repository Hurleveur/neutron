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
#include <functional>

namespace nge::physics
{
	using Kilogram = float;
	using Meter = float;

	// TODO: too bad GLM does not support constexpr :(
	static const math::Vector3 EARTH_GRAVITY_FORCE(0.0f, -9.81f, 0.0f);
	static const math::Vector3 MOON_GRAVITY_FORCE(0.0f, -1.625f, 0.0f);

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

		bool is_static;

	public:
		constexpr Body(const math::Vector3& location, const Kilogram mass, const Meter sphere_radius, bool is_static = false):
			location(location), velocity(), force(EARTH_GRAVITY_FORCE), inv_mass(1.0f / mass), radius(sphere_radius), is_static(is_static)
		{}

		const math::Vector3& GetLocation() const {return location;}
//		float GetCollisionRadius() const {return collision.radius;}

		bool IsStatic() const {return is_static;}

		virtual void Integrate(const timing::Seconds time_step)
		{
			if (is_static)
				return;

			const math::Vector3 body_acceleration = force * inv_mass;

			// perform Euler integration (more error-prone, but fastest)
			velocity += body_acceleration * time_step;
			location += velocity * time_step;

//			force = math::Vector3(0.0f);
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

		static constexpr float COEFFICIENT_OF_RESTITUTION = 1.0f; // 1=perfectly elastic, no loss of energy
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
			if (!other.IsStatic())
				other.velocity += other.inv_mass * impulse;
		}
	};

	class Simulation
	{
		std::vector<Body> spherical_bodies;
		std::vector<std::pair<Body*, Body*>> potential_collisions;

		using ImpactCallback = std::function<void(Body*, Body*)>;
		ImpactCallback impact_callback;

		// perform the actual dynamics
		void UpdateDynamics(const timing::Seconds time_step)
		{
			// update every body state
			for (auto& body : spherical_bodies)
			{
				body.Integrate(time_step);
			}
		}

		void CachePotentialCollisions()
		{
			// TODO: to reduce the number of pairs to check, we could implement spatial partitioning
			// using a grid or an octree
			for (auto& body_a : spherical_bodies)
			{
				for (auto& body_b : spherical_bodies)
				{
					// skip same body
					if (&body_a == &body_b)
						continue;

					if (body_a.IsCollidingWith(body_b))
						potential_collisions.emplace_back(&body_a, &body_b);
				}
			}
		}

		void PerformCollisionResponse(const timing::Seconds time_step)
		{
			for (auto& pair : potential_collisions)
			{
				auto& body_a = pair.first;
				auto& body_b = pair.second;

				// react to collision
				body_a->PerformCollisionResponse(*body_b);

				// trigger a callback if there is one
				if (impact_callback)
					impact_callback(body_a, body_b);
			}
			potential_collisions.clear();
		}

	public:
		void Reset()
		{
			spherical_bodies.clear();
			potential_collisions.clear();
		}

		void SetImpactCallback(const ImpactCallback& callback) {impact_callback = callback;}

		ID AddSphericalBody(const math::Vector3& location, const Kilogram mass, const Meter radius, bool is_static = false)
		{
			const ID id = spherical_bodies.size();
			spherical_bodies.emplace_back(location, mass, radius, is_static);
			return id;
		}

		void Tick(const timing::Seconds time_step = 1.0f/30.0f)
		{
			UpdateDynamics(time_step);
			CachePotentialCollisions();
			PerformCollisionResponse(time_step);
		}

		const math::Vector3& GetBodyLocation(const ID id) const {return spherical_bodies[id].GetLocation();}
	};
}
