#ifndef PackedVertex_h__
#define PackedVertex_h__

//Math Library
#include "mypersonalmathlib/mypersonalmathlib.h"
#include <string.h>

/// <remarks>
///Stores the vertex data. PackedVertex is used as a key in a map inside indexVBO function
/// </remarks>
class PackedVertex
{
public:
	/// <summary>Constructor</summary>
	/// <returns></returns>
	PackedVertex();
	/// <summary>Destructor</summary>
	/// <returns></returns>
	~PackedVertex();

	/// <summary>Overloaded compare operator to allow this class to be used as a key in a map.</summary>
	/// <param name="that">Used to compare to decide the order on the map</param>
	/// <returns>bool</returns>
	bool operator<(const PackedVertex that) const;

	//Vertex data
	Vector3 position;
	Vector2 uv;
	Vector3 normal;

};

#endif // PackedVertex_h__
