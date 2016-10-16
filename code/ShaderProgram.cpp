#include "ShaderProgram.h"

ShaderProgram::ShaderProgram(QOpenGLFunctions_3_3_Core* functions)
{
	m_glFunctions = functions;
}


ShaderProgram::~ShaderProgram()
{
	//Unbind the shader program before deallocating
	m_glFunctions->glUseProgram(0);
	m_glFunctions->glDeleteProgram(m_shaderProgram);
	m_glFunctions = NULL;
}

void ShaderProgram::prepareShaderProgram(const char* vertex_file_path, const char* fragment_file_path)
{
	// Create the shaders
	GLuint VertexShaderID = m_glFunctions->glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = m_glFunctions->glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open())
	{
		std::string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()){
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	m_glFunctions->glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	m_glFunctions->glCompileShader(VertexShaderID);

	// Check Vertex Shader
	m_glFunctions->glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	m_glFunctions->glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> VertexShaderErrorMessage(InfoLogLength);
	m_glFunctions->glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	m_glFunctions->glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	m_glFunctions->glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	m_glFunctions->glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	m_glFunctions->glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
	m_glFunctions->glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

	// Link the program
	fprintf(stdout, "Linking program\n");
	GLuint ProgramID = m_glFunctions->glCreateProgram();
	m_glFunctions->glAttachShader(ProgramID, VertexShaderID);
	m_glFunctions->glAttachShader(ProgramID, FragmentShaderID);
	m_glFunctions->glLinkProgram(ProgramID);

	// Check the program
	m_glFunctions->glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	m_glFunctions->glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> ProgramErrorMessage(max(InfoLogLength, int(1)));
	m_glFunctions->glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

	m_glFunctions->glDeleteShader(VertexShaderID);
	m_glFunctions->glDeleteShader(FragmentShaderID);

	m_shaderProgram = ProgramID;
}

GLuint ShaderProgram::getShaderProgramID()
{
	return m_shaderProgram;
}
