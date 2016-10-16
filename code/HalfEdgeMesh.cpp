#include "HalfEdgeMesh.h"

HalfEdgeMesh::HalfEdgeMesh(QOpenGLFunctions_3_3_Core* functions)
{
	m_glFunctions = functions;
	m_textureID = NULL;

	//Set default Material properties for mesh
	m_ambientMaterial.Insert(0.2, 0.2, 0.2);
	m_specularMaterial.Insert(0.5, 0.5, 0.5);
	m_shininess = 5;

	m_isPlayer = false;
	m_isWireframe = false;

	m_isFrustumCulling = true;
	m_isWireframeBV = false;
	m_isSkybox = false;
	m_isWireFrameOriginalMesh = false;

	m_radiusBV = 0;
}

HalfEdgeMesh::~HalfEdgeMesh(void)
{
	if (!m_vertexData.empty()){
		for (int i = 0; i < m_vertexData.size(); i++){
			delete m_vertexData[i];
		}
	}
	if (!m_halfEdges.empty()){
		for (int i = 0; i < m_halfEdges.size(); i++){
			delete m_halfEdges[i];
		}
	}
	if (!m_faces.empty()){
		for (int i = 0; i < m_faces.size(); i++){
			delete m_faces[i];
		}
	}

	releaseWireframeBoundingSphere();
 	releaseWireframeOriginalMesh();

	//Delete VBOs
	m_glFunctions->glDeleteBuffers(1, &m_halfEdgeVerticesVBO);
	m_glFunctions->glDeleteBuffers(1, &m_halfEdgeUvsVBO);
	m_glFunctions->glDeleteBuffers(1, &m_halfEdgeNormalsVBO);
	m_glFunctions->glDeleteBuffers(1, &m_halfEdgeIndicesEBO);

	//Delete VAO
	m_glFunctions->glDeleteVertexArrays(1, &m_halfEdgeVAO);
	m_glFunctions = NULL;

	//Clear children list
	m_children.clear();
}

void HalfEdgeMesh::update(Matrix44& view, const Matrix44& model)
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

