#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "chMath.h"

#include <iostream>
#include <string>

// TODO LIST
// 2. Key Input processing (rotate, translate on polygon)
// 3. Point Rendering for closest points
// 4. Make sample polygons for testing the GJK algorithm

GLFWwindow* gWindow;

bool checkCompileErrors(GLuint shader, std::string type)
{
	GLint success;
	GLchar infoLog[1024];

	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " 
				<< type << " \n" 
				<< infoLog << std::endl;

			return false;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: "
				<< type << "\n" 
				<< infoLog 	<< std::endl;
			return false;
		}
	}

	return true;
}

class CGRenderLine
{
public:
	CGRenderLine()
	{
		const char* vs =
			"#version 330 core\n"
			"layout(location = 0) in vec3 aPos;\n"
			"layout(location = 1) in vec3 aColor;\n"
			"uniform mat4 projection;\n"
			"uniform mat4 view;\n"
			"out vec4 lineColor;\n"
			"void main()\n"
			"{\n"
				"gl_Position = projection * view * vec4(aPos, 1.0);\n"
				"lineColor = vec4(aColor, 1.0);\n"
			"}";

		const char* fs =
			"#version 330 core\n"
			"out vec4 FragColor;\n"
			"in vec4 lineColor;\n"
			"void main()\n"
			"{\n"
				"FragColor = lineColor;\n"
			"}";
		
		unsigned int vertex, fragment;

		// vertex Shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vs, NULL);
		glCompileShader(vertex);
		if (checkCompileErrors(vertex, "VERTEX") == false)
			assert(0);

		// fragment shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fs, NULL);
		glCompileShader(fragment);
		if (checkCompileErrors(fragment, "FRAGMENT") == false)
			assert(0);

		// shader Program
		m_program = glCreateProgram();
		glAttachShader(m_program, vertex);
		glAttachShader(m_program, fragment);
		glLinkProgram(m_program);
		if (checkCompileErrors(m_program, "PROGRAM") == false)
			assert(0);

		// delete the shaders as they're linked into our program now and no longer necessary
		glDeleteShader(vertex);
		glDeleteShader(fragment);

		glUseProgram(m_program);
		m_ProjLoc = glGetUniformLocation(m_program, "projection");
		m_ViewLoc = glGetUniformLocation(m_program, "view");

		glUseProgram(0);

		prepareData();
	}

	~CGRenderLine()
	{
		glDeleteProgram(m_program), m_program = 0;
		glDeleteBuffers(2, m_VBO);
		glDeleteVertexArrays(1, &m_VAO);
		
		m_count = 0;
	}

	void insertLine(const Chan::ChVector3& From, const Chan::ChVector3& To, const Chan::ChVector3& Color)
	{
		assert(m_count < e_maxVertices);
		m_vertices[m_count] = From;
		m_colors[m_count] = Color;
		++m_count;

		assert(m_count < e_maxVertices);
		m_vertices[m_count] = To;
		m_colors[m_count] = Color;
		++m_count;
	}

	void renderLine(const Chan::ChMat44& proj, const Chan::ChMat44& view)
	{
		if (m_count == 0) return;

		// Vertex Buffer
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(Chan::ChVector3), m_vertices[0].data());

		// Color Buffer
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO[1]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(Chan::ChVector3), m_colors[0].data());

		glUseProgram(m_program);
		{
			glUniformMatrix4fv(m_ProjLoc, 1, GL_FALSE, proj.data());
			glUniformMatrix4fv(m_ViewLoc, 1, GL_FALSE, view.data());

			glBindVertexArray(m_VAO);
			glDrawArrays(GL_LINES, 0, m_count);
		}
		
		// Setting Default again
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		m_count = 0;
	}
private:

	enum { e_maxVertices = 2 * 512 };
	Chan::ChVector3 m_vertices[e_maxVertices];
	Chan::ChVector3 m_colors[e_maxVertices];

	unsigned m_count;

	unsigned m_VAO;
	unsigned m_VBO[2];

	GLuint m_program;
	GLuint m_ProjLoc;
	GLuint m_ViewLoc;

	void prepareData()
	{
		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(2, m_VBO);

		glBindVertexArray(m_VAO);
		glEnableVertexAttribArray(0); // vertex
		glEnableVertexAttribArray(1); // color

		// Vertex Buffer
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		

		// Color Buffer
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_colors), m_colors, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		m_count = 0;
	}
};

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

	while (!glfwWindowShouldClose(gWindow))
	{
		glfwPollEvents();

		glClearColor(0.2, 0.2, 0.2, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lineRenderer.insertLine(p1, p2, Chan::ChVector3(1, 1, 1));
		lineRenderer.renderLine(iden, iden);

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
	
	size_t vec3 = sizeof(Chan::ChVector4);

	
	
	std::cout << "Test Done\n";

	return true;
}