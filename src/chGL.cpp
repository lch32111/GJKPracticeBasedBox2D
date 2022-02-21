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

#include "chGL.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

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

CGRenderText::CGRenderText(int& Screen_Width, int& Screen_Height)
	: m_scrWidth(Screen_Width), m_scrHeight(Screen_Height)
{
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		assert(0);
	}

	FT_Face face;
	if (FT_New_Face(ft, "arial.ttf", 0, &face))
	{
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		assert(0);
	}

	FT_Set_Pixel_Sizes(face, 0, 48);

	if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
	{
		std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
		assert(0);
	}
	
	// Disable byte-alignment restriction
	// 이후의 glReadPixels의 연산 뿐만 아니라, 텍스쳐 패턴의 unpacking에
	// 영향을 미치는 pixel storage mode를 설정한다.
	// 10개 중 4개의 storage parameters는 pixel data가 client memory에
	// 어떻게 반환되는지 영향을 미친다.
	// Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (GLubyte c = 0; c < 128; ++c)
	{
		// Load character glyph
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			assert(0);
		}

		// Generate Texeture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_R8,
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
		m_characters.insert(std::pair<GLchar, Character>(c, character));
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // reset

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	const char* vs =
		"#version 330 core\n"
		"layout (location = 0) in vec4 vertex;\n" // <vec2 pos, vec2 tex>
		"out vec2 TexCoords;\n"
		"uniform mat4 projection;\n"
		"void main()\n"
		"{\n"
		"	gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n"
		"	TexCoords = vertex.zw;\n"
		"}";
	const char* fs =
		"#version 330 core\n"
		"in vec2 TexCoords;\n"
		"out vec4 color;\n"
		"uniform sampler2D text;\n"
		"uniform vec3 textColor;\n"
		"void main()\n"
		"{\n"
		"	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n"
		"	color = vec4(textColor, 1.0) * sampled;\n"
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
	m_projLoc = glGetUniformLocation(m_program, "projection");
	m_textSamplerLoc = glGetUniformLocation(m_program, "text");
	m_textColorLoc = glGetUniformLocation(m_program, "textColor");

	glUseProgram(0);

	prepareGLObject();
}

CGRenderText::~CGRenderText()
{
	glDeleteProgram(m_program), m_program = 0;
	glDeleteBuffers(1, &m_VBO);
	glDeleteVertexArrays(1, &m_VAO);

	std::map<GLchar, Character>::iterator it;
	for (it = m_characters.begin(); it != m_characters.end(); ++it)
		glDeleteTextures(1, &((*it).second.TextureID));
}

void CGRenderText::renderText(
	const std::string& text, 
	Chan::ChReal x, 
	Chan::ChReal y, 
	Chan::ChReal scale, 
	const Chan::ChVector3& color)
{
	glUseProgram(m_program);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Chan::ChMat44 projection = Chan::Ortho(0.0f, m_scrWidth, 0.0, m_scrHeight);
	glUniformMatrix4fv(m_projLoc, 1, GL_FALSE, projection.data());
	glUniform3f(m_textColorLoc, color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(m_VAO);

	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); ++c)
	{
		Character ch = m_characters[*c];

		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;

		// Update VBO for each character
		GLfloat vertices[6][4] =
		{
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};

		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		
		void* memP = glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(vertices), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		memcpy(memP, vertices, sizeof(vertices));
		glUnmapBuffer(GL_ARRAY_BUFFER);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		x += (ch.Advance >> 6) * scale;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDisable(GL_BLEND);
}

void CGRenderText::prepareGLObject()
{
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);

	glBindVertexArray(m_VAO);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
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