void HalfEdgeMesh::draw(ViewFrustumCheck& frustumCheck, const Matrix44& projection, const Matrix44& view, int shaderBranch, const Matrix44& lightView, const Matrix44& model, float bvScaleFactor)
{
	bool isInsideFrustum;
	Vector3 worldSpaceCenterPointBV;
	float scaledRadius;

	//Only do frustum check if frustum culling is enabled and the object is not the skybox. Otherwise just put the [isInsideFrustum] flag to true
	if (m_isFrustumCulling && !m_isSkybox){

		//Scale the bounding sphere's radius with the scale of the transform
		scaledRadius = m_radiusBV * bvScaleFactor;

		//Convert to vec4
		Vector4 centerPointBV_VEC4(m_centerPointBV[0], m_centerPointBV[1], m_centerPointBV[2], 1);
		//World Space
		Vector4 worldSpaceCenterPointBV_VEC4 = model * centerPointBV_VEC4;
		worldSpaceCenterPointBV.Insert(worldSpaceCenterPointBV_VEC4[0], worldSpaceCenterPointBV_VEC4[1], worldSpaceCenterPointBV_VEC4[2]);

		//Test sphere to plane intersection with worldspace centerpoint of bounding sphere and the scaled radius
		isInsideFrustum = frustumCheck.bSphereInFrustum(worldSpaceCenterPointBV, scaledRadius);
		if (!isInsideFrustum && shaderBranch != -1){
			frustumCheck.shapesRendered--;
		}
	}
	else{
		isInsideFrustum = true;
	}
	if (isInsideFrustum){
		//Wireframe mode. Dont render in wireframe if shaderBranch is -1.
		//(To prevent wireframe rendering in multipass rendering)
		if (m_isWireframe && !m_isSkybox && shaderBranch != -1){
			m_glFunctions->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			//Render with wireframe shader branch
			m_glFunctions->glUniform1i(m_shaderModeLocation, 12);
		}
		//Standard non wireframe mode
		else if (!m_isWireframe){
			m_glFunctions->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			//Skybox
			if (m_isSkybox){
				//Cubemap for skybox
				m_glFunctions->glActiveTexture(GL_TEXTURE0);
				m_glFunctions->glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
				m_glFunctions->glUniform1i(m_skyBoxSamplerLocation, 0);

				//We are inside the skybox so we cull the front face instead
				m_glFunctions->glCullFace(GL_FRONT);
				//Too make the skybox a part of the scene we need to change the depth comparison to LESS or Equal.
				//Because we set the z value in shader to w to make sure it will be 1 after perspective division
				m_glFunctions->glDepthFunc(GL_LEQUAL);
			}
			else{
				//Texture
				m_glFunctions->glActiveTexture(GL_TEXTURE0);
				m_glFunctions->glBindTexture(GL_TEXTURE_2D, m_textureID);
				m_glFunctions->glUniform1i(m_textureSamplerLocation, 0);
			}
		}

		//Send the material properties to shader program
		m_glFunctions->glUniform3fv(m_ambientMaterialLocation, 1, &m_ambientMaterial[0]);
		m_glFunctions->glUniform3fv(m_specularMaterialLocation, 1, &m_specularMaterial[0]);
		m_glFunctions->glUniform1f(m_shininessLocation, m_shininess);

		//Form the mvp matrix from matrices supplies with draw function
		Matrix44 mvp = projection * view * model;

		Matrix44 modelMatrix = model;
		Matrix44 viewMatrix = view;
		Matrix44 modelViewMatrix = view * model;

		//Send matrices to shader uniforms
		m_glFunctions->glUniformMatrix4fv(m_modelLocation, 1, GL_TRUE, &modelMatrix[0][0]);
		m_glFunctions->glUniformMatrix4fv(m_viewLocation, 1, GL_TRUE, &viewMatrix[0][0]);
		m_glFunctions->glUniformMatrix4fv(m_modelViewLocation, 1, GL_TRUE, &modelViewMatrix[0][0]);
		m_glFunctions->glUniformMatrix4fv(m_mvpLocation, 1, GL_TRUE, &mvp[0][0]);

		//Bind this mesh VAO
		m_glFunctions->glBindVertexArray(m_halfEdgeVAO);
		//Draw the triangles using the index buffer(EBO)
		m_glFunctions->glDrawElements(GL_TRIANGLES, m_indicesHE.size(), GL_UNSIGNED_INT, 0);

		if (m_isSkybox){
			//Unbind texture
			m_glFunctions->glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			m_glFunctions->glCullFace(GL_BACK);
			m_glFunctions->glDepthFunc(GL_LESS);
		}
		else{
			//Unbind texture
			m_glFunctions->glBindTexture(GL_TEXTURE_2D, 0);
		}

		//Unbind the VAO
		m_glFunctions->glBindVertexArray(0);

		if (m_isWireFrameOriginalMesh){
			//Draw wireframe original mesh
			drawWireframeOriginalMesh(model, view, projection);
			//Revert back to shader branch previously used before rendering the wireframe
			m_glFunctions->glUniform1i(m_shaderModeLocation, shaderBranch);
		}
		//Draw wireframe BV sphere
		if (!m_isPlayer && !m_isWireframe && m_isWireframeBV && m_isFrustumCulling && !m_isSkybox && shaderBranch != -1){
			drawWireframeBoundingSphere(view, projection, worldSpaceCenterPointBV, scaledRadius);
			//Revert back to shader branch previously used before rendering the wireframe
			m_glFunctions->glUniform1i(m_shaderModeLocation, shaderBranch);
		}
	}

	//Calls the childrens' update
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

void HalfEdgeMesh::useShaderProgram(GLuint shaderProgram)
{
	m_shaderProgram = shaderProgram;

	//Get handles for shader uniforms
	m_shaderModeLocation = m_glFunctions->glGetUniformLocation(m_shaderProgram, "mode");
	m_skyBoxSamplerLocation = m_glFunctions->glGetUniformLocation(m_shaderProgram, "skyBoxSampler");
	m_textureSamplerLocation = m_glFunctions->glGetUniformLocation(m_shaderProgram, "textureSampler");
	m_ambientMaterialLocation = m_glFunctions->glGetUniformLocation(m_shaderProgram, "AmbientMaterial");
	m_specularMaterialLocation = m_glFunctions->glGetUniformLocation(m_shaderProgram, "SpecularMaterial");
	m_shininessLocation = m_glFunctions->glGetUniformLocation(m_shaderProgram, "Shininess");
	m_mvpLocation = m_glFunctions->glGetUniformLocation(m_shaderProgram, "mvp");
	m_modelLocation = m_glFunctions->glGetUniformLocation(m_shaderProgram, "model");
	m_viewLocation = m_glFunctions->glGetUniformLocation(m_shaderProgram, "view");
	m_modelViewLocation = m_glFunctions->glGetUniformLocation(m_shaderProgram, "modelView");
}

void HalfEdgeMesh::useTexture(GLuint textureID)
{
	m_textureID = textureID;
}

void HalfEdgeMesh::loadOBJ(const char* path)
{
	//Lists to store the data read from obj file
	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<Vector3> temp_vertices;
	std::vector<Vector2> temp_uvs;
	std::vector<Vector3> temp_normals;

	//Lists with sorted data according the the indices specified in obj file
	std::vector<Vector3> sorted_vertices;
	std::vector<Vector2> sorted_uvs;
	std::vector<Vector3> sorted_normals;
	
	//Open the file in read only mode
	FILE* file = fopen(path, "r");
	if (file == NULL){
	 	printf("Impossible to open the file !\n");
	 	return;
	}
	 
	//Handle the data in the file
	while (true){
	 	//Read the first word of the line and store it in lineHeader
	 	char lineHeader[128];
	 	//Read the first word of the line as a string and store it in lineHeader
	 	int res = fscanf(file, "%s", lineHeader);
	 
	 	//If it's the end of the file then break the loop
	 	if (res == EOF){
	 		break; // EOF = End Of File. Quit the loop.
	 	}
	 
	 	//Read the stored data and check if it's a "v"
	 	if (strcmp(lineHeader, "v") == 0){ //Returns 0 if the strings are equal
	 		Vector3 vertex;
	 		//Read the data as floats and put them in the vector
			fscanf(file, "%f %f %f\n", &vertex[0], &vertex[1], &vertex[2]);
	 		temp_vertices.push_back(vertex);
	 	}
	 	//Read the stored data and check if it's a "vt"
	 	else if (strcmp(lineHeader, "vt") == 0){ //Returns 0 if the strings are equal
	 		Vector2 uv;
	 		//Read the data as floats and put them in the vector
	 		fscanf(file, "%f %f\n", &uv[0], &uv[1]);
	 		//uv[1] = -uv[1]; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
	 		temp_uvs.push_back(uv);
	 	}
	 	//Read the stored data and check if it's a "vn"
	 	else if (strcmp(lineHeader, "vn") == 0){ //Returns 0 if the strings are equal
	 		Vector3 normal;
	 		//Read the data as floats and put them in the vector
	 		fscanf(file, "%f %f %f\n", &normal[0], &normal[1], &normal[2]);
	 		temp_normals.push_back(normal);
	 	}
	 	//Read the stored data and check if it's a "f"
	 	else if (strcmp(lineHeader, "f") == 0){ //Returns 0 if the strings are equal
	 		unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
	 		//Read the data as integers and put them in the arrays
	 		int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
	 		//If the indices in the obj files are not 9 then we can't handle it
	 		if (matches != 9){
	 			printf("File can't be read by our simple parser :-( Try exporting with other options\n");
	 			fclose(file);
	 			return;
	 		}
	 
	 		//Add the indices of vertex data to their respective lists
	 		vertexIndices.push_back(vertexIndex[0]);
	 		vertexIndices.push_back(vertexIndex[1]);
	 		vertexIndices.push_back(vertexIndex[2]);
	 		uvIndices.push_back(uvIndex[0]);
	 		uvIndices.push_back(uvIndex[1]);
	 		uvIndices.push_back(uvIndex[2]);
	 		normalIndices.push_back(normalIndex[0]);
	 		normalIndices.push_back(normalIndex[1]);
	 		normalIndices.push_back(normalIndex[2]);
	 	}
	 	else{
	 		//The data we don't need is put into this buffer just so we can move the reader forward
	 		//fgets reads the whole line. We don't want to loop multiple times if we already know the data is not something we need
	 		char stupidBuffer[1000];
	 		fgets(stupidBuffer, 1000, file);
	 	}
	}
	//Closes the file now that we are done with it
	fclose(file);
	 
	//The obj file indices specifies which 3 vertices are needed to form a triangle
	//This loop will create lists of sorted vertex data according to the obj file
	for (int i = 0; i < vertexIndices.size(); i++){
	 
	 	//Get the indices.
	 	unsigned int vertexIndex = vertexIndices[i];
	 	unsigned int uvIndex = uvIndices[i];
	 	unsigned int normalIndex = normalIndices[i];
	 
	 	//Get the vertex data with the index extracted from the obj file
	 	//Indices start at 1 in obj files but in programming it's off by 1
	 	Vector3 vertex = temp_vertices[vertexIndex - 1];
	 	Vector2 uv = temp_uvs[uvIndex - 1];
	 	Vector3 normal = temp_normals[normalIndex - 1];
	 
	 	//Put the vertex data in their respective lists
	 	//Will now be sorted according to how the obj file indices specifies
		sorted_vertices.push_back(vertex);
		sorted_uvs.push_back(uv);
		sorted_normals.push_back(normal);
	}
	//Vertex indexing. The lists in the class will store the indexed vertex data. Which will not have duplicates.
	//Newly generated indices list will tell which vertices are needed to form a triangle
	indexVBO(sorted_vertices, sorted_uvs, sorted_normals, m_verticesOriginal, m_uvsOriginal, m_normalsOriginal, m_indicesOriginal);



	/*
		Put all of the data in Vertex* List with no duplicates.
		Create the half-edge mesh here. Use indices list 3x for a face. Link the half edges
		Find twin edges.

		Iterate faces and generate the lists for VRAM.
		Run indexVBO
		create vbos

		Those lists will be updated when we subdivide. Subdivision will update the pointers and then vertex data values.
		Iterate the faces to generate the lists for VRAM after.
	
	*/

	//Used to check if there is a Twin half-edge
	std::map<std::pair<const Vertex*, const Vertex*>, HalfEdge*> edgeMap;

	//Load the vertex list used by half edge mesh with vertex data from the original vertex list. This list does not have duplicate vertices and is dependant on the indices list.
	for (int i = 0; i < temp_vertices.size(); i++){
		Vertex* vertexData = new Vertex;
		vertexData->pos = temp_vertices[i];
		m_vertexData.push_back(vertexData);
	}


	//Create the faces and half-edges. Getting the right index for the data using the indices list.
	for (int i = 0; i < vertexIndices.size(); i += 3){
		Face* f = new Face;
		HalfEdge* he1 = new HalfEdge;
		HalfEdge* he2 = new HalfEdge;
		HalfEdge* he3 = new HalfEdge;

		//Associate the half-edges to a face
		he1->face = f;
		he2->face = f;
		he3->face = f;

		//Get the UVs and Normals for vertices(Overwriting an UV if it's the same vertex)
		m_vertexData[vertexIndices[i] - 1]->uv = temp_uvs[uvIndices[i] - 1];
		m_vertexData[vertexIndices[i] - 1]->normal = temp_normals[normalIndices[i] - 1];

		m_vertexData[vertexIndices[i + 1] - 1]->uv = temp_uvs[uvIndices[i + 1] - 1];
		m_vertexData[vertexIndices[i + 1] - 1]->normal = temp_normals[normalIndices[i + 1] - 1];

		m_vertexData[vertexIndices[i + 2] - 1]->uv = temp_uvs[uvIndices[i + 2] - 1];
		m_vertexData[vertexIndices[i + 2] - 1]->normal = temp_normals[normalIndices[i + 2] - 1];


		//Point origin vertex for half edges to right vertex according to indices list
		he1->origin = m_vertexData[vertexIndices[i]-1];
		he2->origin = m_vertexData[vertexIndices[i + 1]-1];
		he3->origin = m_vertexData[vertexIndices[i + 2]-1];

		//Assign half-edge for vertex which has the vertex as origin
		he1->origin->edge = he1;
		he2->origin->edge = he2;
		he3->origin->edge = he3;

		//Assign next edge pointers
		he1->next = he2;
		he2->next = he3;
		he3->next = he1;

		//Assign an half-edge to face
		f->edge = he1;

		m_halfEdges.push_back(he1);
		m_halfEdges.push_back(he2);
		m_halfEdges.push_back(he3);
		m_faces.push_back(f);

		/*
		Make 3 pairs here. To later look up in edgeMap to find the pairs.
		*/
		edgeMap[std::make_pair(he1->origin, he1->next->origin)] = he1;
		edgeMap[std::make_pair(he2->origin, he2->next->origin)] = he2;
		edgeMap[std::make_pair(he3->origin, he3->next->origin)] = he3;

	}

	//Find Pairs
	unsigned int twinCounter = 0;
	for (int i = 0; i < m_halfEdges.size(); i++){
		//Origin vertex for half-edge that we try to find a twin half-edge for
		Vertex* v1 = m_halfEdges[i]->origin;
		//End vertex for half-edge that we try to find a twin half-edge for
		Vertex* v2 = m_halfEdges[i]->next->origin;

		//Search for a pair with opposite vertices which is a pair. If found update pair pointers
		std::map<std::pair<const Vertex*, const Vertex*>, HalfEdge*>::iterator itr = edgeMap.find(std::make_pair(v2, v1));
		if (itr != edgeMap.end())
		{
			HalfEdge* twinEdge = itr->second;
			m_halfEdges[i]->pair = twinEdge;
			twinEdge->pair = m_halfEdges[i];
			twinCounter++;
		}
	}

	/*
	Quadratic method to find pairs
	*/
// 	//Quaddratic method to find the twin half edges
// 	for (int i = 0; i < m_halfEdges.size(); i++){
// 		//Origin vertex for half-edge that we try to find a twin half-edge for
// 		Vertex* v1 = m_halfEdges[i]->origin;
// 		//End vertex for half-edge that we try to find a twin half-edge for
// 		Vertex* v2 = m_halfEdges[i]->next->origin;
// 		//Iterate the list again to compare every half-edge with the one we try to find twin for
// 		for (int j = 0; j < m_halfEdges.size(); j++){
// 			//Origin vertex for compare half-edge
// 			Vertex* e1 = m_halfEdges[j]->origin;
// 			//End vertex for compare half-edge
// 			Vertex* e2 = m_halfEdges[j]->next->origin;
// 			//Opposite half-edge is a twin
// 			if (v1 == e2 && v2 == e1){
// 				m_halfEdges[i]->twin = m_halfEdges[j];
// 				m_halfEdges[j]->twin = m_halfEdges[i];
// 				twinCounter++;
// 			}
// 		}
// 	}

	qDebug() << temp_vertices.size() << "Unique Vertices" << m_halfEdges.size() << "Half-Edges" << m_faces.size() << "Faces" << twinCounter << "Twin-Edges";
	//Create lists with vertices, uvs, normals and indices which will be used in VBOs for rendering
	updateHalfEdgeMesh();

	//////////////////////////////////////////////////////////////////////////
	//Create VAO
	m_glFunctions->glGenVertexArrays(1, &m_halfEdgeVAO);
	//Bind VAO
	m_glFunctions->glBindVertexArray(m_halfEdgeVAO);

	//////////////////////////////////////////////////////////////////////////
	//Vertex VBO
	//Create VBO on the GPU to store the vertex data
	m_glFunctions->glGenBuffers(1, &m_halfEdgeVerticesVBO);
	//Bind VBO to make it current
	m_glFunctions->glBindBuffer(GL_ARRAY_BUFFER, m_halfEdgeVerticesVBO);
	//Set the usage type, allocate VRAM and send the vertex data to the GPU
	m_glFunctions->glBufferData(GL_ARRAY_BUFFER, m_verticesHE.size() * sizeof(Vector3), &m_verticesHE[0], GL_STATIC_DRAW);

	//Sets up which shader attribute will received the data. How many elements will form a vertex, type etc
	m_glFunctions->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	//Enable the shader attribute to receive data
	m_glFunctions->glEnableVertexAttribArray(0);

	//////////////////////////////////////////////////////////////////////////
	//UV VBO
	//Create VBO on the GPU to store the vertex data
	m_glFunctions->glGenBuffers(1, &m_halfEdgeUvsVBO);
	//Bind VBO to make it current
	m_glFunctions->glBindBuffer(GL_ARRAY_BUFFER, m_halfEdgeUvsVBO);
	//Set the usage type, allocate VRAM and send the vertex data to the GPU
	m_glFunctions->glBufferData(GL_ARRAY_BUFFER, m_uvsHE.size() * sizeof(Vector2), &m_uvsHE[0], GL_STATIC_DRAW);
	 
	//Sets up which shader attribute will receive the data. How many elements will form a vertex, type etc
	m_glFunctions->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	//Enable the shader attribute to receive data
	m_glFunctions->glEnableVertexAttribArray(1);

	//////////////////////////////////////////////////////////////////////////
	//Normal VBO
	//Create VBO on the GPU to store the vertex data
	m_glFunctions->glGenBuffers(1, &m_halfEdgeNormalsVBO);
	//Bind VBO to make it current
	m_glFunctions->glBindBuffer(GL_ARRAY_BUFFER, m_halfEdgeNormalsVBO);
	//Set the usage type, allocate VRAM and send the vertex data to the GPU
	m_glFunctions->glBufferData(GL_ARRAY_BUFFER, m_normalsHE.size() * sizeof(Vector3), &m_normalsHE[0], GL_STATIC_DRAW);
	 
	//Sets up which shader attribute will receive the data. How many elements will form a vertex, type etc
	m_glFunctions->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	//Enable the shader attribute to receive data
	m_glFunctions->glEnableVertexAttribArray(2);

	//////////////////////////////////////////////////////////////////////////
	//Indices EBO
	//Create EBO on the GPU to store the vertex data
	m_glFunctions->glGenBuffers(1, &m_halfEdgeIndicesEBO);
	//Bind EBO to make it current
	m_glFunctions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_halfEdgeIndicesEBO);
	//Set the usage type, allocate VRAM and send the vertex data to the GPU
	m_glFunctions->glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indicesHE.size() * sizeof(unsigned int), &m_indicesHE[0], GL_STATIC_DRAW);

	//////////////////////////////////////////////////////////////////////////
	//Unbind the VAO now that the VBOs have been set up
	m_glFunctions->glBindVertexArray(0);

	calculateBoundingSphere();
}

