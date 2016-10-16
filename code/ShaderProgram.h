#ifndef ShaderProgram_h__
#define ShaderProgram_h__

//OpenGL Functions
#include <QOpenGLFunctions_3_3_Core>

//For reading shader files
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm> //max
using namespace std;

#include <stdlib.h>
#include <string.h>

/// <remarks>
///Holds the compiled and linked vertex- and fragment shader
/// </remarks>
class ShaderProgram
{
public:
	/// <summary>Constructor</summary>
	/// <param name="functions">Enables the class to call openGL functions</param>
	/// <returns></returns>
	ShaderProgram(QOpenGLFunctions_3_3_Core* functions);
	/// <summary>Destructor</summary>
	/// <returns></returns>
	~ShaderProgram();

	/// <summary>Compile and Link the vertex and fragment shaders</summary>
	/// <param name="vertex_file_path">path to vertex shader</param>
	/// <param name="fragment_file_path">path to fragment shader</param>
	/// <returns>void</returns>
	void prepareShaderProgram(const char* vertex_file_path, const char* fragment_file_path);
	/// <summary>Returns the ID of the shader program</summary>
	/// <returns>GLuint</returns>
	GLuint getShaderProgramID();

private:
	//Used to call native openGL functions
	QOpenGLFunctions_3_3_Core* m_glFunctions;

	//Holds the compiled and linked vertex and fragment shader from function prepareShaderProgram
	GLuint m_shaderProgram;
};

#endif // ShaderProgram_h__
