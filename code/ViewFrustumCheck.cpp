#include "ViewFrustumCheck.h"


ViewFrustumCheck::ViewFrustumCheck()
{
	shapesRendered = 0;
}


ViewFrustumCheck::~ViewFrustumCheck()
{
}

void ViewFrustumCheck::extractFrustum(Matrix44 view, Matrix44 projection)
{
	float   viewProjectionGL[16];
	float   magnitude;

	Matrix44 viewProjection = projection*view;
	viewProjection.ConvertToOpenGLArray(viewProjectionGL);

	//RIGHT plane
	m_frustum[0][0] = viewProjectionGL[3] - viewProjectionGL[0];
	m_frustum[0][1] = viewProjectionGL[7] - viewProjectionGL[4];
	m_frustum[0][2] = viewProjectionGL[11] - viewProjectionGL[8];
	m_frustum[0][3] = viewProjectionGL[15] - viewProjectionGL[12];

	//Normalize Right plane
	magnitude = sqrt(m_frustum[0][0] * m_frustum[0][0] + m_frustum[0][1] * m_frustum[0][1] + m_frustum[0][2] * m_frustum[0][2]);
	m_frustum[0][0] /= magnitude;
	m_frustum[0][1] /= magnitude;
	m_frustum[0][2] /= magnitude;
	m_frustum[0][3] /= magnitude;

	//LEFT plane
	m_frustum[1][0] = viewProjectionGL[3] + viewProjectionGL[0];
	m_frustum[1][1] = viewProjectionGL[7] + viewProjectionGL[4];
	m_frustum[1][2] = viewProjectionGL[11] + viewProjectionGL[8];
	m_frustum[1][3] = viewProjectionGL[15] + viewProjectionGL[12];

	//Normalize Left plane
	magnitude = sqrt(m_frustum[1][0] * m_frustum[1][0] + m_frustum[1][1] * m_frustum[1][1] + m_frustum[1][2] * m_frustum[1][2]);
	m_frustum[1][0] /= magnitude;
	m_frustum[1][1] /= magnitude;
	m_frustum[1][2] /= magnitude;
	m_frustum[1][3] /= magnitude;

	//BOTTOM plane
	m_frustum[2][0] = viewProjectionGL[3] + viewProjectionGL[1];
	m_frustum[2][1] = viewProjectionGL[7] + viewProjectionGL[5];
	m_frustum[2][2] = viewProjectionGL[11] + viewProjectionGL[9];
	m_frustum[2][3] = viewProjectionGL[15] + viewProjectionGL[13];

	//Normalize Bottom plane
	magnitude = sqrt(m_frustum[2][0] * m_frustum[2][0] + m_frustum[2][1] * m_frustum[2][1] + m_frustum[2][2] * m_frustum[2][2]);
	m_frustum[2][0] /= magnitude;
	m_frustum[2][1] /= magnitude;
	m_frustum[2][2] /= magnitude;
	m_frustum[2][3] /= magnitude;

	//TOP plane
	m_frustum[3][0] = viewProjectionGL[3] - viewProjectionGL[1];
	m_frustum[3][1] = viewProjectionGL[7] - viewProjectionGL[5];
	m_frustum[3][2] = viewProjectionGL[11] - viewProjectionGL[9];
	m_frustum[3][3] = viewProjectionGL[15] - viewProjectionGL[13];

	//Normalize Top plane
	magnitude = sqrt(m_frustum[3][0] * m_frustum[3][0] + m_frustum[3][1] * m_frustum[3][1] + m_frustum[3][2] * m_frustum[3][2]);
	m_frustum[3][0] /= magnitude;
	m_frustum[3][1] /= magnitude;
	m_frustum[3][2] /= magnitude;
	m_frustum[3][3] /= magnitude;

	//FAR plane
	m_frustum[4][0] = viewProjectionGL[3] - viewProjectionGL[2];
	m_frustum[4][1] = viewProjectionGL[7] - viewProjectionGL[6];
	m_frustum[4][2] = viewProjectionGL[11] - viewProjectionGL[10];
	m_frustum[4][3] = viewProjectionGL[15] - viewProjectionGL[14];

	//Normalize Far plane
	magnitude = sqrt(m_frustum[4][0] * m_frustum[4][0] + m_frustum[4][1] * m_frustum[4][1] + m_frustum[4][2] * m_frustum[4][2]);
	m_frustum[4][0] /= magnitude;
	m_frustum[4][1] /= magnitude;
	m_frustum[4][2] /= magnitude;
	m_frustum[4][3] /= magnitude;

	//NEAR plane
	m_frustum[5][0] = viewProjectionGL[3] + viewProjectionGL[2];
	m_frustum[5][1] = viewProjectionGL[7] + viewProjectionGL[6];
	m_frustum[5][2] = viewProjectionGL[11] + viewProjectionGL[10];
	m_frustum[5][3] = viewProjectionGL[15] + viewProjectionGL[14];

	//Normalize Near plane
	magnitude = sqrt(m_frustum[5][0] * m_frustum[5][0] + m_frustum[5][1] * m_frustum[5][1] + m_frustum[5][2] * m_frustum[5][2]);
	m_frustum[5][0] /= magnitude;
	m_frustum[5][1] /= magnitude;
	m_frustum[5][2] /= magnitude;
	m_frustum[5][3] /= magnitude;
}

bool ViewFrustumCheck::bSphereInFrustum(Vector3 centerPosition, float radius)
{
	int insidePlaneCounter = 0;
	float distance;

	for (int i = 0; i < 6; i++)
	{
		//Plane equation
		distance = m_frustum[i][0] * centerPosition[0] + m_frustum[i][1] * centerPosition[1] + m_frustum[i][2] * centerPosition[2] + m_frustum[i][3];

		//Distance returns a negative value if the point tested is on the left of the plane
		//Returns false if the distance is greater than the radius. Which means that the bounding sphere is fully outside
		if (distance <= -radius){
			return false;
		}
		//Keep track of how many planes the bounding sphere is completely inside
		if (distance > radius){
			insidePlaneCounter++;
		}
	}

	if (insidePlaneCounter == 6){
		//Is completetly inside all planes
	}
	//Returns true if the bounding sphere is partially or completely inside the planes
	return true;
}