bool HalfEdgeMesh::getSimilarVertexIndex(PackedVertex& packed, std::map<PackedVertex, unsigned int>& VertexToOutIndex, unsigned int& result)
{
	//Iterator to find a PackedVertex in map that is equal to packed
	std::map<PackedVertex, unsigned int>::iterator it = VertexToOutIndex.find(packed);
	//Return false if nothing is found
	if (it == VertexToOutIndex.end()){
		return false;
	}
	else{
		//Returns the value the key is paired to
		result = it->second;
		return true;
	}
}

void HalfEdgeMesh::indexVBO(
	std::vector<Vector3>& in_vertices, std::vector<Vector2>& in_uvs, std::vector<Vector3>& in_normals,
	std::vector<Vector3>& out_vertices, std::vector<Vector2>& out_uvs, std::vector<Vector3>& out_normals, std::vector<unsigned int>& out_indices)
{
	//Reset lists if they have any data
	out_vertices.clear();
	out_uvs.clear();
	out_normals.clear();
	out_indices.clear();

	//Stores unique vertex data paired with the index
	std::map<PackedVertex, unsigned int> VertexToOutIndex;

	// For each input vertex
	for (int i = 0; i < in_vertices.size(); i++){

		//Create a new PacketVertex which stores all vertex data
		PackedVertex packed;
		packed.position = in_vertices[i];
		packed.uv = in_uvs[i];
		packed.normal = in_normals[i];


		//Will store the index of the vertex found in the map
		unsigned int index;
		//Try to find a PackedVertex in the VertexToOutIndex map that is the same as packed
		//If one is found save the index of it
		bool found = getSimilarVertexIndex(packed, VertexToOutIndex, index);

		//Push the found index to our indices vector
		if (found){ // A similar vertex is already in the VBO, use it instead !
			out_indices.push_back(index);
		}
		//If not found then add the vertex data to our vertex arrays.
		//Also add the the index of the newly added vertex data to indices array. Which will be the last element.
		//Add the vertex data to the map and associate it with the index.
		//So we can iterate next time and check for same vertex data and use the index
		else{ // If not, it needs to be added in the output data.
			out_vertices.push_back(in_vertices[i]);
			out_uvs.push_back(in_uvs[i]);
			out_normals.push_back(in_normals[i]);
			unsigned int newindex = (unsigned int)out_vertices.size() - 1;
			out_indices.push_back(newindex);
			VertexToOutIndex[packed] = newindex;
		}
	}
}

