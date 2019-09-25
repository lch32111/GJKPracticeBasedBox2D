#include "chGL.hpp"

void insertPolygon(CGRenderLine& lR, const Chan::Polygon& p, const Chan::ChTransform& t, const Chan::ChVector3& color)
{
#define WorldPoint(i) Chan::ChVector3(t.R * p.m_points[i] + t.p, 0.f)
	for (int i = 0; i < p.m_count; ++i)
	{
		int next = i + 1;
		if (next == p.m_count) next = 0;

		lR.insertLine(WorldPoint(i), WorldPoint(next), color);
	}
#undef WorldPoint
}

CGRenderLine::CGRenderLine()
{
	const char* vs =
		"#version 330 core\n"
		"layout(location = 0) in vec3 aPos;\n"
		"layout(location = 1) in vec3 aColor;\n"
		"uniform mat4 projView;\n"
		"out vec4 lineColor;\n"
		"void main()\n"
		"{\n"
		"	gl_Position = projView * vec4(aPos, 1.0);\n"
		"	lineColor = vec4(aColor, 1.0);\n"
		"}";

	const char* fs =
		"#version 330 core\n"
		"out vec4 FragColor;\n"
		"in vec4 lineColor;\n"
		"void main()\n"
		"{\n"
		"	FragColor = lineColor;\n"
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
	m_ProjViewLoc = glGetUniformLocation(m_program, "projView");

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

	size_t dataSize = m_count * sizeof(Chan::ChVector3);
	
	// Vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);
	{
		void* mapP = glMapBufferRange(GL_ARRAY_BUFFER, 0, dataSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		memcpy(mapP, m_vertices[0].data(), dataSize);
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}
	
	// Color Buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[1]);
	{
		void* mapP = glMapBufferRange(GL_ARRAY_BUFFER, 0, dataSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		memcpy(mapP, m_colors[0].data(), dataSize);
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}

	glUseProgram(m_program);
	{
		Chan::ChMat44 projView = proj * view;
		glUniformMatrix4fv(m_ProjViewLoc, 1, GL_FALSE, projView.data());
		glBindVertexArray(m_VAO);

		glLineWidth(lineWidth);
		glDrawArrays(GL_LINES, 0, m_count);
	}

	// Setting Default again
	glLineWidth(1.0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
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

CGRenderPoint::CGRenderPoint()
{
	const char* vs = \
		"#version 330 core\n"
		"layout(location = 0) in vec3 aPos;\n"
		"layout(location = 1) in vec3 aColor;\n"
		"layout(location = 2) in float aSize;\n"
		"uniform mat4 projView;\n"
		"out vec4 pointColor;\n"
		"void main(void)\n"
		"{\n"
		"	gl_Position = projView * vec4(aPos, 1.0f);\n"
		"   gl_PointSize = aSize;\n"
		"	pointColor = vec4(aColor, 1.0);\n"
		"}\n";

	const char* fs = \
		"#version 330 core\n"
		"out vec4 FragColor;\n"
		"in vec4 pointColor;\n"
		"void main(void)\n"
		"{\n"
		"	FragColor = pointColor;\n"
		"}\n";

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
	m_ProjViewLoc = glGetUniformLocation(m_program, "projView");

	glUseProgram(0);

	prepareGLObject();
}

CGRenderPoint::~CGRenderPoint()
{
	glDeleteProgram(m_program), m_program = 0;
	glDeleteBuffers(3, m_VBO);
	glDeleteVertexArrays(1, &m_VAO);

	m_count = 0;
}

void CGRenderPoint::insertPoint(const Chan::ChVector3& P, const Chan::ChVector3& Color, float Size)
{
	assert(m_count < e_maxVertices);
	m_vertices[m_count] = P;
	m_colors[m_count] = Color;
	m_sizes[m_count] = Size;
	++m_count;
}

void CGRenderPoint::renderPoint(const Chan::ChMat44& proj, const Chan::ChMat44& view)
{
	if (m_count == 0) return;

	size_t vcSize = m_count * sizeof(Chan::ChVector3);

	// Vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);
	{
		void* mapP = glMapBufferRange(GL_ARRAY_BUFFER, 0, vcSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		memcpy(mapP, m_vertices[0].data(), vcSize);
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}
	
	// Color Buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[1]);
	{
		void* mapP = glMapBufferRange(GL_ARRAY_BUFFER, 0, vcSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		memcpy(mapP, m_colors[0].data(), vcSize);
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}

	size_t sSize = m_count * sizeof(float);

	// Size Buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[2]);
	{
		void* mapP = glMapBufferRange(GL_ARRAY_BUFFER, 0, vcSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		memcpy(mapP, &m_sizes[0], sSize);
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}
	

	glUseProgram(m_program);
	{
		Chan::ChMat44 projView = proj * view;
		glUniformMatrix4fv(m_ProjViewLoc, 1, GL_FALSE, projView.data());
		glBindVertexArray(m_VAO);

		glEnable(GL_PROGRAM_POINT_SIZE);
		glDrawArrays(GL_POINTS, 0, m_count);	
	}

	// Setting Default again
	glDisable(GL_PROGRAM_POINT_SIZE);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	m_count = 0;
}

void CGRenderPoint::prepareGLObject()
{
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(3, m_VBO);

	glBindVertexArray(m_VAO);
	glEnableVertexAttribArray(0); // vertex
	glEnableVertexAttribArray(1); // color
	glEnableVertexAttribArray(2); // size

	// Vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// Color Buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_colors), m_colors, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// Size Buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_sizes), m_sizes, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	m_count = 0;
}

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