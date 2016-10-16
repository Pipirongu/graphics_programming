#include "Texture.h"

Texture::Texture(QOpenGLFunctions_3_3_Core* functions)
{
	m_glFunctions = functions;
	m_textureID = NULL;
}


Texture::~Texture()
{
	m_glFunctions->glDeleteTextures(1, &m_textureID);
	m_glFunctions = NULL;
}

GLuint Texture::getTextureID()
{
	return m_textureID;
}

void Texture::loadTexture(QString texturepath)
{
	//let QT guess the file format of image and convert the image to a format openGL can handle
	QImage texture = QGLWidget::convertToGLFormat(QImage(texturepath));

	//Create openGL Texture
	m_glFunctions->glGenTextures(1, &m_textureID);
	//Bind it
	m_glFunctions->glBindTexture(GL_TEXTURE_2D, m_textureID);
	//Fill with data
	m_glFunctions->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width(), texture.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)texture.bits());

	// When MAGnifying the image (no bigger mipmap available), use LINEAR filtering
	m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// When MINifying the image, use a LINEAR blend of two mipmaps, each filtered LINEARLY too
	m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// Generate mipmaps, by the way.
	m_glFunctions->glGenerateMipmap(GL_TEXTURE_2D);
	//Unbind
	m_glFunctions->glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::loadSkyboxTexture(QString back, QString down, QString front, QString left, QString right, QString up)
{
	//let QT guess the file format of image and convert the image to a format openGL can handle
	QImage backTexture = QGLWidget::convertToGLFormat(QImage(back));
	QImage downTexture = QGLWidget::convertToGLFormat(QImage(down));
	QImage frontTexture = QGLWidget::convertToGLFormat(QImage(front));
	QImage leftTexture = QGLWidget::convertToGLFormat(QImage(left));
	QImage rightTexture = QGLWidget::convertToGLFormat(QImage(right));
	QImage upTexture = QGLWidget::convertToGLFormat(QImage(up));
		

	//Create openGL Texture
	m_glFunctions->glGenTextures(1, &m_textureID);
	//Bind it
	m_glFunctions->glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);

		
	m_glFunctions->glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, rightTexture.width(), rightTexture.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)rightTexture.bits());
	m_glFunctions->glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, upTexture.width(), upTexture.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)upTexture.bits());
	m_glFunctions->glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, frontTexture.width(), frontTexture.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)frontTexture.bits());

	m_glFunctions->glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, leftTexture.width(), leftTexture.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)leftTexture.bits());
	m_glFunctions->glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, downTexture.width(), downTexture.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)downTexture.bits());
	m_glFunctions->glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, backTexture.width(), backTexture.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)backTexture.bits());
		

	m_glFunctions->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_glFunctions->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	m_glFunctions->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	m_glFunctions->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	m_glFunctions->glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	//Unbind
	m_glFunctions->glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}