void HalfEdgeMesh::calculateBoundingSphere(){
	m_centerPointBV.Insert(0, 0, 0);
	m_radiusBV = 0;

	//Calculate the center point by getting the mean value of vertices
	for (int i = 0; i < m_verticesHE.size(); i++){
		m_centerPointBV += m_verticesHE[i];
	}
	m_centerPointBV /= m_verticesHE.size();

	//Find the farthest distance from the center point by iterating all vertices all calculating the distance
	for (int i = 0; i < m_verticesHE.size(); i++){
		Vector3 pos = m_verticesHE[i] - m_centerPointBV;
		float distance = pos.Magnitude();
		if (distance > m_radiusBV){
			m_radiusBV = distance;
		}
	}
}

void HalfEdgeMesh::setAmbientMaterial(float r, float g, float b)
{
	m_ambientMaterial.Insert(r, g, b);
}

void HalfEdgeMesh::setSpecularMaterial(float r, float g, float b, float shininess)
{
	m_specularMaterial.Insert(r, g, b);
	m_shininess = shininess;
}

void HalfEdgeMesh::drawWireframeBoundingSphere(const Matrix44& view, const Matrix44& projection, Vector3 position, float radius)
{
	//Sphere triangles were created clockwise so we tell opengl to treat clockwise as front face
	m_glFunctions->glFrontFace(GL_CW);
	m_glFunctions->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//Bind the shaders to be used
	m_glFunctions->glUniform1i(m_shaderModeLocation, 12);

	//Model Matrix
	Matrix44 mymodel;
	mymodel.Translate(position); //translate to centerpoint of bounding sphere
	mymodel.Scale(radius); //scale to the radius of the bounding sphere
	Matrix44 mvp = projection * view * mymodel;

	//Send the mvp matrix to the vertex shader
	m_glFunctions->glUniformMatrix4fv(m_mvpLocation, 1, GL_TRUE, &mvp[0][0]);

	//Bind this mesh VAO
	m_glFunctions->glBindVertexArray(m_wireframeBvVAO);

	//Draw the sphere as triangle strip and then tell opengl to treat triangles counter clockwise as front face
	m_glFunctions->glDrawArrays(GL_TRIANGLE_STRIP, 0, m_wireframeBvVertices.size());
	m_glFunctions->glFrontFace(GL_CCW);
	m_glFunctions->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//Unbind
	m_glFunctions->glBindTexture(GL_TEXTURE_2D, 0);
	//Unbind the VAO
	m_glFunctions->glBindVertexArray(0);
}

