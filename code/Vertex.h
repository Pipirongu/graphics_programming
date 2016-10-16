#ifndef Vertex_h__
#define Vertex_h__

//Math Library
#include "mypersonalmathlib/mypersonalmathlib.h"

//Forward Declaration
class HalfEdge;

/// <remarks>
///Represents a vertex. Stores position, uv and normal
/// </remarks>
class Vertex
{
public:
	/// <summary>Sets data members to NULL</summary>
	/// <returns></returns>
	Vertex();
	/// <summary>Destructor</summary>
	/// <returns></returns>
	~Vertex();

	//Vertex Data
	Vector3 pos;
	Vector2 uv;
	Vector3 normal;

	//New calculated position for old vertex
	Vector3 newPos;

	//An edge which has this vertex as starting point
	HalfEdge* edge;

};

#endif // Vertex_h__
