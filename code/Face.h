#ifndef Face_h__
#define Face_h__

//Forward Declaration
class HalfEdge;

/// <remarks>
///Face structure for HalfEdge Mesh
/// </remarks>
class Face
{
public:
	/// <summary>Sets data members to NULL</summary>
	/// <returns></returns>
	Face();
	/// <summary>Destructor</summary>
	/// <returns></returns>
	~Face();

	//Can point to any edge which belongs to the face
	HalfEdge* edge;
};

#endif // Face_h__
