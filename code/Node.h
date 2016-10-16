#ifndef Node_h__
#define Node_h__

//OpenGL Functions
#include <QOpenGLFunctions_3_3_Core>

//Math Library
#include "mypersonalmathlib/mypersonalmathlib.h"

#include "ViewFrustumCheck.h"

#include <vector>

//Debugging output
#include <QDebug>

/// <remarks>
///Abstract Base Class for Nodes
/// </remarks>
class Node
{
public:
	/// <summary>Constructor</summary>
	/// <returns></returns>
	Node();
	/// <summary>Pure virtual destructor to make this class abstract</summary>
	/// <returns></returns>
	virtual ~Node(void) = 0;

	/// <summary>Call the childrens' update. Reimplement in subclass for dynamic binding</summary>
	/// <param name="view">a view matrix</param>
	/// <param name="model">a model matrix</param>
	/// <returns>void</returns>
	virtual void update(Matrix44& view, const Matrix44& model = Matrix44());
	/// <summary>Call the childrens' draw. Reimplement in subclass for dynamic binding</summary>
	/// <param name="frustumCheck">Has the view frustum planes and function to check if bounding sphere is inside of the frustum</param>
	/// <param name="projection">a projection matrix</param>
	/// <param name="view">a view matrix</param>
	/// <param name="shaderBranch">Will have the ID of current shader branch. If value is -1 draw call will not draw wireframes.
	/// This prevents the program to draw wireframe in Multipass rendering</param>
	/// <param name="lightView">Light's Camera view Matrix. Only used in shadow map</param>
	/// <param name="model">a model matrix</param>
	/// <param name="bvScaleFactor">Scale factor used to scale the bounding sphere's radius</param>
	/// <returns>void</returns>
	virtual void draw(ViewFrustumCheck& frustumCheck, const Matrix44& projection, const Matrix44& view, int shaderBranch = -1, const Matrix44& lightView = Matrix44(), const Matrix44& model = Matrix44(), float bvScaleFactor = 1);
	/// <summary>Adds a child node</summary>
	/// <param name="childNode">A node to add as child for this</param>
	/// <returns>void</returns>
	void addChildNode(Node* childNode);
	/// <summary>Removes a child node</summary>
	/// <param name="childNode">A node to find as child to remove</param>
	/// <returns>void</returns>
	void removeChildNode(Node* childNode);

protected:
	//Holds children nodes
	std::vector<Node*> m_children;
};
#endif // Node_h__
