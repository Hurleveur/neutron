#include "nge_physics.hh"

// std headers
#include <iostream>
#include <format>

int main()
{
	nge::physics::Simulation sim;
	auto b1 = sim.AddSphericalBody(nge::math::Vector3(0.0f, 5.0f, 0.0f), 1.0f, 1.0f);
	auto b2 = sim.AddSphericalBody(nge::math::Vector3(0.0f, -10.0f, 0.0f), 100.0f, 10.0f, true);

	// react on impact
	sim.SetImpactCallback([](nge::physics::Body* body1, nge::physics::Body* body2)
	{
		std::cout << "Collision detected between body " << body1 << " and body " << body2 << '\n';
	});

	// simulate 240 iterations
	for (u32 frame = 0; frame < 120; frame++)
	{
		sim.Tick(1.0f/30.0f);

		// print results
		const auto& b1_loc = sim.GetBodyLocation(b1);
		std::cout << std::format("Sphere 1 at ({} {} {})\n", b1_loc.x, b1_loc.y, b1_loc.z);
		const auto& b2_loc = sim.GetBodyLocation(b2);
		std::cout << std::format("Sphere 2 at ({} {} {})\n\n", b2_loc.x, b2_loc.y, b2_loc.z);
	}
}
