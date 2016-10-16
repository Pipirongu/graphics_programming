#ifndef HalfEdge_h__
#define HalfEdge_h__

//Forward Declaration
class Vertex;
class HalfEdge;
class Face;

/// <remarks>
///Represents an halfedge of a face
/// </remarks>
class HalfEdge
{
public:
	/// <summary>Sets data members to NULL</summary>
	/// <returns></returns>
	HalfEdge();
	/// <summary>Destructor</summary>
	/// <returns></returns>
	~HalfEdge();

	//The vertex this half-edge starts from
	Vertex* origin; 
	//Points to a vertex that is this one's pair(opposite vertices)
	HalfEdge* pair;
	//Pointer to the next half-edge
	HalfEdge* next;
	//Stores the new vertex representing the midpoint where the halfedge should split
	Vertex* midpoint;
	//Face that this half-edge belongs to
	Face* face;
};

#endif // HalfEdge_h__
