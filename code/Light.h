#ifndef Light_h__
#define Light_h__

#include "Node.h"
#include "Transform.h"

/// <remarks>
///Represents a Light
/// </remarks>
class Light : public Node
{
public:
	/// <summary>Constructor</summary>
	/// <param name="functions">Enables the class to call openGL functions</param>
	/// <returns></returns>
	Light(QOpenGLFunctions_3_3_Core* functions);
	/// <summary>Destructor</summary>
	/// <returns></returns>
	~Light();

	/// <summary>Updates the Light's world position and store it. And then call its children</summary>
	/// <param name="view">Is not handled in this class, just forwards it to its children</param>
	/// <param name="model">Multiplies this with its position vector, and forwards it to its children</param>
	/// <returns>void</returns>
	void update(Matrix44& view, const Matrix44& model = Matrix44());
	/// <summary>Sends Light Properties(Postion, Color, Intensity, Max radius of light) to shader uniforms. And then calls the childrens' draw</summary>
	/// <param name="frustumCheck">Is not handled in this class, just forwards it to its children</param>
	/// <param name="projection">Is not handled in this class, just forwards it to its children</param>
	/// <param name="view">Is not handled in this class, just forwards it to its children</param>
	/// <param name="shaderBranch">Is not handled in this class, just forwards it to its children</param>
	/// <param name="lightView">Is not handled in this class, just forwards it to its children</param>
	/// <param name="model">Is not handled in this class, just forwards it to its children</param>
	/// <param name="bvScaleFactor">Is not handled in this class, just forwards it to its children</param>
	/// <returns>void</returns>
	void draw(ViewFrustumCheck& frustumCheck, const Matrix44& projection, const Matrix44& view, int shaderBranch = -1, const Matrix44& lightView = Matrix44(), const Matrix44& model = Matrix44(), float bvScaleFactor = 1);	/// <summary>Assign a shader program the light should use to send the light position</summary>
	/// <summary>Specify which ID of shader program to use</summary>
	/// <param name="shaderProgram">ID of a shader program</param>
	/// <returns>void</returns>
	void useShaderProgram(GLuint shaderProgram);
	/// <summary>Sets the Color of the light</summary>
	/// <param name="r">red</param>
	/// <param name="g">green</param>
	/// <param name="b">blue</param>
	/// <returns>void</returns>
	void setLightColor(float r, float g, float b);
	/// <summary>Sets the intensity of the light</summary>
	/// <param name="factor">Intensity factor</param>
	/// <returns>void</returns>
	void setLightIntensity(float factor);
	/// <summary>Sets the Max radius of the point light(Currently only supported in deferred shading, shader code)</summary>
	/// <param name="scaleFactor">Scale Factor</param>
	/// <returns>void</returns>
	void setMaxLightRadius(float scaleFactor);

private:
	//Handles for shader uniforms
	GLuint m_lightPosLocation;
	GLuint m_lightColorLocation;
	GLuint m_lightIntensityLocation;
	GLuint m_lightMaxRadiusLocation;
	

	//Light position
	Vector3 m_lightPosition;
	Vector3 m_lightColor;
	float m_lightIntensity;
	float m_maxLightRadius;

	//Used to call native openGL functions
	QOpenGLFunctions_3_3_Core* m_glFunctions;

	//Holds the shader program that should be used by this mesh
	GLuint m_shaderProgram;
};

#endif // Light_h__
