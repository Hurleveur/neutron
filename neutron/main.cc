// engine headers
#include "nge_window.hh"

// game headers
#include "game.hh"

static void GuardedMain()
{
	const nge::Window window("Neutron", nge::Extent2D(1600, 1000));
	NeutronGame game;
	window.SetEventListener(&game);

#if 0
	std::cout <<
			"Welcome to the solar system simulator, featuring particles within the Sun, a physics simulator including gravity and collisions with devastating effects (that is doing the simili orbits), realistic planets with Phong shaders and even normal mapping (each planet featuring their own image, normal and specular maps)."
			<< std::endl;
	std::cout <<
			"All of this would not be complete, however, without the skybox to surround all of it, and the ability to stop time."
			<< std::endl;
	std::cout <<
			"In order to move use Z (up), S(down), Q(left) and D(right) - camera speed can be changed in the code at line 59 of game.cpp - in order to stop/resume time, press T."
			<< std::endl;
#endif

	//glfwSwapInterval(0); // to remove the 60 fps limit
	float time = 0.0f;
	float last_time = 0.0f;
	while (!window.ShouldClose())
	{
		time = static_cast<float>(glfwGetTime());
		const float delta_time = time - last_time;
		last_time = time;

		if (!game.Tick(delta_time))
			break; // we want to quit

		window.Present();
		window.ProcessEvents();
	}
}

int main(int argc, char** argv)
{
	try
	{
		GuardedMain();
	}
	catch (const std::exception& x)
	{
		std::cerr << "Critical engine error: " << x.what() << '\n';
		// std::cout << std::stacktrace::current() << '\n';
		return 1;
	}

	return 0;
}
