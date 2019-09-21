#include "chGL.h"

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
				<< infoLog << std::endl;
			return false;
		}
	}

	return true;
}

CGRenderLine::CGRenderLine()
{
	const char* vs =
		"#version 330 core\n"
		"layout(location = 0) in vec3 aPos;\n"
		"layout(location = 1) in vec3 aColor;\n"
		"uniform mat4 projview;\n"
		"out vec4 lineColor;\n"
		"void main()\n"
		"{\n"
		"gl_Position = projview * vec4(aPos, 1.0);\n"
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
	m_ProjViewLoc = glGetUniformLocation(m_program, "projview");

	glUseProgram(0);

	prepareGLObject();
}

CGRenderLine::~CGRenderLine()
{
	glDeleteProgram(m_program), m_program = 0;
	glDeleteBuffers(2, m_VBO);
	glDeleteVertexArrays(1, &m_VAO);

	m_count = 0;
}

void CGRenderLine::insertLine(const Chan::ChVector3& From, const Chan::ChVector3& To, const Chan::ChVector3& Color)
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

void CGRenderLine::renderLine(const Chan::ChMat44& proj, const Chan::ChMat44& view, float lineWidth)
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
		Chan::ChMat44 projView = proj * view;
		glUniformMatrix4fv(m_ProjViewLoc, 1, GL_FALSE, projView.data());
		glBindVertexArray(m_VAO);

		glLineWidth(lineWidth);
		glDrawArrays(GL_LINES, 0, m_count);
	}

	// Setting Default again
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glLineWidth(1.0);

	m_count = 0;
}

void CGRenderLine::prepareGLObject()
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