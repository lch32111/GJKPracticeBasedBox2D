#include <iostream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "chMath.h"
#include "chGL.h"
#include "Distance2D.h"

// TODO List : test environment implemented
// 1. Study GJK
// 2. Debug Algorithm
// 3. Apply ur own collision detection algorithm to ur engine.

bool UnitTest();
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

GLFWwindow* gWindow;
Chan::ChTransform triTransform;
Chan::ChTransform quadTransform;
int main()
{
	assert(UnitTest());

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	gWindow = glfwCreateWindow(800, 600, "GJK Prac", NULL, NULL);
	glfwMakeContextCurrent(gWindow);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glfwSetKeyCallback(gWindow, keyCallback);
	glfwSwapInterval(0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	CGRenderLine lR;
	CGRenderPoint pR;

	Chan::ChVector2 tris[3] =
	{
		Chan::ChVector2(1.f, 0.0f),
		Chan::ChVector2(0.0f, 1.f),
		Chan::ChVector2(-1.f, 0.0f),
	};

	Chan::Polygon triangle;
	triangle.m_points = tris;
	triangle.m_count = 3;
	triTransform.p = Chan::ChVector2(0, 0);
	triTransform.R = Chan::ChMat22(Chan::Radians(0.0f));

	const Chan::ChVector2 quads[4] = 
	{
		Chan::ChVector2(-1, -1),
		Chan::ChVector2(1, -1),
		Chan::ChVector2(1, 1),
		Chan::ChVector2(-1, 1)
	};

	Chan::Polygon quad;
	quad.m_points = quads;
	quad.m_count = 4;
	quadTransform.p = Chan::ChVector2(3, 3);
	quadTransform.R = Chan::ChMat22(Chan::Radians(0.0f));

	while (!glfwWindowShouldClose(gWindow))
	{
		glfwPollEvents();

		glClearColor(0.2, 0.2, 0.2, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Chan::Input firstTest;
		firstTest.polygon1 = triangle;
		firstTest.polygon2 = quad;
		firstTest.transform1 = triTransform;
		firstTest.transform2 = quadTransform;

		Chan::Output testResult;

		Chan::Distance2D(&testResult, firstTest);

		std::cout << "distance : " << testResult.distance 
			<< " / iteration : " << testResult.iterations << '\n';

		pR.insertPoint(Chan::ChVector3(testResult.point1, 0), Chan::ChVector3(1, 0, 0), 10.f);
		pR.insertPoint(Chan::ChVector3(testResult.point2, 0), Chan::ChVector3(0, 1, 0), 10.f);
		lR.insertLine(Chan::ChVector3(testResult.point1, 0), Chan::ChVector3(testResult.point2, 0), Chan::ChVector3(0.81, 0.4, 0.5));
		insertPolygon(lR, triangle, triTransform, Chan::ChVector3(0.7, 0.2, 0.4));
		insertPolygon(lR, quad, quadTransform, Chan::ChVector3(0.1, 0.4, 0.8));

		Chan::ChMat44 proj = Chan::Ortho(-5.f, 5.f, -5.f, 5.f);
		lR.renderLine(proj, Chan::ChMat44(1.f), 2.f);
		pR.renderPoint(proj, Chan::ChMat44(1.f));

		glfwSwapBuffers(gWindow);
	}

	glfwTerminate();
	return 0;
}

static bool Key1Pressed = false;
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	static bool what = false;
	Chan::ChTransform* object = what ? &triTransform : &quadTransform;

	if (action == GLFW_PRESS || GLFW_REPEAT)
	{
		switch (key)
		{
		case GLFW_KEY_1:
		{
			if (!Key1Pressed)
			{
				what = !what;
				Key1Pressed = true;
			}
			goto Final;
		}
		case GLFW_KEY_W:
			object->p += Chan::ChVector2(0, 0.05);
			goto Final;
		case GLFW_KEY_A:
			object->p += Chan::ChVector2(-0.05, 0.0);
			goto Final;
		case GLFW_KEY_S:
			object->p += Chan::ChVector2(0.0 , -0.05);
			goto Final;
		case GLFW_KEY_D:
			object->p += Chan::ChVector2(0.05, 0.0);
			goto Final;

		case GLFW_KEY_R:
			goto Final;
		}
	}
	else if (action == GLFW_RELEASE)
	{
		switch (key)
		{
		case GLFW_KEY_1:
			if (Key1Pressed) Key1Pressed = false;
			goto Final;
		}
	}
Final:
	return;
}

bool UnitTest()
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

	return vec4 == 16;
}