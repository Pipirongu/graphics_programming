#ifndef Transform_h__
#define Transform_h__

#include "Node.h"

/// <remarks>
///Used by Rotate functions to decide how the object should rotate
/// </remarks>
enum RotateAround
{
	Self,
	World
};

/// <remarks>
///Used by setNodeType function to set the node to a special one which will be handled differently
/// </remarks>
enum TransformType
{
	RootTransform,
	CameraTransform,
	PlayerTransform,
	SkyboxTransform
};

/// <remarks>
///Transform class (translation, rotation, scale)
/// </remarks>
class Transform : public Node
{
public:
	/// <summary>Constructor</summary>
	/// <returns></returns>
	Transform();
	/// <summary>Destructor</summary>
	/// <returns></returns>
	~Transform(void);

	/// <summary>Updates the model/view matrix and then calls its childrens' update/summary>
	/// <param name="view">Used only if the transform instance represents a camera. The view will store the inverse of the parents model matrix</param>
	/// <param name="model">the parent's model matrix</param>
	/// <returns>void</returns>
	void update(Matrix44& view, const Matrix44& model = Matrix44());
	/// <summary>Does nothing with this class. Just calls its childrens' update</summary>
	/// <param name="frustumCheck">Is not handled in this class, just forwards it to its children</param>
	/// <param name="projection">Is not handled in this class, just forwards it to its children</param>
	/// <param name="view">Is not handled in this class, just forwards it to its children</param>
	/// <param name="shaderBranch">Is not handled in this class, just forwards it to its children</param>
	/// <param name="lightView">Is not handled in this class, just forwards it to its children</param>
	/// <param name="model">Is not handled in this class, just forwards it to its children</param>
	/// <param name="bvScaleFactor">Is not handled in this class, just forwards it to its children</param>
	/// <returns>void</returns>
	void draw(ViewFrustumCheck& frustumCheck, const Matrix44& projection, const Matrix44& view, int shaderBranch = -1, const Matrix44& lightView = Matrix44(), const Matrix44& model = Matrix44(), float bvScaleFactor = 1);
	/// <summary>Returns the model/view matrix</summary>
	/// <returns>Matrix4x4</returns>
	Matrix44 getMatrix();

	/// <summary>Multiply the rotation matrix with a Rotation matrix</summary>
	/// <param name="angle">Rotation in degrees</param>
	/// <param name="x">X axis</param>
	/// <param name="y">Y axis</param>
	/// <param name="z">Z axis</param>
	/// <param name="space">Around which axis should the object rotate. (Self, World)</param>
	/// <returns>void</returns>
	void rotate(float angle, int x, int y, int z, RotateAround space = Self);
	/// <summary>Multiply the rotation matrix with a Rotation matrix</summary>
	/// <param name="angle">Rotation in degrees</param>
	/// <param name="vector">Vector3 holding the rotation axis</param>
	/// <param name="space">Around which axis should the object rotate. (Self, World)</param>
	/// <returns>void</returns>
	void rotate(float angle, Vector3 vector, RotateAround space = Self);

	/// <summary>Multiply the translation matrix with a Translation matrix.</summary>
	/// <param name="x">X axis</param>
	/// <param name="y">Y axis</param>
	/// <param name="z">Z axis</param>
	/// <returns>void</returns>
	void translate(float x, float y, float z);
	/// <summary>Multiply the translation matrix with a Translation matrix.</summary>
	/// <param name="vector">Vector3 holding the translation axis</param>
	/// <returns>void</returns>
	void translate(Vector3 vector);

	
	/// <summary>Multiply the scaling matrix with a Scaling matrix. Saves the scale factor so the bounding sphere can be scaled elsewhere in the program. Scales equally for xyz axis</summary>
	/// <param name="factor">scale factor</param>
	/// <returns>void</returns>
	void scale(float factor);
	///Sets the scale values in the matrix to factor
	/// <summary>Sets the scaling matrix to identity before multiplying it with a Scaling matrix. Saves the scale factor so the bounding sphere can be scaled elsewhere in the program. Scales equally for xyz axis</summary>
	/// <param name="factor">scale factor</param>
	/// <returns>void</returns>
	void setScale(float factor);
	/// <summary>Multiply the scaling matrix with a Scaling matrix. Doesn't have to scale uniformly</summary>
	/// <param name="x">Scale in X axis</param>
	/// <param name="y">Scale in Y axis</param>
	/// <param name="z">Scale in Z axis</param>
	/// <returns>void</returns>
	void scale(float x, float y, float z);
	/// <summary>Multiply the scaling matrix with a Scaling matrix. Doesn't have to scale uniformly</summary>
	/// <param name="vector">Vector3 holding the scaling for each axis</param>
	/// <returns>void</returns>
	void scale (Vector3 vector);

	/// <summary>Sets the transform node type (Root, Camera, Player)</summary>
	/// <param name="node">Enum (Root, Camera, Player)</param>
	/// <returns>void</returns>
	void setNodeType(TransformType node);

	/// <summary>Object lookAt function. Inverted camera lookAt</summary>
	/// <param name="eyePosition">The position of the object</param>
	/// <param name="eyeTarget">The Direction the object is looking</param>
	/// <param name="eyeUp">The up vector for the object</param>
	/// <returns>void</returns>
	void lookAt(Vector3 eyePosition, Vector3 eyeTarget, Vector3 eyeUp);

private:
	Matrix44 m_model;

	//Holds the translation values of this transform
	Matrix44 m_translation;
	//Holds the rotation values of this transform, applied after translation. So rotation speed around planets can be changed
	Matrix44 m_rotation;
	//Holds the scaling values of this transform
	Matrix44 m_scaling;
	//Rotation matrix applied first to get effect "rotate around own axis"
	Matrix44 m_rotationAroundOwnAxis;

	//A Combination of Translation/Rotation matrices
	Matrix44 m_lookAt;
	//Holds the scale factor for the transform which is passed to the children
	float m_bvScaleFactor;

	//Flags to help do stuff differently depending on what kind of transform node it is
	bool m_isRoot;
	bool m_isCamera;
	bool m_isPlayer;
	bool m_isSkybox;

};
#endif // Transform_h__
