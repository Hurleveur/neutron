//==============================================================================
//	NGE - Neutron Game Engine
//	(C) 2024 Moczulski Alan
//==============================================================================

#pragma once

// engine headers
#include "build.hh"
#include "types.hh"

// GL headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// std headers
#include <iostream>
#include <cassert>

namespace nge
{
	struct Extent2D
	{
		u32 width, height;
		constexpr Extent2D():
			width(0), height(0)
		{}
		constexpr Extent2D(const u32 width, const u32 height):
			width(width), height(height)
		{}
	};

	// every game needs to implement this to be able to respond to events
	struct WindowEventHandler
	{
		WindowEventHandler() = default;
		virtual ~WindowEventHandler() = default;

		virtual void ProcessKeyPress(u32 key_code, u32 action) = 0;
		virtual void ProcessMouseMotion(float x, float y) = 0;
		virtual void ProcessViewportResize(const Extent2D& dimensions) = 0;
	};

	class Window
	{
		GLFWwindow* window;

		// enables raw, unaccelerated and unconstrained mouse movements
		static void EnableRawInput(GLFWwindow* window)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//			if (glfwRawMouseMotionSupported())
//				glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		}

		static void GLFWKeyCallback(GLFWwindow* window, const int key, const int scancode, const int action, const int mods)
		{
			const auto event_handler = static_cast<WindowEventHandler *>(glfwGetWindowUserPointer(window));
			assert(event_handler);
			event_handler->ProcessKeyPress(key, action);
		}

		static void GLFWCursorPosCallback(GLFWwindow* window, const double x, const double y)
		{
			const auto event_handler = static_cast<WindowEventHandler *>(glfwGetWindowUserPointer(window));
			assert(event_handler);
			static double previous_x = x;
			static double previous_y = y;
			const double x_delta = x - previous_x;
			const double y_delta = y - previous_y;
			previous_x = x;
			previous_y = y;
			event_handler->ProcessMouseMotion(static_cast<float>(x_delta), static_cast<float>(y_delta));
		}

		static void GLFWFramebufferSizeCallback(GLFWwindow* window, const int width, const int height)
		{
			const auto event_handler = static_cast<WindowEventHandler *>(glfwGetWindowUserPointer(window));
			assert(event_handler);
			event_handler->ProcessViewportResize(Extent2D(width, height));
		}

		static void APIENTRY DebugCallbackFunction(
			GLenum source,
			GLenum type,
			GLuint id,
			GLenum severity,
			GLsizei length,
			const GLchar* message,
			const void* userParam)
		{
			std::cerr << "GL error: " << message << '\n';
		}

	public:
		explicit Window(const std::string_view title, const Extent2D& dimensions):
			window(nullptr)
		{
			if (!glfwInit())
				throw std::runtime_error("Failed to initialize GLFW.");

			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			window = glfwCreateWindow((s32)dimensions.width, (s32)dimensions.height, title.data(), nullptr, nullptr);
			if (!window)
				throw std::runtime_error("Failed to create GLFW window.");

			glfwMakeContextCurrent(window);

#if DEBUG_BUILD
//			glEnable(GL_DEBUG_OUTPUT);
//			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
//			glDebugMessageCallback(DebugCallbackFunction, nullptr);
#endif

			glfwSwapInterval(1); // enable vsync

			EnableRawInput(window);

			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
				throw std::runtime_error("Failed to initialize GLAD.");
		}

		~Window()
		{
			glfwDestroyWindow(window);
			glfwTerminate();
		}

		// sets the given event listener, so that it will be called whenever an event is produced
		void SetEventListener(const WindowEventHandler* event_listener) const
		{
			glfwSetWindowUserPointer(window, (void *)event_listener);
			glfwSetKeyCallback(window, GLFWKeyCallback);
			glfwSetCursorPosCallback(window, GLFWCursorPosCallback);
			glfwSetFramebufferSizeCallback(window, GLFWFramebufferSizeCallback);
		}

		Extent2D GetViewport() const
		{
			Extent2D viewport;
			glfwGetFramebufferSize(window, (s32*)&viewport.width, (s32*)&viewport.height);
			return viewport;
		}

		bool ShouldClose() const
		{
			return glfwWindowShouldClose(window);
		}

		void ProcessEvents() const
		{
			glfwPollEvents();
		}

		// flips the swapchain's backbuffer
		void Present() const
		{
			glfwSwapBuffers(window);
		}

		operator const GLFWwindow *() const { return window; }
	};
}
