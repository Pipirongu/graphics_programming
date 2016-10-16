#ifndef Texture_h__
#define Texture_h__

//For storing texture image
#include <QImage>
//OpenGL Functions
#include <QOpenGLFunctions_3_3_Core>
//For converting QImage to openGL Format
#include <QtOpenGL/QGLWidget>

/// <remarks>
///Holds the texture of a mesh
/// </remarks>
class Texture
{
public:
	/// <summary>Constructor</summary>
	/// <param name="functions">Enables the class to call openGL functions</param>
	/// <returns></returns>
	Texture(QOpenGLFunctions_3_3_Core* functions);
	/// <summary>Destructor</summary>
	/// <returns></returns>
	~Texture();

	/// <summary>Returns the ID of the texture that was put in VRAM</summary>
	/// <returns>GLuint</returns>
	GLuint getTextureID();
	/// <summary>Loads a texture to be used by a mesh</summary>
	/// <param name="texturepath">Path of the texture</param>
	/// <returns></returns>
	void loadTexture(QString texturepath);
	/// <summary>Loads 6 images for cube map meant to be used by a skybox mesh</summary>
	/// <param name="back">Path of the back(-Z) texture</param>
	/// <param name="down">Path of the down(-Y) texture</param>
	/// <param name="front">Path of the front(+Z) texture</param>
	/// <param name="left">Path of the left(-X) texture</param>
	/// <param name="right">Path of the right(+X) texture</param>
	/// <param name="up">Path of the up(+Y) texture</param>
	/// <returns></returns>
	void loadSkyboxTexture(QString back, QString down, QString front, QString left, QString right, QString up);

private:
	//Used to call native openGL functions
	QOpenGLFunctions_3_3_Core* m_glFunctions;

	//Holds the texture that should be used by this mesh
	GLuint m_textureID;
};
#endif // Texture_h__

