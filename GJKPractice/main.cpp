#include <iostream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "chMath.h"
#include "chGL.h"

// TODO LIST
// 3. Point Rendering for closest points
// 4. Make sample polygons for testing the GJK algorithm

GLFWwindow* gWindow;

bool TempTest();

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS || GLFW_REPEAT)
	{
		switch (key)
		{
		case GLFW_KEY_W:
			std::cout << "W pressed\n";
			goto Final;
		case GLFW_KEY_A:
			std::cout << "A pressed\n";
			goto Final;
		case GLFW_KEY_S:
			std::cout << "S pressed\n";
			goto Final;
		case GLFW_KEY_D:
			std::cout << "D pressed\n";
			goto Final;
		}
	}
	else if (action == GLFW_RELEASE)
	{
		
	}
Final:
	return;
}

int main()
{
	assert(TempTest());

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	gWindow = glfwCreateWindow(800, 600, "GJK Prac", NULL, NULL);
	glfwMakeContextCurrent(gWindow);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	//glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glfwSetFramebufferSizeCallback(gWindow, framebuffer_size_callback);
	// glfwSetCursorPosCallback(gWindow, cursorPos_callback);
	// glfwSetMouseButtonCallback(gWindow, mouseButton_callback);
	// glfwSetScrollCallback(gWindow, scroll_callback);
	glfwSetKeyCallback(gWindow, keyCallback);

	glfwSwapInterval(0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	Chan::ChVector4 x(1, 0, 0, 0);
	Chan::ChVector4 y(0, 1, 0, 0);
	Chan::ChVector4 z(0, 0, 1, 0);
	Chan::ChVector4 w(0, 0, 0, 1);
	Chan::ChMat44 iden(x, y, z, w);

	Chan::ChVector3 p1(0, 0, 0);
	Chan::ChVector3 p2(0.5, 0, 0);

	CGRenderLine lineRenderer;
	CGRenderPoint pointRenderer;

	while (!glfwWindowShouldClose(gWindow))
	{
		glfwPollEvents();

		glClearColor(0.2, 0.2, 0.2, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lineRenderer.insertLine(p1, p2, Chan::ChVector3(1, 1, 1));
		pointRenderer.insertPoint(p1, Chan::ChVector3(1, 0, 0), 10);

		lineRenderer.renderLine(iden, iden);
		pointRenderer.renderPoint(iden, iden);

		glfwSwapBuffers(gWindow);
	}

	glfwTerminate();
	return 0;
}

bool TempTest()
{
	std::cout << "Hello World\n";

	Chan::ChVector4 p1(0, 0, 0, 0);
	Chan::ChVector4 p2(0.5, 0, 0, 0);
	
	Chan::ChVector4 container[2];
	container[0] = p1;
	container[1] = p2;

	float* entryPoint = &(container[0].x);
	float testContainer[8] = { -1, };
	for (int i = 0; i < 8; ++i)
	{
		testContainer[i] = *(entryPoint + i);
		std::cout << *(entryPoint + i) << '\n';
	}
	
	size_t vec4 = sizeof(Chan::ChVector4);
	
	const Chan::ChVector3 t(1, 0, 0);
	const float ff = t[0];
	
	std::cout << "Test Done\n";

	return true;
}