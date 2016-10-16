#ifndef Mesh_h__
#define Mesh_h__

#include "Node.h"
//For typecasting parent node to Transform
#include "Transform.h"
//For class representing key for map in the vertex indexing algorithm
#include "PackedVertex.h"


//For opening files
#include <stdio.h>
#include <stdlib.h>

/// <remarks>
///Used by setNodeType function to set the node to a special one which will be handled differently
/// </remarks>
enum MeshType
{
	PlayerMesh,
	SkyboxMesh
};

/// <remarks>
///Represents the mesh of an object. Can load obj files.
/// </remarks>
class Mesh : public Node
{
public:
	/// <summary>Constructor</summary>
	/// <param name="functions">Enables the class to call openGL functions</param>
	/// <returns></returns>
	Mesh(QOpenGLFunctions_3_3_Core* functions);
	/// <summary>Destructor</summary>
	/// <returns></returns>
	~Mesh();

	/// <summary>Does nothing with this class. Just calls its childrens' update</summary>
	/// <param name="view">Is not handled in this class, just forwards it to its children</param>
	/// <param name="model">Is not handled in this class, just forwards it to its children</param>
	/// <returns>void</returns>
	void update(Matrix44& view, const Matrix44& model = Matrix44());
	/// <summary>Check the bounding sphere of the mesh if it's inside the view frustum. Draw the mesh if it's inside. And then calls the childrens' draw</summary>
	/// <param name="frustumCheck">Has the view frustum planes and function to check if bounding sphere is inside of the frustum</param>
	/// <param name="projection">a projection matrix</param>
	/// <param name="view">a view matrix</param>
	/// <param name="shaderBranch">Will have the ID of current shader branch. If value is -1 draw call will not draw wireframes.
	/// This prevents the program to draw wireframe in Multipass rendering</param>
	/// <param name="lightView">Light's Camera view Matrix. Only used in shadow map</param>
	/// <param name="model">a model matrix</param>
	/// <param name="bvScaleFactor">Scale factor used to scale the bounding sphere's radius</param>
	/// <returns>void</returns>
	void draw(ViewFrustumCheck& frustumCheck, const Matrix44& projection, const Matrix44& view, int shaderBranch = -1, const Matrix44& lightView = Matrix44(), const Matrix44& model = Matrix44(), float bvScaleFactor = 1);	/// <summary>Assign a shader program to be used to render the object with</summary>
	/// <summary>Specify which ID of shader program to use</summary>
	/// <param name="shaderProgram">ID of a shader program</param>
	/// <returns>void</returns>
	void useShaderProgram(GLuint shaderProgram);
	/// <summary>Assign a texture to be used to render the object with</summary>
	/// <param name="textureID">ID of a shader program</param>
	/// <returns>void</returns>
	void useTexture(GLuint textureID);
	/// <summary>Object Loader. Takes the path to the obj file. Reads the data and puts it into VBOs on the VRAM. Also generates the bounding sphere for the mesh</summary>
	/// <param name="path">Path to obj file</param>
	/// <returns>void</returns>
	void loadOBJ(const char* path);

	/// <summary>Sets the material property for ambient light</summary>
	/// <param name="r">red value</param>
	/// <param name="g">green value</param>
	/// <param name="b">blue value</param>
	/// <returns>void</returns>
	void setAmbientMaterial(float r, float g, float b);
	/// <summary>Sets the material property for specular light and amount of shininess</summary>
	/// <param name="r">red value</param>
	/// <param name="g">green value</param>
	/// <param name="b">blue value</param>
	/// <param name="shininess">amount of shininess</param>
	/// <returns>void</returns>
	void setSpecularMaterial(float r, float g, float b, float shininess = 5);
	/// <summary>Sets the type of mesh (currently only meant to be used to set the mesh as player to disable rendering the bounding sphere)</summary>
	/// <param name="node">Enum (Player)</param>
	/// <returns>void</returns>
	void setNodeType(MeshType node);
	/// <summary>Sets a flag if wireframe mode is used or not. Prevent drawing the wireframe bounding sphere if flag is off</summary>
	/// <param name="flag">On or Off</param>
	/// <returns>void</returns>
	void setWireframeMode(bool flag);
	/// <summary>Sets a flag if frustum culling should be used for this mesh</summary>
	/// <param name="flag">On or Off</param>
	/// <returns>void</returns>
	void setFrustumCulling(bool flag);
	/// <summary>Sets a flag if wireframe bound sphere should be rendered</summary>
	/// <param name="flag">On or Off</param>
	/// <returns>void</returns>
	void setWireframeBV(bool flag);
	/// <summary>Initialize VBO for a sphere which represents the bounding sphere of this mesh</summary>
	/// <returns>void</returns>
	void initWireframeBoundingSphere();
	/// <summary>Deallocate resources used by wirefram bounding sphere</summary>
	/// <returns>void</returns>
	void releaseWireframeBoundingSphere();
	
private:
	//Handles for shader uniforms
	GLuint m_shaderModeLocation; //Uniform to select a shader branch from the uber-shader
	GLuint m_skyBoxSamplerLocation;
	GLuint m_textureSamplerLocation;
	GLuint m_depthMVPLocation;
	GLuint m_ambientMaterialLocation;
	GLuint m_specularMaterialLocation;
	GLuint m_shininessLocation;