void HalfEdgeMesh::setNodeType(HalfEdgeMeshType node)
{
	if (node == PlayerHalfEdgeMesh){
		m_isPlayer = true;
	}
	if (node == SkyboxHalfEdgemesh){
		m_isSkybox = true;
	}
}

void HalfEdgeMesh::setWireframeMode(bool flag)
{
	m_isWireframe = flag;
}

void HalfEdgeMesh::setFrustumCulling(bool flag)
{
	m_isFrustumCulling = flag;
}

void HalfEdgeMesh::setWireframeBV(bool flag)
{
	m_isWireframeBV = flag;
}

void HalfEdgeMesh::setWireframeOriginalMeshHE(bool flag)
{
	m_isWireFrameOriginalMesh = flag;
}

void HalfEdgeMesh::initWireframeBoundingSphere()
{
	const float degreeIncrement = 20;

	//Holds a vertex
	Vector3 sphereVertex3f;

	float radius = 1.0f;

	//Iterate 180 Degrees downwards and then take next step sideways until we have iterated around to 360 degrees
	for (float theta = 0; theta < 180; theta += degreeIncrement) // Iterate through height of sphere
	{
		for (float phi = 0; phi < 360; phi += degreeIncrement) // Iterate around the sphere
		{
			sphereVertex3f[0] = radius * sinf((phi)* MyPersonalMathLibraryConstants::PI / 180.0) * sinf((theta)* MyPersonalMathLibraryConstants::PI / 180.0);
			sphereVertex3f[1] = radius * cosf((phi)* MyPersonalMathLibraryConstants::PI / 180.0) * sinf((theta)* MyPersonalMathLibraryConstants::PI / 180.0);
			sphereVertex3f[2] = radius * cosf((theta)* MyPersonalMathLibraryConstants::PI / 180.0);

			m_wireframeBvVertices.push_back(sphereVertex3f);


			sphereVertex3f[0] = radius * sinf((phi)* MyPersonalMathLibraryConstants::PI / 180.0) * sinf((theta + degreeIncrement) * MyPersonalMathLibraryConstants::PI / 180.0);
			sphereVertex3f[1] = radius * cosf((phi)* MyPersonalMathLibraryConstants::PI / 180.0) * sinf((theta + degreeIncrement) * MyPersonalMathLibraryConstants::PI / 180.0);
			sphereVertex3f[2] = radius * cosf((theta + degreeIncrement) * MyPersonalMathLibraryConstants::PI / 180.0);

			m_wireframeBvVertices.push_back(sphereVertex3f);

			sphereVertex3f[0] = radius * sinf((phi + degreeIncrement) * MyPersonalMathLibraryConstants::PI / 180.0) * sinf((theta)* MyPersonalMathLibraryConstants::PI / 180.0);
			sphereVertex3f[1] = radius * cosf((phi + degreeIncrement) * MyPersonalMathLibraryConstants::PI / 180.0) * sinf((theta)* MyPersonalMathLibraryConstants::PI / 180.0);
			sphereVertex3f[2] = radius * cosf((theta)* MyPersonalMathLibraryConstants::PI / 180.0);

			m_wireframeBvVertices.push_back(sphereVertex3f);

			sphereVertex3f[0] = radius * sinf((phi + degreeIncrement) * MyPersonalMathLibraryConstants::PI / 180.0) * sinf((theta + degreeIncrement) * MyPersonalMathLibraryConstants::PI / 180.0);
			sphereVertex3f[1] = radius * cosf((phi + degreeIncrement) * MyPersonalMathLibraryConstants::PI / 180.0) * sinf((theta + degreeIncrement) * MyPersonalMathLibraryConstants::PI / 180.0);
			sphereVertex3f[2] = radius * cosf((theta + degreeIncrement) * MyPersonalMathLibraryConstants::PI / 180.0);

			m_wireframeBvVertices.push_back(sphereVertex3f);
		}
	}

	//////////////////////////////////////////////////////////////////////////

	//Create VAO
	m_glFunctions->glGenVertexArrays(1, &m_wireframeBvVAO);
	//Bind VAO
	m_glFunctions->glBindVertexArray(m_wireframeBvVAO);

	//////////////////////////////////////////////////////////////////////////
	//Vertex VBO
	//Create VBO on the GPU to store the vertex data
	m_glFunctions->glGenBuffers(1, &m_wireframeBvVBO);
	//Bind VBO to make it current
	m_glFunctions->glBindBuffer(GL_ARRAY_BUFFER, m_wireframeBvVBO);
	//Set the usage type, allocate VRAM and send the vertex data to the GPU
	m_glFunctions->glBufferData(GL_ARRAY_BUFFER, m_wireframeBvVertices.size() * sizeof(Vector3), &m_wireframeBvVertices[0], GL_STATIC_DRAW);

	//Sets up which shader attribute will received the data. How many elements will form a vertex, type etc
	m_glFunctions->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	//Enable the shader attribute to receive data
	m_glFunctions->glEnableVertexAttribArray(0);

	//////////////////////////////////////////////////////////////////////////
	//Unbind the VAO now that the VBOs have been set up
	m_glFunctions->glBindVertexArray(0);
}

