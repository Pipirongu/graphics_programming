#include "Mesh.h"

Mesh::Mesh(QOpenGLFunctions_3_3_Core* functions)
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

	m_radiusBV = 0;
}

Mesh::~Mesh(void)
{
	releaseWireframeBoundingSphere();

	//Delete VBOs
	m_glFunctions->glDeleteBuffers(1, &m_vertexVBO);
	m_glFunctions->glDeleteBuffers(1, &m_uvVBO);
	m_glFunctions->glDeleteBuffers(1, &m_normalVBO);
	m_glFunctions->glDeleteBuffers(1, &m_indicesEBO);

	//Delete VAO
	m_glFunctions->glDeleteVertexArrays(1, &m_vao);
	m_glFunctions = NULL;

	//Clear children list
	m_children.clear();
}

void Mesh::update(Matrix44& view, const Matrix44& model)
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

void Mesh::draw(ViewFrustumCheck& frustumCheck, const Matrix44& projection, const Matrix44& view, int shaderBranch, const Matrix44& lightView, const Matrix44& model, float bvScaleFactor)
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
		//MVP matrix for light(camera is attached to where the light is)
		Matrix44 depthMVP = projection*lightView*model;

		Matrix44 modelMatrix = model;
		Matrix44 viewMatrix = view;
		Matrix44 modelViewMatrix = view * model;

		//Send matrices to shader uniforms
		m_glFunctions->glUniformMatrix4fv(m_modelLocation, 1, GL_TRUE, &modelMatrix[0][0]);
		m_glFunctions->glUniformMatrix4fv(m_viewLocation, 1, GL_TRUE, &viewMatrix[0][0]);
		m_glFunctions->glUniformMatrix4fv(m_modelViewLocation, 1, GL_TRUE, &modelViewMatrix[0][0]);
		m_glFunctions->glUniformMatrix4fv(m_depthMVPLocation, 1, GL_TRUE, &depthMVP[0][0]);
		m_glFunctions->glUniformMatrix4fv(m_mvpLocation, 1, GL_TRUE, &mvp[0][0]);

		//Bind this mesh VAO
		m_glFunctions->glBindVertexArray(m_vao);
		//Draw the triangles using the index buffer(EBO)
		m_glFunctions->glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);

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

		//Draw wireframe BV sphere
		if (!m_isPlayer && !m_isWireframe && m_isWireframeBV && m_isFrustumCulling && !m_isSkybox && shaderBranch != -1){
			drawWireframeBoundingSphere(view, projection, worldSpaceCenterPointBV, scaledRadius);
			//Revert back to shader branch previously used before rendering the wireframe
			m_glFunctions->glUniform1i(m_shaderModeLocation, shaderBranch);
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
}

void Mesh::useShaderProgram(GLuint shaderProgram)
{
	m_shaderProgram = shaderProgram;

	//Get handles for shader uniforms
	m_shaderModeLocation = m_glFunctions->glGetUniformLocation(m_shaderProgram, "mode");
	m_skyBoxSamplerLocation = m_glFunctions->glGetUniformLocation(m_shaderProgram, "skyBoxSampler");
	m_textureSamplerLocation = m_glFunctions->glGetUniformLocation(m_shaderProgram, "textureSampler");
	m_depthMVPLocation = m_glFunctions->glGetUniformLocation(m_shaderProgram, "depthMVP");
	m_ambientMaterialLocation = m_glFunctions->glGetUniformLocation(m_shaderProgram, "AmbientMaterial");
	m_specularMaterialLocation = m_glFunctions->glGetUniformLocation(m_shaderProgram, "SpecularMaterial");
	m_shininessLocation = m_glFunctions->glGetUniformLocation(m_shaderProgram, "Shininess");
	m_mvpLocation = m_glFunctions->glGetUniformLocation(m_shaderProgram, "mvp");
	m_modelLocation = m_glFunctions->glGetUniformLocation(m_shaderProgram, "model");
	m_viewLocation = m_glFunctions->glGetUniformLocation(m_shaderProgram, "view");
	m_modelViewLocation = m_glFunctions->glGetUniformLocation(m_shaderProgram, "modelView");
}

void Mesh::useTexture(GLuint textureID)
{
	m_textureID = textureID;
}

