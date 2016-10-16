#ifndef HalfEdgeMesh_h__
#define HalfEdgeMesh_h__

#include "Node.h"
//For typecasting parent node to Transform
#include "Transform.h"
//For class representing key for map in the vertex indexing algorithm
#include "PackedVertex.h"

//For opening files
#include <stdio.h>
#include <stdlib.h>


//Half-Edge Data Structure
#include "Face.h"
#include "HalfEdge.h"
#include "Vertex.h"

#include <QTime>
//Used to save mesh to file
#include <QFileDialog>

/// <remarks>
///Used by setNodeType function to set the node to a special one which will be handled differently
/// </remarks>
enum HalfEdgeMeshType
{
	PlayerHalfEdgeMesh,
	SkyboxHalfEdgemesh
};

/// <remarks>
///Represents a mesh of an object. Can load obj files. 
///A half edge mesh data structure is generated when using this class.
///Has function to subdivided the mesh
/// </remarks>
class HalfEdgeMesh : public Node
{
public:
	/// <summary>Constructor</summary>
	/// <param name="functions">Enables the class to call openGL functions</param>
	/// <returns></returns>
	HalfEdgeMesh(QOpenGLFunctions_3_3_Core* functions);
	/// <summary>Destructor</summary>
	/// <returns></returns>
	~HalfEdgeMesh();

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
	/// <summary>Object Loader. Takes the path to the obj file. 
	///Reads the data and generate lists for rendering the original mesh and creates a half-edge mesh data structure.
	///From the half-edge mesh data structure lists for rendering are also created. Lastly it calculates the bounding sphere for the mesh </summary>
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
	void setNodeType(HalfEdgeMeshType node);
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
	/// <summary>Sets a flag if wireframe for original mesh should be rendered</summary>
	/// <param name="flag">On or Off</param>
	/// <returns>void</returns>
	void setWireframeOriginalMeshHE(bool flag);
	/// <summary>Initialize VBO for a sphere which represents the bounding sphere of this mesh</summary>
	/// <returns>void</returns>
	void initWireframeBoundingSphere();
	/// <summary>Deallocate resources used by wireframe bounding sphere</summary>
	/// <returns>void</returns>
	void releaseWireframeBoundingSphere();
	/// <summary>Initialize VBO for the original mesh</summary>
	/// <returns>void</returns>
	void initWireframeOriginalMesh();
	/// <summary>Deallocate resources used by wireframe original mesh</summary>
	/// <returns>void</returns>
	void releaseWireframeOriginalMesh();


	/// <summary>Subdivides the mesh, calculats the new normals and bounding sphere. Update the lists for rendering</summary>
	/// <returns>void</returns>
	void subdivide();
	/// <summary>Saves the mesh as obj file to computer</summary>
	/// <param name="renderWindow">The GUI QT widget which called the function</param>
	/// <returns>void</returns>
	void saveMeshToFile(QWidget* renderWindow);

private:
	//Handles for shader uniforms
	GLuint m_shaderModeLocation; //Uniform to select a shader branch from the uber-shader
	GLuint m_skyBoxSamplerLocation;
	GLuint m_textureSamplerLocation;
	GLuint m_ambientMaterialLocation;
	GLuint m_specularMaterialLocation;
	GLuint m_shininessLocation;

	GLuint m_mvpLocation;
	GLuint m_modelLocation;
	GLuint m_viewLocation;
	GLuint m_modelViewLocation;

	//Subdivision Timer
	QTime m_subdivisionTimer; //Timer to check how long it takes to subdivide

	//vertices, uvs, normals, indices for original mesh
	std::vector<Vector3> m_verticesOriginal;
	std::vector<Vector2> m_uvsOriginal;
	std::vector<Vector3> m_normalsOriginal;
	std::vector<unsigned int> m_indicesOriginal;

	//VAO
	GLuint m_originalVAO;
	//VBO Vertices
	GLuint m_originalVerticesVBO;
	//EBO Indices
	GLuint m_originalIndicesEBO;

	//Vertices for wireframe bounding sphere
	std::vector<Vector3> m_wireframeBvVertices;

	//VAO bounding sphere
	GLuint m_wireframeBvVAO;
	//VBO Vertices bounding sphere
	GLuint m_wireframeBvVBO;

	//Half-Edge mesh data structure
	std::vector<Vertex*> m_vertexData;
	std::vector<HalfEdge*> m_halfEdges;
	std::vector<Face*> m_faces;

	//The lists to send to VRAM which are made from half-edge mesh. Updated each subdivision
	std::vector<Vector3> m_verticesHE;
	std::vector<Vector2> m_uvsHE;
	std::vector<Vector3> m_normalsHE;
	std::vector<unsigned int> m_indicesHE;

	//VAO
	GLuint m_halfEdgeVAO;
	//VBO Vertices
	GLuint m_halfEdgeVerticesVBO;
	//VBO UVS
	GLuint m_halfEdgeUvsVBO;
	//VBO Normals
	GLuint m_halfEdgeNormalsVBO;
	//EBO Indices
	GLuint m_halfEdgeIndicesEBO;


	//Used to call native openGL functions
	QOpenGLFunctions_3_3_Core* m_glFunctions;

	//Holds the texture that should be used by this mesh
	GLuint m_textureID;

	//Holds the shader program that should be used by this mesh
	GLuint m_shaderProgram;

	//Center Point of bounding sphere in model space
	Vector3 m_centerPointBV;
	//Radius of bounding sphere
	float m_radiusBV;

	//Flags for rendering different features
	bool m_isWireframe;
	bool m_isPlayer;
	bool m_isFrustumCulling;
	bool m_isWireframeBV;
	bool m_isSkybox;
	bool m_isWireFrameOriginalMesh;

	//Material Properties
	Vector3 m_ambientMaterial;
	Vector3 m_specularMaterial;
	float m_shininess;


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
	void indexVBO(
		std::vector<Vector3>& in_vertices, std::vector<Vector2>& in_uvs, std::vector<Vector3>& in_normals,
		std::vector<Vector3>& out_vertices, std::vector<Vector2>& out_uvs, std::vector<Vector3>& out_normals, std::vector<unsigned int>& out_indices);
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
	/// <summary>Draws the wireframe original mesh</summary>
	/// <param name="model">model matrix</param>
	/// <param name="view">view matrix</param>
	/// <param name="projection">projection matrix</param>
	/// <returns>void</returns>
	void drawWireframeOriginalMesh(const Matrix44& model, const Matrix44& view, const Matrix44& projection);
	
	/// <summary>Calculates new position for existing vertices and save it in the newPos field of the vertex</summary>
	/// <returns>void</returns>
	void calculateOldVerticesPosition();
	/// <summary>Create a new vertex for each half-edge and calculates its position Saves it in a field for half edge</summary>
	/// <returns>void</returns>
	void calculateMidpointPosition();
	/// <summary>Split halfedges into two. The existing is updated and a new halfedge is created</summary>
	/// <returns>void</returns>
	void splitHalfEdges();
	/// <summary>Updates all vertices position by setting the position = new position</summary>
	/// <returns>void</returns>
	void updateVertexPositions();
	/// <summary>Create new faces and inner edges and connect everything</summary>
	/// <returns>void</returns>
	void updateConnectivity();
	/// <summary>Update lists for rendering and vbos</summary>
	/// <returns>void</returns>
	void updateHalfEdgeMesh();
	/// <summary>Calculates normals for mesh</summary>
	/// <returns>void</returns>
	void calculateNormals();
};
#endif // HalfEdgeMesh_h__