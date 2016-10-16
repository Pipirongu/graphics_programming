#include "Light.h"

Light::Light(QOpenGLFunctions_3_3_Core* functions)
{
	m_glFunctions = functions;
	//Set default Light properties
	m_lightColor.Insert(1, 1, 1);
	m_lightIntensity = 2.0f;
}

Light::~Light()
{
	m_glFunctions = NULL;

	//Clear children list
	m_children.clear();
}

void Light::update(Matrix44& view, const Matrix44& model)
{	
	//Get parent's model matrix
	Matrix44 parent_Transform = model;

	//Light position in world space
	Vector4 lightPos;
	Vector4 worldSpaceLightPosition = parent_Transform * lightPos;
	m_lightPosition.Insert(worldSpaceLightPosition[0], worldSpaceLightPosition[1], worldSpaceLightPosition[2]);

	//Calls the childrens' update
	if (!m_children.empty())
	{
		for (int i = 0; i < m_children.size(); i++)
		{
			if (m_children[i] != NULL)
			{
				m_children[i]->update(view, model);
			}
		}
	}
}

void Light::draw(ViewFrustumCheck& frustumCheck, const Matrix44& projection, const Matrix44& view, int shaderBranch, const Matrix44& lightView, const Matrix44& model, float bvScaleFactor)
{
	//Send Light properties to shader uniforms
	m_glFunctions->glUniform3fv(m_lightPosLocation, 1, &m_lightPosition[0]);
	m_glFunctions->glUniform3fv(m_lightColorLocation, 1, &m_lightColor[0]);
	m_glFunctions->glUniform1f(m_lightIntensityLocation, m_lightIntensity);
	m_glFunctions->glUniform1f(m_lightMaxRadiusLocation, m_maxLightRadius);

	//Calls the childrens' draw
	if (!m_children.empty())
	{
		for (int i = 0; i < m_children.size(); i++)
		{
			if (m_children[i] != NULL)
			{
				m_children[i]->draw(frustumCheck, projection, view, shaderBranch, lightView, model, bvScaleFactor);
			}
		}
	}
}

void Light::useShaderProgram(GLuint shaderProgram)
{
	m_shaderProgram = shaderProgram;

	//Get handles for shader uniforms
	m_lightPosLocation = m_glFunctions->glGetUniformLocation(m_shaderProgram, "LightPosition_worldspace");
	m_lightColorLocation = m_glFunctions->glGetUniformLocation(m_shaderProgram, "LightColor");
	m_lightIntensityLocation = m_glFunctions->glGetUniformLocation(m_shaderProgram, "LightIntensity");
	m_lightMaxRadiusLocation = m_glFunctions->glGetUniformLocation(m_shaderProgram, "maxLightRadius");
}

void Light::setLightColor(float r, float g, float b)
{
	m_lightColor.Insert(r, g, b);
}

void Light::setLightIntensity(float factor)
{
	m_lightIntensity = factor;
}

void Light::setMaxLightRadius(float scaleFactor)
{
	m_maxLightRadius = scaleFactor;
}