void HalfEdgeMesh::releaseWireframeBoundingSphere()
{
	//Delete VBOs
	m_glFunctions->glDeleteBuffers(1, &m_wireframeBvVBO);

	m_glFunctions->glDeleteBuffers(1, &m_wireframeBvVAO);
	m_wireframeBvVertices.clear();
}

void HalfEdgeMesh::subdivide()
{
	m_subdivisionTimer.start();

	//Calculate new position for existing vertices
	calculateOldVerticesPosition();
	//Create a new vertex for each halfedge and calculate its position
	calculateMidpointPosition();
	//Split each halfedge into two by updating the existing halfedge's pointer and creating a new one. Linking them together
	splitHalfEdges();
	//Create the inner edges and the new faces. And linking them all together
	updateConnectivity();
	//Sets all vertices position = new postion
	updateVertexPositions();
	//Calculate normals for mesh
	calculateNormals();

	//Update lists for rendering and vbos
	updateHalfEdgeMesh();
	//Update the bounding sphere
	calculateBoundingSphere();

	//Bind VBO to make it current
	m_glFunctions->glBindBuffer(GL_ARRAY_BUFFER, m_halfEdgeVerticesVBO);
	//Set the usage type, allocate VRAM and send the vertex data to the GPU
	m_glFunctions->glBufferData(GL_ARRAY_BUFFER, m_verticesHE.size() * sizeof(Vector3), &m_verticesHE[0], GL_STATIC_DRAW);

	//////////////////////////////////////////////////////////////////////////
	//UV VBO
	//Bind VBO to make it current
	m_glFunctions->glBindBuffer(GL_ARRAY_BUFFER, m_halfEdgeUvsVBO);
	//Set the usage type, allocate VRAM and send the vertex data to the GPU
	m_glFunctions->glBufferData(GL_ARRAY_BUFFER, m_uvsHE.size() * sizeof(Vector2), &m_uvsHE[0], GL_STATIC_DRAW);

	//////////////////////////////////////////////////////////////////////////
	//Normal VBO
	//Bind VBO to make it current
	m_glFunctions->glBindBuffer(GL_ARRAY_BUFFER, m_halfEdgeNormalsVBO);
	//Set the usage type, allocate VRAM and send the vertex data to the GPU
	m_glFunctions->glBufferData(GL_ARRAY_BUFFER, m_normalsHE.size() * sizeof(Vector3), &m_normalsHE[0], GL_STATIC_DRAW);

	//////////////////////////////////////////////////////////////////////////
	//Indices EBO
	//Bind EBO to make it current
	m_glFunctions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_halfEdgeIndicesEBO);
	//Set the usage type, allocate VRAM and send the vertex data to the GPU
	m_glFunctions->glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indicesHE.size() * sizeof(unsigned int), &m_indicesHE[0], GL_STATIC_DRAW);

	qDebug() << m_subdivisionTimer.elapsed() << "ms";
}

void HalfEdgeMesh::calculateOldVerticesPosition()
{
	for (int i = 0; i < m_vertexData.size(); i++){
		//Get the vertex to which we will calculate its new position
		Vertex* vertex = m_vertexData[i];
		//Get the half-edge of the vertex we are working on
		HalfEdge* halfedge = m_vertexData[i]->edge;

		//All neighbors added together
		Vector3 pSum;
		//Amount of neighbors around the vertex
		float n = 0;

		//Traverser, traversing all neighbors of the vertex
		HalfEdge* traverser = halfedge->next;
		
		//Do until halfedge is same as start halfedge
		do 
		{
			//Handle current neighbor vertex
			pSum += traverser->origin->pos;
			n++;

			//Update traverse pointer to next neighbor
			traverser = traverser->next->pair->next;
		} while (traverser != halfedge->next);

		//Calculate b which is a function used by the equation which calculates the new position
		//Using  Warren and Weimer's equation which doesnt have expensive trigonometrix functions
		float b = 3.0 / (n*(n + 2.0));

		vertex->newPos = (1.0 - n*b)*vertex->pos + b*pSum;
	}
}

void HalfEdgeMesh::calculateMidpointPosition()
{
	/*
	Iterate edges calculate the midpoint vertex
	*/

	for (int i = 0; i < m_halfEdges.size(); i++){
		//Get the halfedge to which we will calculate midpoint position of the new vertex
		HalfEdge* halfedge = m_halfEdges[i];

		//We want to point the pair's midpoint to the same vertex, aka no duplicates.
		//So for each iteration we point the pair's midpoint pointer as well.
		//If later in the loop we come across the halfedge which was the pair the midpoint won't be NULL
		if (halfedge->midpoint == NULL){
			Vertex* p1 = halfedge->origin;
			Vertex* p2 = halfedge->next->origin;
			Vertex* p3 = halfedge->next->next->origin;
			Vertex* p4 = halfedge->pair->next->next->origin;

			Vector3 pAll = 3 * p1->pos + 3 * p2->pos + p3->pos + p4->pos;


			Vertex* midpoint = new Vertex;
			Vector3 calculatedPosition = pAll / 8.0;
			midpoint->pos = calculatedPosition;
			//The uv is an interpolation between two existing ones
			midpoint->uv = (p1->uv + p2->uv) / 2.0;
			midpoint->newPos = calculatedPosition;
			halfedge->midpoint = midpoint;
			halfedge->pair->midpoint = midpoint;


			m_vertexData.push_back(midpoint);
		}
	}
}

void HalfEdgeMesh::splitHalfEdges()
{
	//Split edges and link twins
	unsigned int halfEdgeListSize = m_halfEdges.size();
	//Split halfedges into two by iterating the halfedges and split also linking their pairs pointers
	for (int i = 0; i < halfEdgeListSize; i++){
		//Checks if the half edge has already been split
		if (m_halfEdges[i]->next->origin != m_halfEdges[i]->midpoint){
			//Create new half-edge
			HalfEdge* newHalfEdge1 = new HalfEdge;
			//New halfedge starts at the midpoint of the existing half edge
			newHalfEdge1->origin = m_halfEdges[i]->midpoint;
			//Update the vertex edge pointer
			newHalfEdge1->origin->edge = newHalfEdge1;
			//Connect the next pointer to the existing halfedge's next
			newHalfEdge1->next = m_halfEdges[i]->next;
			//Associate with the existing halfedge's face
			newHalfEdge1->face = m_halfEdges[i]->face;

			//Create new half-edge for the pair of the halfedge we are currently in progress of splitting
			HalfEdge* newHalfEdge2 = new HalfEdge;
			//New halfedge starts at the midpoint of the existing half edge's pair
			newHalfEdge2->origin = m_halfEdges[i]->pair->midpoint;
			//Update the vertex edge pointer
			newHalfEdge2->origin->edge = newHalfEdge2;
			//Connect the next pointer to the existing halfedge's pair's next
			newHalfEdge2->next = m_halfEdges[i]->pair->next;
			//Associate with the existing halfedge's pair's face
			newHalfEdge2->face = m_halfEdges[i]->pair->face;


			//We can now update the existing halfedges next and pair pointers
			m_halfEdges[i]->next = newHalfEdge1;
			m_halfEdges[i]->pair->next = newHalfEdge2;

			m_halfEdges[i]->pair->pair = newHalfEdge1;
			newHalfEdge1->pair = m_halfEdges[i]->pair;

			m_halfEdges[i]->pair = newHalfEdge2;
			newHalfEdge2->pair = m_halfEdges[i];


			m_halfEdges.push_back(newHalfEdge1);
			m_halfEdges.push_back(newHalfEdge2);
		}
	}
}

