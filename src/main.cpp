// MIT License

// Copyright(c) 2022 Chanhaeng Lee

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this softwareand associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :

// The above copyright noticeand this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

#define DBG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DBG_NEW
#endif

#include <iostream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include "chMath.hpp"
#include "chGL.hpp"
#include "Distance2D.hpp"

// TODO List : test environment implemented
// 1. Study GJK
// 2. Debug Algorithm
// 3. Apply ur own collision detection algorithm to ur engine.

bool CRTMemorySet();
bool UnitTest();
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void sizeCallback(GLFWwindow* window, int width, int height);
void scrollCallback(GLFWwindow* window, double dx, double dy);

GLFWwindow* gWindow;
int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;

Chan::ChReal C_ZOOM = 1.0;
Chan::ChVector2 C_CENTER = Chan::ChVector2(0, 0);
Chan::ChReal C_DIMENSION = 5.0;

Chan::ChTransform triTransform;
Chan::ChTransform quadTransform;
int main()
{
	assert(CRTMemorySet());
	assert(UnitTest());
	
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	gWindow = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GJK Prac", NULL, NULL);
	glfwMakeContextCurrent(gWindow);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glfwSetKeyCallback(gWindow, keyCallback);
	glfwSetFramebufferSizeCallback(gWindow, sizeCallback);
	glfwSetScrollCallback(gWindow, scrollCallback);
	glfwSwapInterval(0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

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

	CGRenderLine lR;
	CGRenderPoint pR;
	CGRenderText tR(SCR_WIDTH, SCR_HEIGHT);

	int fpsCounter = 0;
	double currentTime = 0;
	double lastTime = 0;
	double fpsTime = 0;
	int FPS = 0;

	Chan::SimplexCache SimCache;
	SimCache.count = 0;
	while (!glfwWindowShouldClose(gWindow))
	{
		// Calculate FPS
		{
			++fpsCounter;
			lastTime = currentTime;
			currentTime = glfwGetTime();
			double deltaTime = currentTime - lastTime;
			fpsTime += deltaTime;
			if (fpsTime >= 1.0)
			{
				FPS = fpsCounter;
				fpsCounter = 0;
				fpsTime = 0;
			}
		}

		glfwPollEvents();

		glClearColor(0.2, 0.2, 0.2, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// GJK Algorithm
		Chan::Input InputTest;
		Chan::Output OutputTest;

		double noCacheCurTime;
		double noCachePostTime;
		{
			InputTest.polygon1 = triangle;
			InputTest.polygon2 = quad;
			InputTest.transform1 = triTransform;
			InputTest.transform2 = quadTransform;

			noCacheCurTime = glfwGetTime();
			Chan::Distance2D(&OutputTest, InputTest);
			noCachePostTime = glfwGetTime();
		}

		// GJK Cache Algorithm
		Chan::Output CachedOutput;
		double CacheCurTime, CachePostTime;
		{
			CacheCurTime = glfwGetTime();
			Chan::Distance2D(&CachedOutput, &SimCache, InputTest);
			CachePostTime = glfwGetTime();
		}
		
		// Insert Output Primitives
		{
			pR.insertPoint(Chan::ChVector3(OutputTest.point1, 0), Chan::ChVector3(1, 0, 0), 10.f);
			pR.insertPoint(Chan::ChVector3(OutputTest.point2, 0), Chan::ChVector3(0, 1, 0), 10.f);
			pR.insertPoint(Chan::ChVector3(CachedOutput.point1, 0), Chan::ChVector3(0, 0, 1), 15.f);
			pR.insertPoint(Chan::ChVector3(CachedOutput.point2, 0), Chan::ChVector3(0, 0, 1), 15.f);
			lR.insertLine(Chan::ChVector3(OutputTest.point1, 0), Chan::ChVector3(OutputTest.point2, 0), Chan::ChVector3(0.81, 0.4, 0.5));
			lR.insertLine(Chan::ChVector3(CachedOutput.point1, 0), Chan::ChVector3(CachedOutput.point2, 0), Chan::ChVector3(0.21, 0.7, 0.5));
			insertPolygon(lR, triangle, triTransform, Chan::ChVector3(0.7, 0.2, 0.4));
			insertPolygon(lR, quad, quadTransform, Chan::ChVector3(0.1, 0.4, 0.8));
		}
		
		// Render
		{
			Chan::ChReal ratio = (Chan::ChReal)SCR_WIDTH / (Chan::ChReal)SCR_HEIGHT;
			Chan::ChVector2 extents(ratio * C_DIMENSION * C_ZOOM, C_DIMENSION * C_ZOOM);
			Chan::ChVector2 lower = C_CENTER - extents;
			Chan::ChVector2 upper = C_CENTER + extents;
			Chan::ChMat44 proj = Chan::Ortho(lower.x, upper.x, lower.y, upper.y);

			lR.renderLine(proj, Chan::ChMat44(1.f), 2.f);
			pR.renderPoint(proj, Chan::ChMat44(1.f));

			tR.renderText("FPS : " + std::to_string(FPS), 0, 0, 0.5, Chan::ChVector3(0.4, 0.6, 0.78));
			tR.renderText("Cached Dist : " + std::to_string(CachedOutput.distance), 0, SCR_HEIGHT - 20, 0.5, Chan::ChVector3(1, 0, 0));
			tR.renderText("UnCached Dist : " + std::to_string(OutputTest.distance), 0, SCR_HEIGHT - 50, 0.5, Chan::ChVector3(1, 0, 0));
			tR.renderText("Cache Time : " + std::to_string(CachePostTime - CacheCurTime), 0, 40, 0.4, Chan::ChVector3(0.4, 0.6, 0.78));
			tR.renderText("UnCache Time : " + std::to_string(noCachePostTime - noCacheCurTime), 0, 20, 0.4, Chan::ChVector3(0.4, 0.6, 0.78));
		}

		glfwSwapBuffers(gWindow);
	}

	glfwTerminate();
	return 0;
}

bool CRTMemorySet()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);

	return true;
}

static bool Key1Pressed = false;
static Chan::ChReal moveVel = 0.2;
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	static bool what = false;
	Chan::ChTransform* object = what ? &triTransform : &quadTransform;

	if (action == GLFW_PRESS || action == GLFW_REPEAT)
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
			object->p += Chan::ChVector2(0, moveVel);
			goto Final;
		case GLFW_KEY_A:
			object->p += Chan::ChVector2(-moveVel, 0.0);
			goto Final;
		case GLFW_KEY_S:
			object->p += Chan::ChVector2(0.0 , -moveVel);
			goto Final;
		case GLFW_KEY_D:
			object->p += Chan::ChVector2(moveVel, 0.0);
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

void sizeCallback(GLFWwindow* window, int width, int height)
{
	SCR_WIDTH = width, SCR_HEIGHT = height;
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
}

void scrollCallback(GLFWwindow* window, double dx, double dy)
{
	if (dy > 0) C_ZOOM /= 1.1f;
	else C_ZOOM *= 1.1f;
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