void Mesh::loadOBJ(const char* path)
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
	indexVBO(sorted_vertices, sorted_uvs, sorted_normals);

	//////////////////////////////////////////////////////////////////////////
	//Create VAO
	m_glFunctions->glGenVertexArrays(1, &m_vao);
	//Bind VAO
	m_glFunctions->glBindVertexArray(m_vao);

	//////////////////////////////////////////////////////////////////////////
	//Vertex VBO
	//Create VBO on the GPU to store the vertex data
	m_glFunctions->glGenBuffers(1, &m_vertexVBO);
	//Bind VBO to make it current
	m_glFunctions->glBindBuffer(GL_ARRAY_BUFFER, m_vertexVBO);
	//Set the usage type, allocate VRAM and send the vertex data to the GPU
	m_glFunctions->glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vector3), &m_vertices[0], GL_STATIC_DRAW);

	//Sets up which shader attribute will received the data. How many elements will form a vertex, type etc
	m_glFunctions->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	//Enable the shader attribute to receive data
	m_glFunctions->glEnableVertexAttribArray(0);

	//////////////////////////////////////////////////////////////////////////
	//UV VBO
	//Create VBO on the GPU to store the vertex data
	m_glFunctions->glGenBuffers(1, &m_uvVBO);
	//Bind VBO to make it current
	m_glFunctions->glBindBuffer(GL_ARRAY_BUFFER, m_uvVBO);
	//Set the usage type, allocate VRAM and send the vertex data to the GPU
	m_glFunctions->glBufferData(GL_ARRAY_BUFFER, m_uvs.size() * sizeof(Vector2), &m_uvs[0], GL_STATIC_DRAW);

	//Sets up which shader attribute will receive the data. How many elements will form a vertex, type etc
	m_glFunctions->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	//Enable the shader attribute to receive data
	m_glFunctions->glEnableVertexAttribArray(1);

	//////////////////////////////////////////////////////////////////////////
	//Normal VBO
	//Create VBO on the GPU to store the vertex data
	m_glFunctions->glGenBuffers(1, &m_normalVBO);
	//Bind VBO to make it current
	m_glFunctions->glBindBuffer(GL_ARRAY_BUFFER, m_normalVBO);
	//Set the usage type, allocate VRAM and send the vertex data to the GPU
	m_glFunctions->glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(Vector3), &m_normals[0], GL_STATIC_DRAW);

	//Sets up which shader attribute will receive the data. How many elements will form a vertex, type etc
	m_glFunctions->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	//Enable the shader attribute to receive data
	m_glFunctions->glEnableVertexAttribArray(2);

	//////////////////////////////////////////////////////////////////////////
	//Indices EBO
	//Create EBO on the GPU to store the vertex data
	m_glFunctions->glGenBuffers(1, &m_indicesEBO);
	//Bind EBO to make it current
	m_glFunctions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesEBO);
	//Set the usage type, allocate VRAM and send the vertex data to the GPU
	m_glFunctions->glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

	//////////////////////////////////////////////////////////////////////////
	//Unbind the VAO now that the VBOs have been set up
	m_glFunctions->glBindVertexArray(0);

	calculateBoundingSphere();
}

bool Mesh::getSimilarVertexIndex(PackedVertex& packed, std::map<PackedVertex, unsigned int>& VertexToOutIndex, unsigned int& result)
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

void Mesh::indexVBO(std::vector<Vector3>& in_vertices, std::vector<Vector2>& in_uvs, std::vector<Vector3>& in_normals)
{
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
			m_indices.push_back(index);
		}
		//If not found then add the vertex data to our vertex arrays.
		//Also add the the index of the newly added vertex data to indices array. Which will be the last element.
		//Add the vertex data to the map and associate it with the index.
		//So we can iterate next time and check for same vertex data and use the index
		else{ // If not, it needs to be added in the output data.
			m_vertices.push_back(in_vertices[i]);
			m_uvs.push_back(in_uvs[i]);
			m_normals.push_back(in_normals[i]);
			unsigned int newindex = (unsigned int)m_vertices.size() - 1;
			m_indices.push_back(newindex);
			VertexToOutIndex[packed] = newindex;
		}
	}
}

void Mesh::calculateBoundingSphere(){

	//Calculate the center point by getting the mean value of vertices
	for (int i = 0; i < m_vertices.size(); i++){
		m_centerPointBV += m_vertices[i];
	}
	m_centerPointBV /= m_vertices.size();

	//Find the farthest distance from the center point by iterating all vertices all calculating the distance
	for (int i = 0; i < m_vertices.size(); i++){
		Vector3 pos = m_vertices[i] - m_centerPointBV;
		float distance = pos.Magnitude();
		if (distance > m_radiusBV){
			m_radiusBV = distance;
		}
	}

}

void Mesh::setAmbientMaterial(float r, float g, float b)
{
	m_ambientMaterial.Insert(r, g, b);
}

void Mesh::setSpecularMaterial(float r, float g, float b, float shininess)
{
	m_specularMaterial.Insert(r, g, b);
	m_shininess = shininess;
}

void Mesh::drawWireframeBoundingSphere(const Matrix44& view, const Matrix44& projection, Vector3 position, float radius)
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

void Mesh::setNodeType(MeshType node)
{
	if (node == PlayerMesh){
		m_isPlayer = true;
	}
	if (node == SkyboxMesh){
		m_isSkybox = true;
	}
}

void Mesh::setWireframeMode(bool flag)
{
	m_isWireframe = flag;
}

void Mesh::setFrustumCulling(bool flag)
{
	m_isFrustumCulling = flag;
}

void Mesh::setWireframeBV(bool flag)
{
	m_isWireframeBV = flag;
}

void Mesh::initWireframeBoundingSphere()
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

void Mesh::releaseWireframeBoundingSphere()
{
	//Delete VBOs
	m_glFunctions->glDeleteBuffers(1, &m_wireframeBvVBO);

	m_glFunctions->glDeleteBuffers(1, &m_wireframeBvVAO);
	m_wireframeBvVertices.clear();
}