void HalfEdgeMesh::updateVertexPositions()
{
	for (int i = 0; i < m_vertexData.size(); i++){
		m_vertexData[i]->pos = m_vertexData[i]->newPos;
	}
}

void HalfEdgeMesh::updateConnectivity()
{
	unsigned int faceListSize = m_faces.size();
	for (int i = 0; i < faceListSize; i++){

		Face* leftFace = m_faces[i];
		Face* topFace = new Face;
		Face* rightFace = new Face;
		Face* middleFace = new Face;

		HalfEdge* innerLeftEdge = new HalfEdge;
		HalfEdge* innerTopEdge = new HalfEdge;
		HalfEdge* innerRightEdge = new HalfEdge;

		//New triangle, middle
		HalfEdge* innerMidEdge1 = new HalfEdge;
		HalfEdge* innerMidEdge2 = new HalfEdge;
		HalfEdge* innerMidEdge3 = new HalfEdge;

		//Saves the existing outer edges before we update their pointers
		HalfEdge* oldLeftEdge1 = m_faces[i]->edge;
		HalfEdge* oldTopEdge1 = m_faces[i]->edge->next;
		HalfEdge* oldTopEdge2 = m_faces[i]->edge->next->next;
		HalfEdge* oldRightEdge1 = m_faces[i]->edge->next->next->next;
		HalfEdge* oldRightEdge2 = m_faces[i]->edge->next->next->next->next;
		HalfEdge* oldLeftEdge2 = m_faces[i]->edge->next->next->next->next->next;


		/*
			Gets the right vertex as start point for the the new inner edges and link them to the right other edges and update their pair pointer.
			Update the outer edges next pointers and associate a face to an edge which forms a triangle
		*/
		//Connectivity for inner edges
		//Bottom left face
		innerLeftEdge->origin = oldTopEdge1->origin;  //vertex to vertex
		innerLeftEdge->origin->edge = innerLeftEdge; //associate halfedge to vertex

		//connectivity
		innerLeftEdge->next = oldLeftEdge2;
		oldLeftEdge1->next = innerLeftEdge;

		innerLeftEdge->face = leftFace;

		innerLeftEdge->pair = innerMidEdge2;
		innerMidEdge2->pair = innerLeftEdge;

		//Top face
		topFace->edge = oldTopEdge1;
		innerTopEdge->origin = oldRightEdge1->origin;
		innerTopEdge->origin->edge = innerTopEdge;

		//connectivity
		innerTopEdge->next = oldTopEdge1;
		oldTopEdge2->next = innerTopEdge;

		oldTopEdge1->face = topFace;
		oldTopEdge2->face = topFace;
		innerTopEdge->face = topFace;

		innerTopEdge->pair = innerMidEdge3;
		innerMidEdge3->pair = innerTopEdge;

		//Bottom right face
		rightFace->edge = innerRightEdge;
		innerRightEdge->origin = oldLeftEdge2->origin;
		innerRightEdge->origin->edge = innerRightEdge;

		//inner->oldright1->oldright2->inner
		innerRightEdge->next = oldRightEdge1;
		oldRightEdge2->next = innerRightEdge;

		oldRightEdge1->face = rightFace;
		oldRightEdge2->face = rightFace;
		innerRightEdge->face = rightFace;

		innerRightEdge->pair = innerMidEdge1;
		innerMidEdge1->pair = innerRightEdge;

		//Middle face
		middleFace->edge = innerMidEdge1;
		innerMidEdge1->origin = oldRightEdge1->origin;
		innerMidEdge1->origin->edge = innerMidEdge1;
		innerMidEdge1->next = innerMidEdge2;

		innerMidEdge2->origin = oldLeftEdge2->origin;
		innerMidEdge2->origin->edge = innerMidEdge2;
		innerMidEdge2->next = innerMidEdge3;

		innerMidEdge3->origin = oldTopEdge1->origin;
		innerMidEdge3->origin->edge = innerMidEdge3;
		innerMidEdge3->next = innerMidEdge1;

		innerMidEdge1->face = middleFace;
		innerMidEdge2->face = middleFace;
		innerMidEdge3->face = middleFace;



		m_faces.push_back(topFace);
		m_faces.push_back(rightFace);
		m_faces.push_back(middleFace);

		m_halfEdges.push_back(innerLeftEdge);
		m_halfEdges.push_back(innerTopEdge);
		m_halfEdges.push_back(innerRightEdge);
		m_halfEdges.push_back(innerMidEdge1);
		m_halfEdges.push_back(innerMidEdge2);
		m_halfEdges.push_back(innerMidEdge3);
	}
}

void HalfEdgeMesh::updateHalfEdgeMesh()
{
	//Clears the lists of any vertices
	m_verticesHE.clear();
	m_uvsHE.clear();
	m_normalsHE.clear();
	m_indicesHE.clear();

	//Using map to speed up the generation of index list
	std::map<const Vertex*, unsigned int> indexMap;

	//Puts data from halfedge vertex list into rendering lists.
	//Puts vertex into the map associated with an index
	for (int i = 0; i < m_vertexData.size();i++){
		m_verticesHE.push_back(m_vertexData[i]->pos);
		m_uvsHE.push_back(m_vertexData[i]->uv);
		m_normalsHE.push_back(m_vertexData[i]->normal);

		indexMap[m_vertexData[i]] = i;
	}
	//Iterating faces to find which three vertices form a triangle. Checks in the map for the vertex to find the index.
	//This will create indices list for rendering
	for (int i = 0; i < m_faces.size(); i++){
		unsigned int index;
		std::map<const Vertex*, unsigned int>::iterator it;
		it = indexMap.find(m_faces[i]->edge->origin);
		index = it->second;
		m_indicesHE.push_back(index);

		it = indexMap.find(m_faces[i]->edge->next->origin);
		index = it->second;
		m_indicesHE.push_back(index);

		it = indexMap.find(m_faces[i]->edge->next->next->origin);
		index = it->second;
		m_indicesHE.push_back(index);
	}
}

