#include "Transform.h"

Transform::Transform()
{
	m_isRoot = false;
	m_isCamera = false;
	m_isPlayer = false;
	m_bvScaleFactor = 1;
	m_isSkybox = false;
}

Transform::~Transform(void)
{
	//Clear children list
	m_children.clear();
}

void Transform::update(Matrix44& view, const Matrix44& model)
{
	if (!m_isRoot){
		/*
		If the model matrix isn't reset and just multiplied with the parents model matrix.
		The matrices you just want to do once to place it in the world will add up and undesired effects occurs.
		For example the parent has a translation -10 and the child inherits it by matrix multiplication. 
		But the next render loop the child will have -10 and inherit additional translation from parent.
		Instead transform matrices are implemented to hold the updated transforms. Model matrix is reset and inherits these transforms + the parent's.
		*/
		m_model = m_rotation  * m_lookAt * m_translation  * m_rotationAroundOwnAxis * m_scaling; //A rotation is applied first to make the object rotate around itself.

		Matrix44 thisModel = m_model;
		//Multiply the parent's model matrix with this
		m_model = model * thisModel;
	}
	if (m_isCamera){
		//Third person camera
		//Invert the player's model matrix and set it as the view matrix
		view = m_model.Inverse();
	}
	if (m_isSkybox){

		//Get the scale matrix
		Matrix44 onlyScaleAndTranslation = m_scaling;

		//Insert the parents translation values
		onlyScaleAndTranslation[0][3] = model.GetElement(0, 3);
		onlyScaleAndTranslation[1][3] = model.GetElement(1, 3);
		onlyScaleAndTranslation[2][3] = model.GetElement(2, 3);

		//Set the matrix with only scale and translation as skybox's model matrix
		m_model = onlyScaleAndTranslation;
	}

	//Calls the childrens' update
	if (!m_children.empty())
	{
		for (int i = 0; i < m_children.size(); i++)
		{
			if (m_children[i] != NULL)
			{
				m_children[i]->update(view, m_model);
			}
		}
	}
}

void Transform::draw(ViewFrustumCheck& frustumCheck, const Matrix44& projection, const Matrix44& view, int shaderBranch, const Matrix44& lightView, const Matrix44& model, float bvScaleFactor)
{
	//Calculate the total scale factor inherited from parent transform and self, to later forward to children nodes
	float updatedScaleFactor = m_bvScaleFactor * bvScaleFactor;

	//Calls the childrens' update
	if (!m_children.empty())
	{
		for (int i = 0; i < m_children.size(); i++)
		{
			if (m_children[i] != NULL)
			{
				m_children[i]->draw(frustumCheck, projection, view, shaderBranch, lightView, m_model, updatedScaleFactor);
			}
		}
	}
}

Matrix44 Transform::getMatrix()
{
	return m_model;
}

void Transform::rotate(float angle, int x, int y, int z, RotateAround space)
{
	if (space == Self){ //Rotates around own axis
		m_rotationAroundOwnAxis.Rotate(angle, x, y, z);
	}
	else if (space == World){ //Rotate around world
		m_rotation.Rotate(angle, x, y, z);
	}
	
}

void Transform::rotate(float angle, Vector3 vector, RotateAround space)
{
	if (space == Self){ //Rotates around own axis
		m_rotationAroundOwnAxis.Rotate(angle, vector);
	}
	else if (space == World){ //Rotate around world
		m_rotation.Rotate(angle, vector);
	}
	
}

void Transform::translate( float x, float y, float z )
{
	m_translation.Translate(x, y, z);
}

void Transform::translate( Vector3 vector )
{
	m_translation.Translate(vector);
}

void Transform::scale( float factor )
{
	m_bvScaleFactor = factor;
	m_scaling.Scale(factor);
}

void Transform::setScale(float factor)
{
	m_bvScaleFactor = factor;
	m_scaling.SetToIdentity();
	m_scaling.Scale(factor);
}

void Transform::scale( float x, float y, float z )
{
	m_scaling.Scale(x, y, z);
}

void Transform::scale( Vector3 vector )
{
	m_scaling.Scale(vector);
}

void Transform::setNodeType(TransformType node)
{
	if(node == RootTransform){
		m_isRoot = true;
	}
	if(node == CameraTransform){
		m_isCamera = true;
	}
	if(node == PlayerTransform){
		m_isPlayer = true;
	}
	if (node == SkyboxTransform){
		m_isSkybox = true;
	}
}

void Transform::lookAt(Vector3 eyePosition, Vector3 eyeTarget, Vector3 eyeUp)
{
	m_lookAt.LookAt(eyePosition, eyeTarget, eyeUp);

	//Invert the lookAt matrix so it can be used to move an object
	Matrix44 invertedLookAt = m_lookAt.Inverse();
	m_lookAt = invertedLookAt;
}
