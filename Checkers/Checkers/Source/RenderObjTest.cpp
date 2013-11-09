#include "RenderObjTest.h"

void RenderObjTest::Render()
{
	// 48 is specified as the count because that's how many indices we need to draw the image
	// Essentially, that's how many vertices we need, but not necessarily unique vertices
	// GL_UNSIGNED_BYTE is used because we must define the type that we used
	// to store the indices.
	// The final parameter allows us to specify where in the indices array
	// we would like to start sampling for triangle drawing.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glDrawElements(GL_TRIANGLES, 48, GL_UNSIGNED_BYTE, (GLvoid*)0);
	BindForRender();
	glDrawElements(GL_TRIANGLES, 48, GL_UNSIGNED_BYTE, (GLvoid*)0);
	UnbindForRender();
}

void RenderObjTest::BindForRender()
{
	glUseProgram(m_renderInfo.ProgramId);
	glBindVertexArray(m_renderInfo.VaoId);
	glBindBuffer(GL_ARRAY_BUFFER, m_renderInfo.VboId);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_renderInfo.IndexBufferId);

}

void RenderObjTest::UnbindForRender()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}


void RenderObjTest::CreateVBO(void)
{
	Vertex Vertices[] =
	{
		{ { 0.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } },
		// Top
		{ { -0.2f, 0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
		{ { 0.2f, 0.8f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
		{ { 0.0f, 0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
		{ { 0.0f, 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
		// Bottom
		{ { -0.2f, -0.8f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
		{ { 0.2f, -0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
		{ { 0.0f, -0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
		{ { 0.0f, -1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
		// Left
		{ { -0.8f, -0.2f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
		{ { -0.8f, 0.2f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
		{ { -0.8f, 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
		{ { -1.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
		// Right
		{ { 0.8f, -0.2f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
		{ { 0.8f, 0.2f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
		{ { 0.8f, 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
		{ { 1.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } }
	};

	// defines the order to draw vertices in
	GLubyte Indices[] = {
		// Top
		0, 1, 3,
		0, 3, 2,
		3, 1, 4,
		3, 4, 2,
		// Bottom
		0, 5, 7,
		0, 7, 6,
		7, 5, 8,
		7, 8, 6,
		// Left
		0, 9, 11,
		0, 11, 10,
		11, 9, 12,
		11, 12, 10,
		// Right
		0, 13, 15,
		0, 15, 14,
		15, 13, 16,
		15, 16, 14
	};


	GLenum ErrorCheckValue = glGetError();
	const size_t BufferSize = sizeof(Vertices);
	const size_t VertexSize = sizeof(Vertices[0]);
	const size_t RgbOffset = sizeof(Vertices[0].Position);

	// Generates array that describes how the vertex attributes are stored in VBO
	glGenVertexArrays(1, &m_renderInfo.VaoId);
	glBindVertexArray(m_renderInfo.VaoId);

	// Generates the actual buffer, and handle, to store the vertices
	glGenBuffers(1, &m_renderInfo.VboId);
	// Makes VboId the current buffer by binding it as a GL_ARRAY_BUFFER. 
	// Until another is bound, or this one unbound,
	// all buffer related operations execute on this buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_renderInfo.VboId);
	// Uploads data to currently bound buffer
	glBufferData(GL_ARRAY_BUFFER, BufferSize, Vertices, GL_STATIC_DRAW);
	// glVertexAttribPointer defines the data that was just buffered by labeling 
	// it as the type associated with index=0 in the vertex shader.
	// We point to the attrib index, define the count, describe the type of data it is,
	// disable normalization, 
	// describe the stride between variables as VertexSize because that's
	// the size of the struct we're using as a variable,
	// and finally the pointer offset is 0 because we passed a single type data array
	// and there is no required offset to access the data we uploaded.
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, VertexSize, 0);
	// index has changed to 1 because it is the second vertex attribute.
	// changed pointer offset to RgbOffset to signify distance from start of 
	// individual vertex data to the color data
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)RgbOffset);
	// Enables the vertex attribute (variable) for use during drawing.
	// Should be disabled after this buffer is done being drawn.
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// Generates index buffer id
	glGenBuffers(1, &m_renderInfo.IndexBufferId);
	// specifies that the buffer we're uploading is an array that details
	// which vertices we are using and in what order
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_renderInfo.IndexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);


	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
	{
		fprintf(
			stderr,
			"ERROR: Could not create a VBO: %s \n",
			gluErrorString(ErrorCheckValue)
			);

		exit(-1);
	}
	UnbindForRender();
}

void RenderObjTest::DestroyVBO(void)
{
	GLenum ErrorCheckValue = glGetError();

	UnbindForRender();

	glDeleteBuffers(1, &m_renderInfo.VboId);

	glDeleteBuffers(1, &m_renderInfo.IndexBufferId);

	glDeleteVertexArrays(1, &m_renderInfo.VaoId);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
	{
		fprintf(
			stderr,
			"ERROR: Could not destroy the VBO: %s \n",
			gluErrorString(ErrorCheckValue)
			);

		exit(-1);
	}
}

void RenderObjTest::CreateShaders(void)
{
	GLenum ErrorCheckValue = glGetError();

	if (ErrorCheckValue != GL_NO_ERROR)
	{
		fprintf(
			stderr,
			"ERROR: Could not create the shaders: %s \n",
			gluErrorString(ErrorCheckValue)
			);

		exit(-1);
	}

	m_renderInfo.VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(m_renderInfo.VertexShaderId, 1, &VertexShader, NULL);
	glCompileShader(m_renderInfo.VertexShaderId);

	m_renderInfo.FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(m_renderInfo.FragmentShaderId, 1, &FragmentShader, NULL);
	glCompileShader(m_renderInfo.FragmentShaderId);

	m_renderInfo.ProgramId = glCreateProgram();
	glAttachShader(m_renderInfo.ProgramId, m_renderInfo.VertexShaderId);
	glAttachShader(m_renderInfo.ProgramId, m_renderInfo.FragmentShaderId);
	glLinkProgram(m_renderInfo.ProgramId);
	glUseProgram(m_renderInfo.ProgramId);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
	{
		fprintf(
			stderr,
			"ERROR: Could not create the shaders: %s \n",
			gluErrorString(ErrorCheckValue)
			);

		exit(-1);
	}
}

void RenderObjTest::DestroyShaders(void)
{
	GLenum ErrorCheckValue = glGetError();

	glUseProgram(0);

	glDetachShader(m_renderInfo.ProgramId, m_renderInfo.VertexShaderId);
	glDetachShader(m_renderInfo.ProgramId, m_renderInfo.FragmentShaderId);

	glDeleteShader(m_renderInfo.FragmentShaderId);
	glDeleteShader(m_renderInfo.VertexShaderId);

	glDeleteProgram(m_renderInfo.ProgramId);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
	{
		fprintf(
			stderr,
			"ERROR: Could not destroy the shaders: %s \n",
			gluErrorString(ErrorCheckValue)
			);

		exit(-1);
	}
}