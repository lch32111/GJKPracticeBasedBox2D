#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

#define DBG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DBG_NEW
#endif

#include <iostream>
#include <string>
#include <map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "chMath.hpp"
#include "chGL.hpp"
#include "Distance2D.hpp"

// TODO List : test environment implemented
// 1. Study GJK
// 2. Debug Algorithm
// 3. Apply ur own collision detection algorithm to ur engine.

bool CRTMemorySet();
bool UnitTest();
bool initFreeType();
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

GLFWwindow* gWindow;
Chan::ChTransform triTransform;
Chan::ChTransform quadTransform;
int main()
{
	assert(CRTMemorySet());
	assert(UnitTest());
	int* a = new int[5];
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	gWindow = glfwCreateWindow(800, 600, "GJK Prac", NULL, NULL);
	glfwMakeContextCurrent(gWindow);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glfwSetKeyCallback(gWindow, keyCallback);
	glfwSwapInterval(0);

	assert(initFreeType());

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

struct Character
{
	GLuint TextureID;			// ID handle of the glyph texture
	Chan::ChVector2 Size;		// Size of glyph
	Chan::ChVector2 Bearing;	// Offset from baseline to left/top of glyph
	GLuint Advance;				// Offset to advance to next glyph
};
std::map<GLchar, Character> Characters;

bool initFreeType()
{
	bool fail = true;

	FT_Library ft;
	if ((fail = FT_Init_FreeType(&ft)))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		goto Final;
	}

	FT_Face face;
	if ((fail = FT_New_Face(ft, "fonts/arial.ttf", 0, &face)))
	{
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		goto Final;
	}

	FT_Set_Pixel_Sizes(face, 0, 48);

	if ((fail = FT_Load_Char(face, 'X', FT_LOAD_RENDER)))
	{
		std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
		goto Final;
	}
	

	// Disable byte-alignment restriction
	// 이후의 glReadPixels의 연산 뿐만 아니라, 텍스쳐 패턴의 unpacking에
	// 영향을 미치는 pixel storage mode를 설정한다.
	// 10개 중 4개의 storage parameters는 pixel data가 client memory에
	// 어떻게 반환되는지 영향을 미친다.
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (GLubyte c = 0; c < 128; ++c)
	{
		// Load character glyph
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			goto Final;
		}

		// Generate Texeture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Character character =
		{
			texture,
			Chan::ChVector2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			Chan::ChVector2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
	}
Final:
	if (fail) return false;
	else return true;

	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}