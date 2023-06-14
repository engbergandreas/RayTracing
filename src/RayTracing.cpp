#pragma once

#include <string>

#include "Camera.h"
#include "Triangle.h"
#include "Scene.h"
#include <chrono>
#include "Utils.h"
#include <thread>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void raytracingCPU() {
	Scene scene{};
	std::string filename{ settings::FILENAME };

	glm::dvec3 cameraPos{ -1.1, 0.0, 0.0 };
	glm::dvec3 cameraFront{ 0.0, 1.0, 0.0 };
	glm::dvec3 cameraUp{ 0.0, 0.0, 1.0 };
	Camera camera{ cameraPos, cameraFront, cameraUp };

	auto start = std::chrono::high_resolution_clock::now();
	camera.render(scene);
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
	std::cout << "Finished rendering in: " << duration.count() << " seconds\n";

	camera.writeToFile(filename);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);

void processInput(GLFWwindow* window);

GLFWwindow* initGLFW() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(
		settings::CAMERA_PLANE_WIDTH, settings::CAMERA_PLANE_HEIGHT,
		"RayTracer", NULL, NULL);

	if (window == NULL)
		return NULL;

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	return window;
}

/// <summary>
/// Check whether OpenGL was setup correctly, returns false if there was a failure
/// </summary>
/// <param name="window">window context to validate</param>
bool validateOpenGL(GLFWwindow* window) {
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}
	return true;
}

int main()
{
	GLFWwindow* window{ initGLFW() };
	if (!validateOpenGL(window))
		return -1;

	//Setup initial size of window
	glViewport(0, 0, settings::CAMERA_PLANE_WIDTH, settings::CAMERA_PLANE_HEIGHT);


	while (!glfwWindowShouldClose(window))
	{
		//Input
		processInput(window);

		//Render
		glClearColor(0.4f, 0.1f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	//Close window on escape key
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	std::cout << "width: " << width << " height: " << height << '\n';
}

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
	std::cout << "cursor move to screen pos x: " << xpos << " y: " << ypos << '\n';
}