	GLuint m_mvpLocation;
	GLuint m_modelLocation;
	GLuint m_viewLocation;
	GLuint m_modelViewLocation;

	//vertices, uvs, normals, indices
	std::vector<Vector3> m_vertices;
	std::vector<Vector2> m_uvs;
	std::vector<Vector3> m_normals;
	std::vector<unsigned int> m_indices;

	//Center Point of bounding sphere in model space
	Vector3 m_centerPointBV;
	//Radius of bounding sphere
	float m_radiusBV;

	bool m_isWireframe;
	bool m_isPlayer;
	bool m_isFrustumCulling;
	bool m_isWireframeBV;
	bool m_isSkybox;

	//Used to call native openGL functions
	QOpenGLFunctions_3_3_Core* m_glFunctions;

	//VAO
	GLuint m_vao;
	//VBO Vertices
	GLuint m_vertexVBO;
	//VBO UVS
	GLuint m_uvVBO;
	//VBO Normals
	GLuint m_normalVBO;
	//EBO Indices
	GLuint m_indicesEBO;

	//Holds the texture that should be used by this mesh
	GLuint m_textureID;

	//Holds the shader program that should be used by this mesh
	GLuint m_shaderProgram;

	//Material Properties
	Vector3 m_ambientMaterial;
	Vector3 m_specularMaterial;
	float m_shininess;


	//vertices for wireframe bounding sphere
	std::vector<Vector3> m_wireframeBvVertices;

	//VAO bounding sphere
	GLuint m_wireframeBvVAO;
	//VBO Vertices bounding sphere
	GLuint m_wireframeBvVBO;

	/// <summary>Used by indexVBO function to compare a vertex with vertices in a list. If one is found in the list. Function returns true and result parameter will hold the index of the vertex found in the list</summary>
	/// <param name="packed">Holds vertex data for one vertex</param>
	/// <param name="VertexToOutIndex">List of vertices as keys associated with their index as value</param>
	/// <param name="result">Stores the index of vertex found in VertexToOutIndex list</param>
	/// <returns>bool</returns>
	bool getSimilarVertexIndex(PackedVertex& packed, std::map<PackedVertex, unsigned int>& VertexToOutIndex, unsigned int& result);
	/// <summary>Vertex indexing. Generates lists of vertices with no duplicates. An indices list is also generated to tell which vertices should form a triangle.</summary>
	/// <param name="in_vertices">list of vertices to index</param>
	/// <param name="in_uvs">list of uvs to index</param>
	/// <param name="in_normals">list of normals to index</param>
	/// <returns>void</returns>
	void indexVBO(std::vector<Vector3>& in_vertices, std::vector<Vector2>& in_uvs, std::vector<Vector3>& in_normals);
	/// <summary>Calculates a center point for the mesh and calculates the radius from the centerpoint which will encapsulate the whole object</summary>
	/// <returns>void</returns>
	void calculateBoundingSphere();
	/// <summary>Draws the wireframe bounding sphere for this mesh</summary>
	/// <param name="view">a view matrix</param>
	/// <param name="projection">a projection matrix</param>
	/// <param name="position">position of the bounding sphere</param>
	/// <param name="radius">radius of the bounding sphere</param>
	/// <returns>void</returns>
	void drawWireframeBoundingSphere(const Matrix44& view, const Matrix44& projection, Vector3 position, float radius);
};

#endif // Mesh_h__
