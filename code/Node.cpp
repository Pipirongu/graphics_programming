#include "Node.h"

Node::Node()
{
}

Node::~Node(void)
{
	//Clear children list
	m_children.clear();
}

void Node::update(Matrix44& view, const Matrix44& model)
{
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

void Node::draw(ViewFrustumCheck& frustumCheck, const Matrix44& projection, const Matrix44& view, int shaderBranch, const Matrix44& lightView, const Matrix44& model, float bvScaleFactor)
{
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

void Node::addChildNode(Node* childNode)
{
	//Sets the child node's parent to this and adds the child to this node's children list
	if(childNode != NULL)
	{
		m_children.push_back(childNode);
	}
}

void Node::removeChildNode(Node* childNode)
{
	//Iterate the children list and find the the child. Then remove it from the list
	if (childNode != NULL && !m_children.empty())
	{
		for (int i = 0; i < m_children.size(); i++)
		{
			if (m_children[i] == childNode)
			{
				m_children.erase(m_children.begin() + i);
				break; //break the for loop
			}
		}
	}
}