void HalfEdgeMesh::initWireframeOriginalMesh()
{
	//////////////////////////////////////////////////////////////////////////
	//Create VAO
	m_glFunctions->glGenVertexArrays(1, &m_originalVAO);
	//Bind VAO
	m_glFunctions->glBindVertexArray(m_originalVAO);

	//////////////////////////////////////////////////////////////////////////
	//Vertex VBO
	//Create VBO on the GPU to store the vertex data
	m_glFunctions->glGenBuffers(1, &m_originalVerticesVBO);
	//Bind VBO to make it current
	m_glFunctions->glBindBuffer(GL_ARRAY_BUFFER, m_originalVerticesVBO);
	//Set the usage type, allocate VRAM and send the vertex data to the GPU
	m_glFunctions->glBufferData(GL_ARRAY_BUFFER, m_verticesOriginal.size() * sizeof(Vector3), &m_verticesOriginal[0], GL_STATIC_DRAW);

	//Sets up which shader attribute will received the data. How many elements will form a vertex, type etc
	m_glFunctions->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	//Enable the shader attribute to receive data
	m_glFunctions->glEnableVertexAttribArray(0);

	//////////////////////////////////////////////////////////////////////////
	//Indices EBO
	//Create EBO on the GPU to store the vertex data
	m_glFunctions->glGenBuffers(1, &m_originalIndicesEBO);
	//Bind EBO to make it current
	m_glFunctions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_originalIndicesEBO);
	//Set the usage type, allocate VRAM and send the vertex data to the GPU
	m_glFunctions->glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indicesOriginal.size() * sizeof(unsigned int), &m_indicesOriginal[0], GL_STATIC_DRAW);

	//////////////////////////////////////////////////////////////////////////
	//Unbind the VAO now that the VBOs have been set up
	m_glFunctions->glBindVertexArray(0);
}

void HalfEdgeMesh::releaseWireframeOriginalMesh()
{
	//Delete VBOs
	m_glFunctions->glDeleteBuffers(1, &m_originalVerticesVBO);
	m_glFunctions->glDeleteBuffers(1, &m_originalIndicesEBO);

	m_glFunctions->glDeleteBuffers(1, &m_originalVAO);
}

void HalfEdgeMesh::drawWireframeOriginalMesh(const Matrix44& model, const Matrix44& view, const Matrix44& projection)
{
	m_glFunctions->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//Bind the shaders to be used
	m_glFunctions->glUniform1i(m_shaderModeLocation, 12);

	//Mvp
	Matrix44 mvp = projection * view * model;

	//Send the mvp matrix to the vertex shader
	m_glFunctions->glUniformMatrix4fv(m_mvpLocation, 1, GL_TRUE, &mvp[0][0]);

	//Bind this mesh VAO
	m_glFunctions->glBindVertexArray(m_originalVAO);

	//Draw the triangles using the index buffer(EBO)
	m_glFunctions->glDrawElements(GL_TRIANGLES, m_indicesOriginal.size(), GL_UNSIGNED_INT, 0);
	m_glFunctions->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//Unbind the VAO
	m_glFunctions->glBindVertexArray(0);
}

void HalfEdgeMesh::calculateNormals()
{
	//Limit Normals
	for (int i = 0; i < m_vertexData.size(); i++){
		//Get the vertex to which we will calculate its new position
		Vertex* vertex = m_vertexData[i];
		//Get the half-edge of the vertex we are working on
		HalfEdge* halfedge = m_vertexData[i]->edge;

		//Will store all the neighbors found
		std::vector<Vertex*> neighbors;
		//Traverser, traversing all neighbors of the vertex
		HalfEdge* traverser = halfedge->next;

		//Do until halfedge is same as start halfedge
		do
		{
			//Handle current neighbor vertex
			neighbors.push_back(traverser->origin);

			//Update traverse pointer to next neighbor
			traverser = traverser->next->pair->next;
		} while (traverser != halfedge->next);

		//Amount of neighbors around the vertex
		float n = neighbors.size();

		//Tangent vector 1
		Vector3 t1;
		//Tangent vector 2
		Vector3 t2;

		for (int i = 0; i < neighbors.size(); i++){
			t1 += cos((2.0 * MyPersonalMathLibraryConstants::PI*i) / n) * neighbors[i]->pos;
			t2 += sin((2.0 * MyPersonalMathLibraryConstants::PI*i) / n) * neighbors[i]->pos;
		}
		//Limit normal using cross product
		vertex->normal = t1.Cross(t2);
	}
}

void HalfEdgeMesh::saveMeshToFile(QWidget* renderWindow)
{
	QString filename = QFileDialog::getSaveFileName(renderWindow, "Save File", "untitled.obj", "Wavefront (*.obj)");
	QFile f(filename);
	f.open(QIODevice::WriteOnly);
	QTextStream stream(&f);
	//Vertices
	for (int i = 0; i < m_verticesHE.size(); i++){
		stream << "v " + QString::number(m_verticesHE[i].GetElement(0)) + " " + QString::number(m_verticesHE[i].GetElement(1)) + " " + QString::number(m_verticesHE[i].GetElement(2)) << endl;
	}
	//Uvs
	for (int i = 0; i < m_uvsHE.size(); i++){
		stream << "vt " + QString::number(m_uvsHE[i].GetElement(0)) + " " + QString::number(m_uvsHE[i].GetElement(1))<< endl;
	}
	//Normals
	for (int i = 0; i < m_normalsHE.size(); i++){
		stream << "vn " + QString::number(m_normalsHE[i].GetElement(0)) + " " + QString::number(m_normalsHE[i].GetElement(1)) + " " + QString::number(m_normalsHE[i].GetElement(2)) << endl;
	}
	//Indices
	for (int i = 0; i < m_indicesHE.size(); i+=3){
		stream << 
			"f " + QString::number(m_indicesHE[i] + 1) + "/" + QString::number(m_indicesHE[i] + 1) + "/" + QString::number(m_indicesHE[i] + 1) +
			" " + QString::number(m_indicesHE[i + 1] + 1) + "/" + QString::number(m_indicesHE[i + 1] + 1) + "/" + QString::number(m_indicesHE[i + 1] + 1) +
			" " + QString::number(m_indicesHE[i + 2] + 1) + "/" + QString::number(m_indicesHE[i + 2] + 1) + "/" + QString::number(m_indicesHE[i + 2] + 1) << endl;
	}

	f.close();
}
