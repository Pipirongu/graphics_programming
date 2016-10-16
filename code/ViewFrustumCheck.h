#ifndef ViewFrustumCheck_h__
#define ViewFrustumCheck_h__

#include "mypersonalmathlib/mypersonalmathlib.h"


/// <remarks>
///Holds the planes of the view frustum. Has function to extract the planes and sphere to plane intersection function
/// </remarks>
class ViewFrustumCheck
{
public:
	/// <summary>Constructor</summary>
	/// <returns></returns>
	ViewFrustumCheck();
	/// <summary>Destructor</summary>
	/// <returns></returns>
	~ViewFrustumCheck();

	/// <summary>Extracts the 6 planes of the view frustum from the ViewProjection matrix</summary>
	/// <param name="view">view matrix</param>
	/// <param name="projection">projection matrix</param>
	/// <returns>void</returns>
	void extractFrustum(Matrix44 view, Matrix44 projection);
	/// <summary>Checks if a bounding sphere intersects with the plane. Returns true if the bounding sphere is partially or completely inside the view frustum</summary>
	/// <param name="centerPosition">Center position of the bounding sphere in world space</param>
	/// <param name="radius">radius of the bounding sphere</param>
	/// <returns>bool</returns>
	bool bSphereInFrustum(Vector3 centerPosition, float radius);

	//Counter for amount of objects that are actually rendered
	unsigned int shapesRendered;

private:
	//Stores the frustum planes
	float m_frustum[6][4];
};

#endif // ViewFrustumCheck_h__
