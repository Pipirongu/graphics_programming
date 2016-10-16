#include "openglwin.h"

OpenGLWin::OpenGLWin(const QGLFormat& format)
	: QGLWidget(format)
{
	//Sets radix character to . and not ,
	setlocale(LC_ALL, "POSIX");
	ui.setupUi(this);

	//Fullscreen Quad vertices
	m_quadVertices.push_back(Vector3(-1.0f, -1.0f, +0.0f));
	m_quadVertices.push_back(Vector3(+1.0f, -1.0f, +0.0f));
	m_quadVertices.push_back(Vector3(-1.0f, +1.0f, +0.0f));
	m_quadVertices.push_back(Vector3(-1.0f, +1.0f, +0.0f));
	m_quadVertices.push_back(Vector3(+1.0f, -1.0f, +0.0f));
	m_quadVertices.push_back(Vector3(+1.0f, +1.0f, +0.0f));

	m_latestShapeAdded = NULL;
	//The random seed
	srand(time(NULL));

	m_wireframeToggle = false;
	m_frustumCheckToggle = true;
	m_wireframeBVToggle = false;
	m_originalMeshHEToggle = false;

	m_isFocus = true;
	
	m_glFunctions = new QOpenGLFunctions_3_3_Core;
	m_isShapes = false;
	m_isSolarSystem = false;
	m_isSubdivision = false;
	m_isShadowmap = false;
	m_isDeferredShading = false;

	//Counters
	m_shapesAddedToScene = 0;
	m_subdivisionCounter = 0;
	m_dsLightCounter = 0;

	//////////////////////////////////////////////////////////////////////////
	//Start position of the player in world space
	m_position.Insert(0, 0, 50);

	// horizontal angle : toward -Z
	m_horizontalAngle = 3.14f;
	// vertical angle : 0, look at the horizon
	m_verticalAngle = 0.0f;

	m_keyboardSensitivity = 0.05f;
	m_mouseSensitivity = 0.005f;
	m_isMousePress = false;

	m_fov = 60;
	m_near = 0.5;// 0.1;
	m_far = 1000;
	m_debugLogger = NULL;

	//Position of the window when it's created
	move(1000, 300);
	//Size of the window when it's created
	resize(600, 400);
	//Set timer to update openGL widget. 
	m_timer.setInterval(0);
	//Set the accuracy of the timer
	m_timer.setTimerType(Qt::PreciseTimer);
	//When timer timeout call these SLOTS
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(keyPress()));
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(mousePress()));
	//Start the timers
	m_timer.start();
	m_fpsTimer.start();
	m_elapsedTimer.start();
}

OpenGLWin::~OpenGLWin()
{
	qDebug() << "Destructor called";

	//FBO and their Textures
	//////////////////////////////////////////////////////////////////////////
	//Shadow Map
	m_glFunctions->glDeleteTextures(1, &m_shadowMapTexture);
	m_glFunctions->glDeleteTextures(1, &m_shadowMapBlurTexture);
	m_glFunctions->glDeleteRenderbuffers(1, &m_shadowmapDepthBuffer);
	m_glFunctions->glDeleteFramebuffers(1, &m_shadowMapFBO);
	m_glFunctions->glDeleteFramebuffers(1, &m_shadowMapBlurFBO);

	//Deferred Shading
	m_glFunctions->glDeleteTextures(1, &m_deferredShadingDepthTexture);
	m_glFunctions->glDeleteTextures(1, &m_deferredShadingFinalTexture);
	//G-Buffer
	m_glFunctions->glDeleteTextures(1, &m_diffuseTexture);
	m_glFunctions->glDeleteTextures(1, &m_positionTexture);
	m_glFunctions->glDeleteTextures(1, &m_normalsTexture);
	m_glFunctions->glDeleteFramebuffers(1, &m_deferredShadingFBO);

	//Fullscreen Quad
	m_glFunctions->glDeleteBuffers(1, &m_quadVBO);
	m_glFunctions->glDeleteVertexArrays(1, &m_quadVAO);

	//Deallocate everything in the lists
	if (!m_cameraList.empty()){
		for (int i = 0; i < m_cameraList.size(); i++){
			delete m_cameraList[i];
		}
	}
	if (!m_transformList.empty()){
		for (int i = 0; i < m_transformList.size(); i++){
			delete m_transformList[i];
		}
	}
	if (!m_meshList.empty()){
		for (int i = 0; i < m_meshList.size(); i++){
			delete m_meshList[i];
		}
	}
	if (!m_textureList.empty()){
		for (int i = 0; i < m_textureList.size(); i++){
			delete m_textureList[i];
		}
	}
	if (!m_shaderProgramList.empty()){
		for (int i = 0; i < m_shaderProgramList.size(); i++){
			delete m_shaderProgramList[i];
		}
	}
	if (!m_halfEdgeMeshList.empty()){
		for (int i = 0; i < m_halfEdgeMeshList.size(); i++){
			delete m_halfEdgeMeshList[i];
		}
	}
	if (!m_dsLightTransformList.empty()){
		for (int i = 0; i < m_dsLightTransformList.size(); i++){
			delete m_dsLightTransformList[i];
		}
	}
	if (!m_dsLightList.empty()){
		for (int i = 0; i < m_dsLightList.size(); i++){
			delete m_dsLightList[i];
		}
	}

	//Clear the lists
	m_cameraList.clear();
	m_transformList.clear();
	m_meshList.clear();
	m_textureList.clear();
	m_shaderProgramList.clear();
	m_halfEdgeMeshList.clear();
	m_dsLightTransformList.clear();
	m_dsLightList.clear();

	//If the debugger is used then deallocate it
	if (m_debugLogger != NULL){
		delete m_debugLogger;
	}

	//Delete the resources used by the player nodes and skybox
	delete m_root;
	delete m_skyboxT;
	delete m_skyboxM;
	delete m_playerT;
	
	delete m_pointLight;

	delete m_glFunctions;
}

void OpenGLWin::setFov(float fov)
{
	//Sets a new fov for perspective projection matrix
	m_fov = fov;
	qDebug() << "Field of View: " << m_fov << endl;
	float aspect = (float)m_width / (float)m_height;
	m_projection.Perspective(m_fov, aspect, m_near, m_far);
}

void OpenGLWin::setSceneType(SceneType scene)
{
	if (scene == Shapes){
		m_isShapes = true;
	}
	if (scene == SolarSystem){
		m_isSolarSystem = true;
	}
	if (scene == Subdivision){
		m_isSubdivision = true;
	}
	if (scene == Shadowmap){
		m_isShadowmap = true;
	}
	if (scene == Deferred_Shading){
		m_isDeferredShading = true;
	}
}

//Default state of OpenGL
void OpenGLWin::initializeGL()
{
	//Needed to use QOpenGLFunctions_3_3_Core functions 
	m_glFunctions->initializeOpenGLFunctions();
	//openGL debugger
	//initErrorCheck();

	//Check if openGL context has MSAA enabled
	QGLFormat glFormat = QGLWidget::format();
	if (!glFormat.sampleBuffers()){
		qWarning() << "Could not enable sample buffers";
	}
	else{
		qDebug() << "16x MSAA Enabled";
	}

	m_glFunctions->glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	m_glFunctions->glDepthFunc(GL_LESS);
	//Enable back face culling
	m_glFunctions->glEnable(GL_CULL_FACE);
	// Set the clear color to black
	m_glFunctions->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//////////////////////////////////////////////////////////////////////////
	//Create the shader programs
	m_uberShaderProgram = new ShaderProgram(m_glFunctions);

	//Compile and link shaders
	m_uberShaderProgram->prepareShaderProgram("shaders/uberVertexShader.glsl", "shaders/uberFragmentShader.glsl");

	//Push to the lists to deallocate easier
	m_shaderProgramList.push_back(m_uberShaderProgram);

	//Associate shader variables to Handles
	m_shaderModeLocation = m_glFunctions->glGetUniformLocation(m_uberShaderProgram->getShaderProgramID(), "mode");
	m_scaleUniform = m_glFunctions->glGetUniformLocation(m_uberShaderProgram->getShaderProgramID(), "ScaleU");
	m_textureSamplerLocation = m_glFunctions->glGetUniformLocation(m_uberShaderProgram->getShaderProgramID(), "textureSampler");
	m_shadowMapSamplerLocation = m_glFunctions->glGetUniformLocation(m_uberShaderProgram->getShaderProgramID(), "shadowMapSampler");
	m_screenSizeLocation = m_glFunctions->glGetUniformLocation(m_uberShaderProgram->getShaderProgramID(), "screenSize");
	m_fragmentViewMatrixLocation = m_glFunctions->glGetUniformLocation(m_uberShaderProgram->getShaderProgramID(), "viewMatrix");
	m_maxLightRadiusLocation = m_glFunctions->glGetUniformLocation(m_uberShaderProgram->getShaderProgramID(), "maxLightRadius");

	//G-Buffer
	m_diffuseSamplerLocation = m_glFunctions->glGetUniformLocation(m_uberShaderProgram->getShaderProgramID(), "diffuseSampler");
	m_positionSamplerLocation = m_glFunctions->glGetUniformLocation(m_uberShaderProgram->getShaderProgramID(), "positionSampler");
	m_normalsSamplerLocation = m_glFunctions->glGetUniformLocation(m_uberShaderProgram->getShaderProgramID(), "normalsSampler");

	//Bind shader to use
	m_glFunctions->glUseProgram(m_uberShaderProgram->getShaderProgramID());

	//Associate texture unit to samplers
	m_glFunctions->glUniform1i(m_textureSamplerLocation, 0);
	m_glFunctions->glUniform1i(m_shadowMapSamplerLocation, 1);
	//G-Buffer
	m_glFunctions->glUniform1i(m_diffuseSamplerLocation, 2);
	m_glFunctions->glUniform1i(m_positionSamplerLocation, 3);
	m_glFunctions->glUniform1i(m_normalsSamplerLocation, 4);

	//////////////////////////////////////////////////////////////////////////
	//Textures for skybox
	m_skyboxTexture = new Texture(m_glFunctions);
	m_skyboxTexture->loadSkyboxTexture(
		"textures/skybox/skybox_back",
		"textures/skybox/skybox_down",
		"textures/skybox/skybox_front",
		"textures/skybox/skybox_left", 
		"textures/skybox/skybox_right", 
		"textures/skybox/skybox_up");

	//Push to the lists to deallocate easier
	m_textureList.push_back(m_skyboxTexture);

	//////////////////////////////////////////////////////////////////////////
	//Transforms
	m_root = new Transform;
	m_root->setNodeType(RootTransform);

	//Skybox
	m_skyboxT = new Transform;
	m_skyboxT->scale(150);
	m_skyboxT->setNodeType(SkyboxTransform);

	//Player
	m_playerT = new Transform;
	m_playerT->setNodeType(PlayerTransform);

	//////////////////////////////////////////////////////////////////////////
	//Skybox Mesh
	m_skyboxM = new Mesh(m_glFunctions);
	m_skyboxM->setNodeType(SkyboxMesh);
	m_skyboxM->useShaderProgram(m_uberShaderProgram->getShaderProgramID());
	m_skyboxM->useTexture(m_skyboxTexture->getTextureID());
	m_skyboxM->loadOBJ("models/cube.obj");

	//////////////////////////////////////////////////////////////////////////
	//Third Person Camera
	Transform* camera1 = new Transform;
	camera1->setNodeType(CameraTransform);
	//Push to the lists to deallocate easier
	m_cameraList.push_back(camera1);

	//Sun Light
	m_pointLight = new Light(m_glFunctions);
	m_pointLight->useShaderProgram(m_uberShaderProgram->getShaderProgramID());

	//////////////////////////////////////////////////////////////////////////
	//Connect the Scenegraph
	m_root->addChildNode(m_playerT);
	m_playerT->addChildNode(m_skyboxT);

	if (m_isShapes){
		//Attach camera to player
		m_playerT->addChildNode(m_cameraList[0]);

		//Textures
		m_pyramidTexture = new Texture(m_glFunctions);
		m_cubeTexture = new Texture(m_glFunctions);
		m_sphereTexture = new Texture(m_glFunctions);

		m_pyramidTexture->loadTexture("textures/pyramid");
		m_cubeTexture->loadTexture("textures/cube");
		m_sphereTexture->loadTexture("textures/sphere");

		//Push to the lists to deallocate easier
		m_textureList.push_back(m_pyramidTexture);
		m_textureList.push_back(m_cubeTexture);
		m_textureList.push_back(m_sphereTexture);

		//Pyramid Mesh
		m_pyramidM = new Mesh(m_glFunctions);
		m_pyramidM->useShaderProgram(m_uberShaderProgram->getShaderProgramID());
		m_pyramidM->useTexture(m_pyramidTexture->getTextureID());
		m_pyramidM->loadOBJ("models/pyramid.obj");

		//Cube Mesh
		m_cubeM = new Mesh(m_glFunctions);
		m_cubeM->useShaderProgram(m_uberShaderProgram->getShaderProgramID());
		m_cubeM->useTexture(m_cubeTexture->getTextureID());
		m_cubeM->loadOBJ("models/cube.obj");

		//Sphere Mesh
		m_sphereM = new Mesh(m_glFunctions);
		m_sphereM->useShaderProgram(m_uberShaderProgram->getShaderProgramID());
		m_sphereM->useTexture(m_sphereTexture->getTextureID());
		m_sphereM->loadOBJ("models/sphere.obj");

		//Push to the lists to deallocate easier
		m_meshList.push_back(m_pyramidM);
		m_meshList.push_back(m_cubeM);
		m_meshList.push_back(m_sphereM);
	}	
	if (m_isSolarSystem){
		//Attach camera to player
		m_playerT->addChildNode(m_cameraList[0]);

		//Textures
		m_sunTexture = new Texture(m_glFunctions);
		m_planet1Texture = new Texture(m_glFunctions);
		m_planet2Texture = new Texture(m_glFunctions);
		m_planet3Texture = new Texture(m_glFunctions);
		m_moonTexture = new Texture(m_glFunctions);

		m_sunTexture->loadTexture("textures/sun");
		m_planet1Texture->loadTexture("textures/planet1");
		m_planet2Texture->loadTexture("textures/planet2");
		m_planet3Texture->loadTexture("textures/planet3");
		m_moonTexture->loadTexture("textures/moon");

		//Push to the lists to deallocate easier
		m_textureList.push_back(m_sunTexture);
		m_textureList.push_back(m_planet1Texture);
		m_textureList.push_back(m_planet2Texture);
		m_textureList.push_back(m_planet3Texture);
		m_textureList.push_back(m_moonTexture);

		//Transforms
		//Sun Transform
		m_sunT = new Transform;
		m_sunT->translate(0, 0, -30);

		//Planet1 Transform
		m_planet1T = new Transform;
		m_planet1T->scale(0.5);
		m_planet1T->translate(0, 0, -100);

		//Planet2 Transform
		m_planet2T = new Transform;
		m_planet2T->scale(0.5);
		m_planet2T->translate(0, 0, -150);

		//Planet3 Transform
		m_planet3T = new Transform;
		m_planet3T->scale(0.5);
		m_planet3T->translate(0, 0, -200);
		//Rotate 23 degrees
		m_planet3T->rotate(23, 1, 0, 0);

		//Moon Transform
		m_moonT = new Transform;
		m_moonT->scale(0.45);
		m_moonT->translate(0, 0, -60);

		//Push to the lists to deallocate easier
		m_transformList.push_back(m_sunT);
		m_transformList.push_back(m_planet1T);
		m_transformList.push_back(m_planet2T);
		m_transformList.push_back(m_planet3T);
		m_transformList.push_back(m_moonT);

		//Meshes
		//Sun Mesh
		m_sunM = new Mesh(m_glFunctions);
		m_sunM->useShaderProgram(m_uberShaderProgram->getShaderProgramID());
		m_sunM->useTexture(m_sunTexture->getTextureID());
		m_sunM->loadOBJ("models/sphere.obj");
		m_sunM->setAmbientMaterial(3.0, 3.0, 3.0);

		//Planet1 Mesh
		m_planet1M = new Mesh(m_glFunctions);
		m_planet1M->useShaderProgram(m_uberShaderProgram->getShaderProgramID());
		m_planet1M->useTexture(m_planet1Texture->getTextureID());
		m_planet1M->loadOBJ("models/sphere.obj");

		//Planet2 Mesh
		m_planet2M = new Mesh(m_glFunctions);
		m_planet2M->useShaderProgram(m_uberShaderProgram->getShaderProgramID());
		m_planet2M->useTexture(m_planet2Texture->getTextureID());
		m_planet2M->loadOBJ("models/sphere.obj");

		//Planet3 Mesh
		m_planet3M = new Mesh(m_glFunctions);
		m_planet3M->useShaderProgram(m_uberShaderProgram->getShaderProgramID());
		m_planet3M->useTexture(m_planet3Texture->getTextureID());
		m_planet3M->loadOBJ("models/sphere.obj");

		//Moon Mesh
		m_moonM = new Mesh(m_glFunctions);
		m_moonM->useShaderProgram(m_uberShaderProgram->getShaderProgramID());
		m_moonM->useTexture(m_moonTexture->getTextureID());
		m_moonM->loadOBJ("models/sphere.obj");

		//////////////////////////////////////////////////////////////////////////
		//Push to the lists to deallocate easier
		m_meshList.push_back(m_sunM);
		m_meshList.push_back(m_planet1M);
		m_meshList.push_back(m_planet2M);
		m_meshList.push_back(m_planet3M);
		m_meshList.push_back(m_moonM);

		//Attach to scenegraph
		//Sun linked to Root
		m_root->addChildNode(m_sunT);
		m_sunT->addChildNode(m_sunM);
		m_shapesAddedToScene++;

		//Planet1 linked to Sun
		m_sunT->addChildNode(m_planet1T);
		m_planet1T->addChildNode(m_planet1M);
		m_shapesAddedToScene++;

		//Planet2 linked to Sun
		m_sunT->addChildNode(m_planet2T);
		m_planet2T->addChildNode(m_planet2M);
		m_shapesAddedToScene++;

		//Planet3 linked to Sun
		m_sunT->addChildNode(m_planet3T);
		m_planet3T->addChildNode(m_planet3M);
		m_shapesAddedToScene++;

		//Moon linked to Planet3
		m_planet3T->addChildNode(m_moonT);
		m_moonT->addChildNode(m_moonM);
		m_shapesAddedToScene++;
	}
	if (m_isSubdivision){
		//Attach camera to player
		m_playerT->addChildNode(m_cameraList[0]);

		//Textures
		m_subdivisionCubeTexture = new Texture(m_glFunctions);
		m_subdivisionCubeTexture->loadTexture("textures/planet2");

		//Push to the lists to deallocate easier
		m_textureList.push_back(m_subdivisionCubeTexture);

		//Transforms
		m_subdivisionCubeT = new Transform;
		m_subdivisionCubeT->scale(20);

		//Push to the lists to deallocate easier
		m_transformList.push_back(m_subdivisionCubeT);

		//Halfedge Mesh
		m_subdivisionCubeM = new HalfEdgeMesh(m_glFunctions);
		m_subdivisionCubeM->useShaderProgram(m_uberShaderProgram->getShaderProgramID());
		m_subdivisionCubeM->useTexture(m_subdivisionCubeTexture->getTextureID());
		m_subdivisionCubeM->loadOBJ("models/cube.obj");

		//Push to the lists to deallocate easier
		m_halfEdgeMeshList.push_back(m_subdivisionCubeM);

		//Attach to scenegraph
		m_root->addChildNode(m_subdivisionCubeT);
		m_subdivisionCubeT->addChildNode(m_subdivisionCubeM);
		m_shapesAddedToScene++;
	}
	if (m_isShadowmap){
		//Attach camera to player
		m_playerT->addChildNode(m_cameraList[0]);

		//Textures
		m_shadowmapTexture = new Texture(m_glFunctions);
		m_shadowmapTexture->loadTexture("textures/moon");

		//Push to the lists to deallocate easier
		m_textureList.push_back(m_shadowmapTexture);

		//Transforms
		m_shadowmapT = new Transform;
		m_shadowMapPointLightT = new Transform();
		m_shadowMapPointLightT1 = new Transform();

		m_shadowmapT->translate(0, -3, -15);
		m_shadowmapT->rotate(180, 0, 1, 0);
		m_shadowMapPointLightT1->scale(0.2);

		m_transformList.push_back(m_shadowmapT);
		m_transformList.push_back(m_shadowMapPointLightT);
		m_transformList.push_back(m_shadowMapPointLightT1);

		//Meshes
		m_shadowmapM = new Mesh(m_glFunctions);
		m_shadowmapM->useShaderProgram(m_uberShaderProgram->getShaderProgramID());
		m_shadowmapM->useTexture(m_shadowmapTexture->getTextureID());
		m_shadowmapM->loadOBJ("models/room.obj");

		m_shadowMapPointLightM = new Mesh(m_glFunctions);
		m_shadowMapPointLightM->useShaderProgram(m_uberShaderProgram->getShaderProgramID());
		m_shadowMapPointLightM->useTexture(m_shadowmapTexture->getTextureID());
		m_shadowMapPointLightM->loadOBJ("models/pointLightSphere.obj");
		m_shadowMapPointLightM->setAmbientMaterial(2, 2, 2);
		m_shapesAddedToScene++;

		//Push to the lists to deallocate easier
		m_meshList.push_back(m_shadowmapM);
		m_meshList.push_back(m_shadowMapPointLightM);
		
		//Attach to scenegraph
		m_root->addChildNode(m_shadowMapPointLightT);
		m_shadowMapPointLightT->addChildNode(m_shadowMapPointLightT1);
		m_root->addChildNode(m_shadowmapT);
		m_shadowmapT->addChildNode(m_shadowmapM);
		m_shapesAddedToScene++;

		//Setup FBOs(render targets) for shadow map
		initShadowMap();
	}
	if (m_isDeferredShading){
		//Attach camera to player
		m_playerT->addChildNode(m_cameraList[0]);

		//Textures
		m_deferredShadingTexture = new Texture(m_glFunctions);
		m_deferredShadingTexture->loadTexture("textures/moon");

		//Push to the lists to deallocate easier
		m_textureList.push_back(m_deferredShadingTexture);

		//Transforms
		m_deferredShadingT = new Transform;
		m_deferredShadingT->translate(0, -3, -5);
		m_deferredShadingT->rotate(180, 0, 1, 0);

		//Push to the lists to deallocate easier
		m_transformList.push_back(m_deferredShadingT);

		//Meshes
		m_deferredShadingM = new Mesh(m_glFunctions);
		m_deferredShadingM->useShaderProgram(m_uberShaderProgram->getShaderProgramID());
		m_deferredShadingM->useTexture(m_deferredShadingTexture->getTextureID());
		m_deferredShadingM->loadOBJ("models/room.obj");

		m_dsPointLightM = new Mesh(m_glFunctions);
		m_dsPointLightM->useShaderProgram(m_uberShaderProgram->getShaderProgramID());
		m_dsPointLightM->loadOBJ("models/pointLightSphere.obj");

		//Push to the lists to deallocate easier
		m_meshList.push_back(m_deferredShadingM);
		m_meshList.push_back(m_dsPointLightM);

		//Light Meshes for deferred shading
		addLightToDsScene(0, 0, -10, 1, 1, 1, 5.0);
		addLightToDsScene(3, 0, -3, 1, 1, 1, 5.0);
		addLightToDsScene(0, -3, -5, 1, 1, 1, 5.0);

		//Attach to scenegraph
		m_root->addChildNode(m_deferredShadingT);
		m_deferredShadingT->addChildNode(m_deferredShadingM);
		m_shapesAddedToScene = m_dsLightCounter;

		//Setup FBO(render target) for deferred shading
		initDeferredShading();
		//Start timer to prevent lights color change spam
		m_dsColorTimer.start();
	}

	//////////////////////////////////////////////////////////////////////////
	//Init the camera vectors just so the objects are rendered in the scene
	updateCamera();
}

//Function called when the openGL widget needs to update
void OpenGLWin::paintGL()
{
	//////////////////////////////////////////////////////////////////////////
	//Moves the player
	m_playerT->lookAt(m_position, m_position + m_direction, m_up);

	if (m_isShapes){
		drawShapesScene();
		drawSkybox();

	}
	if (m_isSolarSystem){	
		drawSolarSystemScene();
		drawSkybox();
	}
	if (m_isSubdivision){
		drawSubdivisionScene();
		drawSkybox();
	}
	if (m_isShadowmap){
		shadowMapPass1();
		blurShadowmap(9);
		shadowMapPass2();
		drawSkybox();
		drawShadowmapTexture();
	}
	if (m_isDeferredShading){
		//Randomize the lights' color after 250 ms
		if (m_dsColorTimer.elapsed() >= 250){
			for (int i = 0; i < m_dsLightCounter; i++){
				float red = (50 + rand() % 255) / 255.0f;
				float blue = (50 + rand() % 255) / 255.0f;
				float green = (50 + rand() % 255) / 255.0f;
				m_dsLightList[i]->setLightColor(red, blue, green);
			}
			m_dsColorTimer.restart();
		}
		//Rotate all lights in z axis
		for (int i = 0; i < m_dsLightCounter; i++){
			m_dsLightTransformList[i]->rotate((15 * m_deltaTime / 100), 0, 0, 1, World);
		}
		//Attach all Lights to scenegraph
		for (int i = 0; i < m_dsLightCounter; i++){
			m_root->addChildNode(m_dsLightTransformList[i]);
		}

		//Update scenegraph, Lights as well
		m_root->update(m_view);

		//Deattach the Lights from scenegraph
		for (int i = 0; i < m_dsLightCounter; i++){
			m_root->removeChildNode(m_dsLightTransformList[i]);
		}

		//Extracting the view frustum planes
		m_frustum.extractFrustum(m_view, m_projection);

		//Init the counter for rendred objects to the amount of objects created
		m_frustum.shapesRendered = m_shapesAddedToScene;

		//Render scene to G-Buffer
		dsGeometryPass();

		//Init the counter for rendred objects to the amount of objects created
		m_frustum.shapesRendered = m_shapesAddedToScene;

		//Enable Stencil Test for the Stencil/Light pass
		m_glFunctions->glEnable(GL_STENCIL_TEST);

		//Stencil / Light pass
		dsStencilAndLightPass();
		
		m_glFunctions->glCullFace(GL_BACK);
		m_glFunctions->glDisable(GL_STENCIL_TEST);

		//////////////////////////////////////////////////////////////////////////
		//Blend scene with ambient light with the light meshes
		dsDrawSceneWithAmbientLight();
		m_glFunctions->glDisable(GL_BLEND);

		m_glFunctions->glEnable(GL_DEPTH_TEST);
		drawSkybox();
		m_glFunctions->glDisable(GL_DEPTH_TEST);

		dsDrawGBufferTextures();
		//Blit the final scene to main frame buffer
		dsCopyFinalTextureToScreen();
	}
	//////////////////////////////////////////////////////////////////////////
	//DeltaTime
	m_deltaTime = (float)m_elapsedTimer.nsecsElapsed() / 1000000;

	//FPS Display
	if (m_fpsTimer.elapsed() >= 250){
		m_fpsTimeWindowTitle = QString::number((1000 / m_deltaTime)) + " FPS" + "   " + QString::number(m_deltaTime) + " ms/frame";
		m_fpsTimeGUI = QString::number((1000 / m_deltaTime)) + " FPS" + "\n" + QString::number(m_deltaTime) + " ms/frame";
		m_fpsTimer.restart();
	}
	//Window Title
	QString windowTitle = 
		m_fpsTimeWindowTitle
		+ "   [" + QString::number(m_shapesAddedToScene) + "]" + " Objects Added"
		+ "   [" + QString::number(m_frustum.shapesRendered) + "]" + " Objects Rendered"
		+ "   [" + QString::number(m_subdivisionCounter) + "]" + " Subdivisions";
	setWindowTitle(windowTitle);

	//GUI Display
	windowTitle = 
		m_fpsTimeGUI + "\n"
		+ "[" + QString::number(m_shapesAddedToScene) + "]" + " Objects Added" + "\n"
		+ "[" + QString::number(m_frustum.shapesRendered) + "]" + " Objects Rendered" + "\n"
		+ "[" + QString::number(m_subdivisionCounter) + "]" + " Subdivisions" + "\n";
	emit setGUIText(windowTitle);
	m_elapsedTimer.restart();
}

//Called when the window has resized
void OpenGLWin::resizeGL(int w, int h)
{
	m_width = w;
	m_height = h;
	float aspect = (float)w / (float)h;
	m_projection.Perspective(m_fov, aspect, m_near, m_far);

	//Update deferred shading textures whenever the window resizes
	updateDeferredShadingTextures(w, h);
}

void OpenGLWin::onMessageLogged(QOpenGLDebugMessage message)
{
	//Ignore LowSeverity messages
	if (message.severity() == QOpenGLDebugMessage::LowSeverity){
		return;
	}
	if (message.id() == 131218){ // NVIDIA: "shader will be recompiled due to GL state mismatches"(Some sort of error that doesn't affect the program if ignored)
		return;
	}
	//Print out the error messages
	qDebug() << message;
	int waitForUserInput;
	cin >> waitForUserInput;
}

void OpenGLWin::mousePress()
{
	if (m_isMousePress){
		//Stores the mouse coords
		QPoint mousePosition = QCursor::pos();

		//Coverts the widget's coordinates to screen's coordinates. NOTE: QT sees multiple screens as one big screen.
		QPoint mid = mapToGlobal(QPoint(width() / 2, height() / 2)); //X, Y of widget converted here. Divided by 2 to get the center position of the widget

		//Prevents camera from jumping
		if (abs(mid.x() - mousePosition.x()) > 100 || abs(mid.y() - mousePosition.y()) > 100){
			QCursor::setPos(mid);
			return;
		}

		//////////////////////////////////////////////////////////////////////////
		// Compute new orientation
		//look left and right
		m_horizontalAngle += m_mouseSensitivity * float(mid.x() - mousePosition.x());
		//look up and down
		m_verticalAngle += m_mouseSensitivity * float(mid.y() - mousePosition.y());

		//Locks the vertical angle to -90 to 90
		if (m_verticalAngle*180.0 / MyPersonalMathLibraryConstants::PI > 90)
			m_verticalAngle = 90 * MyPersonalMathLibraryConstants::PI / 180.0;
		if (m_verticalAngle*180.0 / MyPersonalMathLibraryConstants::PI < -90)
			m_verticalAngle = -90 * MyPersonalMathLibraryConstants::PI / 180.0;

		//Sets the horizontal angle between 0 and 360 
		if (m_horizontalAngle*180.0 / MyPersonalMathLibraryConstants::PI < 0.0)
			m_horizontalAngle += 360.0*MyPersonalMathLibraryConstants::PI / 180.0;
		if (m_horizontalAngle*180.0 / MyPersonalMathLibraryConstants::PI > 360.0)
			m_horizontalAngle -= 360 * MyPersonalMathLibraryConstants::PI / 180.0;

		//Update the camera vectors
		updateCamera();

		//////////////////////////////////////////////////////////////////////////
		//Hide the cursor
		setCursor(Qt::BlankCursor);
		//Set the mouse pointer to the middle of the widget
		QCursor::setPos(mid);
	}
	if (!m_isMousePress){
		//Show the cursor
		setCursor(Qt::ArrowCursor);
	}
}

void OpenGLWin::keyPress()
{
	//Clear the list when the window is out of focus to prevent the player from auto moving
	if (!m_isFocus){
		m_keysPressed.clear();
	}
	foreach(Qt::Key k, m_keysPressed){
		if (k == Qt::Key_W){
			m_position += m_direction * m_deltaTime * m_keyboardSensitivity;
		}
		if (k == Qt::Key_A){
			m_position -= m_right * m_deltaTime * m_keyboardSensitivity;
		}
		if (k == Qt::Key_S){
			m_position -= m_direction * m_deltaTime * m_keyboardSensitivity;
		}
		if (k == Qt::Key_D){
			m_position += m_right * m_deltaTime * m_keyboardSensitivity;
		}
		if (k == Qt::Key_Escape){
			exit(1);
		}
		if (k == Qt::Key_E && m_isShapes){
			addRandomShapeToScene();
		}
		if (k == Qt::Key_Q && m_isShapes){
			deleteLatestShape();
		}
		if (k == Qt::Key_Up && m_isShadowmap){
			m_shadowMapPointLightT->translate(0, (2 * m_deltaTime / 100), 0);
		}
		if (k == Qt::Key_Down && m_isShadowmap){
			m_shadowMapPointLightT->translate(0, -(2 * m_deltaTime / 100), 0);
		}
		if (k == Qt::Key_Left && m_isShadowmap){
			m_shadowMapPointLightT->translate(-(2 * m_deltaTime / 100), 0, 0);
		}
		if (k == Qt::Key_Right && m_isShadowmap){
			m_shadowMapPointLightT->translate((2 * m_deltaTime / 100), 0, 0);
		}
	}
}

void OpenGLWin::deleteLatestShape()
{
	if (m_isShapes){
		if (m_transformList.empty()){
			qDebug() << "No shapes to remove...";
			return;
		}
		m_root->removeChildNode(m_transformList[m_transformList.size() - 1]); //removes the latest transform
		
		m_latestShapeAdded = NULL;
		m_shapesAddedToScene--;
		emit resetScaleSlider();

		delete m_transformList[m_transformList.size() - 1]; //delete the transform
		m_transformList.pop_back(); //removes the transform from vector
	}
}

void OpenGLWin::enableWireframe(int toggle)
{
	if (toggle == 0){
		//off
		for (int i = 0; i < m_meshList.size(); i++)
		{
			m_meshList[i]->setWireframeMode(false);
		}
		for (int i = 0; i < m_halfEdgeMeshList.size(); i++){
			m_halfEdgeMeshList[i]->setWireframeMode(false);
		}
	}
	if (toggle == 1){
		//on
		for (int i = 0; i < m_meshList.size(); i++)
		{

			m_meshList[i]->setWireframeMode(true);
		}
		for (int i = 0; i < m_halfEdgeMeshList.size(); i++){
			m_halfEdgeMeshList[i]->setWireframeMode(true);
		}
	}
}

void OpenGLWin::enableFrustumCulling(int toggle)
{
	if (toggle == 0){
		//off
		for (int i = 0; i < m_meshList.size(); i++)
		{
			m_meshList[i]->setFrustumCulling(false);
		}
		for (int i = 0; i < m_halfEdgeMeshList.size(); i++){
			m_halfEdgeMeshList[i]->setFrustumCulling(false);
		}
		emit enableWireframeBVComboBox(0);
		emit updateWireframeBVComboBox(0);
	}
	if (toggle == 1){
		//on
		for (int i = 0; i < m_meshList.size(); i++)
		{
			m_meshList[i]->setFrustumCulling(true);
		}
		for (int i = 0; i < m_halfEdgeMeshList.size(); i++){
			m_halfEdgeMeshList[i]->setFrustumCulling(true);
		}
		emit enableWireframeBVComboBox(1);
		emit updateWireframeBVComboBox(0);
	}
}

void OpenGLWin::enableWireframeBV(int toggle)
{
	if (toggle == 0){
		//off
		for (int i = 0; i < m_meshList.size(); i++)
		{
			m_meshList[i]->setWireframeBV(false);
			//delete bv vbo
			m_meshList[i]->releaseWireframeBoundingSphere();
		}
		for (int i = 0; i < m_halfEdgeMeshList.size(); i++){
			m_halfEdgeMeshList[i]->setWireframeBV(false);
			//delete bv vbo
			m_halfEdgeMeshList[i]->releaseWireframeBoundingSphere();
		}
	}
	if (toggle == 1){
		//on
		for (int i = 0; i < m_meshList.size(); i++)
		{
			m_meshList[i]->setWireframeBV(true);
			//create vbos
			m_meshList[i]->initWireframeBoundingSphere();
		}
		for (int i = 0; i < m_halfEdgeMeshList.size(); i++){
			m_halfEdgeMeshList[i]->setWireframeBV(true);
			//create vbos
			m_halfEdgeMeshList[i]->initWireframeBoundingSphere();
		}
	}
}

void OpenGLWin::enableWireframeOriginalMeshHE(int toggle)
{
	if (toggle == 0){
		for (int i = 0; i < m_halfEdgeMeshList.size(); i++){
			m_halfEdgeMeshList[i]->setWireframeOriginalMeshHE(false);
			//delete bv vbo
			m_halfEdgeMeshList[i]->releaseWireframeOriginalMesh();
		}
		emit updateWireframeOriginalMeshHEComboBox(0);
	}
	if (toggle == 1){
		for (int i = 0; i < m_halfEdgeMeshList.size(); i++){
			m_halfEdgeMeshList[i]->setWireframeOriginalMeshHE(true);
			//create vbos
			m_halfEdgeMeshList[i]->initWireframeOriginalMesh();
		}
		emit updateWireframeOriginalMeshHEComboBox(1);
	}
}

void OpenGLWin::addRandomShapeToScene()
{
	if (m_isShapes){
		//The transform of the random shape to be added
		Transform* shapeT = new Transform;
		m_transformList.push_back(shapeT);

		//Min 1 max 3. Randomize the Mesh of the shape
		int randomMesh = 1 + rand() % 3;
		if (randomMesh == 1){
			shapeT->addChildNode(m_pyramidM);
		}
		if (randomMesh == 2){
			shapeT->addChildNode(m_cubeM);
		}
		if (randomMesh == 3){
			shapeT->scale(0.1);
			shapeT->addChildNode(m_sphereM);
		}

		//Get the player translation
		Matrix44 playerPosition = m_playerT->getMatrix();
		int playerPosX = playerPosition[0][3];
		int playerPosY = playerPosition[1][3];
		int playerPosZ = playerPosition[2][3];

		//Theta and phi are used to randomize position of the shape in a sphere shape
		int theta = 0 + rand() % 180;
		int phi = 0 + rand() % 360;

		//Generate the position of the shape
		int posX = playerPosX + (50 * cos(phi* MyPersonalMathLibraryConstants::PI / 180.0) * sin(theta*MyPersonalMathLibraryConstants::PI / 180.0));
		int posY = playerPosY + (50 * sin(phi*MyPersonalMathLibraryConstants::PI / 180.0) * sin(theta*MyPersonalMathLibraryConstants::PI / 180.0));
		int posZ = playerPosZ + (50 * cos(theta*MyPersonalMathLibraryConstants::PI / 180.0));

		//Translate the shape
		shapeT->translate(posX, posY, posZ);

		m_root->addChildNode(shapeT);
		m_latestShapeAdded = shapeT;
		m_shapesAddedToScene++;
		emit resetScaleSlider();
	}
}

void OpenGLWin::setScaleLatestShape(int factor)
{
	if (m_latestShapeAdded != NULL){
		m_latestShapeAdded->setScale(factor);
	}
	else{
		qDebug() << "Add a new shape to be able to scale it";
	}
}

bool OpenGLWin::event(QEvent * event)
{
	//If the window is focused
	if (event->type() == QEvent::WindowActivate){
		m_isFocus = true;
	}
	//If the window is not focused
	if (event->type() == QEvent::WindowDeactivate){
		m_isFocus = false;
	}
	return QGLWidget::event(event);
}

void OpenGLWin::mouseMoveEvent(QMouseEvent* event)
{
	m_isMousePress = true;
}

void OpenGLWin::mouseReleaseEvent(QMouseEvent* event)
{
	m_isMousePress = false;
}

//User input listener. Puts all the keys pressed in a list
void OpenGLWin::keyPressEvent( QKeyEvent* event )
{
	if(event->isAutoRepeat())
	{
		event->ignore();
	}
	else{
		//Wireframe Toggle
		if (event->key() == Qt::Key_1){
			if (!m_wireframeToggle){
				m_wireframeBVToggle = false;
				enableWireframeBV(0);
				emit updateWireframeBVComboBox(0);

				enableWireframe(1);
				m_wireframeToggle = true;
				//emit index has changed
				emit updateWireframeComboBox(1);
			}
			else if (m_wireframeToggle){
				enableWireframe(0);
				m_wireframeToggle = false;
				//emit index has changed
				emit updateWireframeComboBox(0);
			}
		}
		//Frustum Check Toggle
		if (event->key() == Qt::Key_2){
			if (!m_frustumCheckToggle){
				enableFrustumCulling(1);
				m_frustumCheckToggle = true;
				//emit index has changed
				emit updateFrustumComboBox(1);
			}
			else if (m_frustumCheckToggle){
				m_wireframeBVToggle = false;
				enableWireframeBV(0);
				emit updateWireframeBVComboBox(0);

				enableFrustumCulling(0);
				m_frustumCheckToggle = false;
				//emit index has changed
				emit updateFrustumComboBox(0);
			}
		}
		//Bounding Sphere Wireframe Toggle
		if (event->key() == Qt::Key_3){
			if (m_frustumCheckToggle && !m_wireframeToggle){
				if (!m_wireframeBVToggle){
					enableWireframeBV(1);
					m_wireframeBVToggle = true;
					//emit index has changed
					emit updateWireframeBVComboBox(1);
				}
				else if (m_wireframeBVToggle){
					enableWireframeBV(0);
					m_wireframeBVToggle = false;
					//emit index has changed
					emit updateWireframeBVComboBox(0);
				}
			}
		}
		//Subdivide
		if (event->key() == Qt::Key_4){
			subdivideScenegraph();
		}
		//Render Original Mesh for Subdivision
		if (event->key() == Qt::Key_5){
			if (m_isSubdivision){
				if (!m_originalMeshHEToggle){
					enableWireframeOriginalMeshHE(1);
					m_originalMeshHEToggle = true;
				}
				else if (m_originalMeshHEToggle){
					enableWireframeOriginalMeshHE(0);
					m_originalMeshHEToggle = false;
				}
			}
		}
	}
	m_keysPressed += (Qt::Key)event->key();
}

//User input listener. Removes all the keys released from the list
void OpenGLWin::keyReleaseEvent( QKeyEvent* event )
{
	if(event->isAutoRepeat())
	{
		event->ignore(); 
	}

	m_keysPressed -= (Qt::Key)event->key();
}

void OpenGLWin::closeEvent( QCloseEvent* event )
{
	qDebug() << "Close Event called";
	emit windowDestroyed();

}

void OpenGLWin::initErrorCheck()
{
	m_debugLogger = new QOpenGLDebugLogger;
	connect(m_debugLogger, SIGNAL(messageLogged(QOpenGLDebugMessage)), this, SLOT(onMessageLogged(QOpenGLDebugMessage)), Qt::DirectConnection);

	if (m_debugLogger->initialize()) {
		m_debugLogger->startLogging(QOpenGLDebugLogger::SynchronousLogging);
		m_debugLogger->enableMessages();
	}
}

void OpenGLWin::updateCamera()
{
	//Calculate the new direction vector
	m_direction.Insert(
		cos(m_verticalAngle) * sin(m_horizontalAngle),
		sin(m_verticalAngle),
		cos(m_verticalAngle) * cos(m_horizontalAngle));

	//Calculate the new right vector
	m_right.Insert(
		sin(m_horizontalAngle - 3.14f / 2.0f),
		0,
		cos(m_horizontalAngle - 3.14f / 2.0f));

	//Cross product to find the new up vector
	m_up = m_right.Cross(m_direction);
}

void OpenGLWin::subdivideScenegraph()
{
	//Iterate the whole scenegraph and subdivide every mesh
	if (m_isSubdivision){
		for (int i = 0; i < m_halfEdgeMeshList.size(); i++){
			m_halfEdgeMeshList[i]->subdivide();
		}
		m_subdivisionCounter++;
	}
}

void OpenGLWin::saveHalfEdgeMeshToFile()
{
	//Currently hardcoded to only subdivide the only cube mesh in the scene
	m_subdivisionCubeM->saveMeshToFile(this);
}

void OpenGLWin::initShadowMap()
{
	//Framebuffer, container for textures and depth buffer. Works similar to VAO?
	m_glFunctions->glGenFramebuffers(1, &m_shadowMapFBO);
	m_glFunctions->glGenRenderbuffers(1, &m_shadowmapDepthBuffer);

	// Bind the depth buffer
	m_glFunctions->glBindRenderbuffer(GL_RENDERBUFFER, m_shadowmapDepthBuffer);
	m_glFunctions->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1024, 1024);
	//////////////////////////////////////////////////////////////////////////
	//Render depth to texture
	m_glFunctions->glGenTextures(1, &m_shadowMapTexture);
	//Bind created texture to make it current
	m_glFunctions->glBindTexture(GL_TEXTURE_2D, m_shadowMapTexture);

	//VSM
	m_glFunctions->glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, 1024, 1024, 0, GL_RGBA, GL_FLOAT, 0);
	m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//////////////////////////////////////////////////////////////////////////
	//Bind FBO
	m_glFunctions->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_shadowMapFBO);
	m_glFunctions->glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_shadowmapDepthBuffer);
	//Attach textures to FBO
	m_glFunctions->glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_shadowMapTexture, 0);


	//Check if framebuffer is OK
	GLenum fboStatus = m_glFunctions->glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE){
		qDebug() << "FrameBuffer error: " + fboStatus;
		return;
	}

	//Unbind to render scene
	m_glFunctions->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);


	//////////////////////////////////////////////////////////////////////////
	// Creating the blur FBO
	m_glFunctions->glGenFramebuffers(1, &m_shadowMapBlurFBO);

	m_glFunctions->glGenTextures(1, &m_shadowMapBlurTexture);
	m_glFunctions->glBindTexture(GL_TEXTURE_2D, m_shadowMapBlurTexture);
	m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_glFunctions->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	m_glFunctions->glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	m_glFunctions->glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, 1024, 1024, 0, GL_RGBA, GL_FLOAT, 0);

	//Bind FBO
	m_glFunctions->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_shadowMapBlurFBO);
	//Attach Textures
	m_glFunctions->glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_shadowMapBlurTexture, 0);

	//Check if framebuffer is OK
	fboStatus = m_glFunctions->glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE){
		qDebug() << "FrameBuffer error: " + fboStatus;
		return;
	}

	m_glFunctions->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	//////////////////////////////////////////////////////////////////////////
	// Create vao/vbo for quad
	m_glFunctions->glGenVertexArrays(1, &m_quadVAO);
	m_glFunctions->glBindVertexArray(m_quadVAO);

	//////////////////////////////////////////////////////////////////////////
	//Vertex VBO
	//Create VBO on the GPU to store the vertex data
	m_glFunctions->glGenBuffers(1, &m_quadVBO);
	//Bind VBO to make it current
	m_glFunctions->glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
	//Set the usage type, allocate VRAM and send the vertex data to the GPU
	m_glFunctions->glBufferData(GL_ARRAY_BUFFER, m_quadVertices.size() * sizeof(Vector3), &m_quadVertices[0], GL_STATIC_DRAW);

	//Sets up which shader attribute will received the data. How many elements will form a vertex, type etc
	m_glFunctions->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	//Enable the shader attribute to receive data
	m_glFunctions->glEnableVertexAttribArray(0);
	m_glFunctions->glBindVertexArray(0);
}

void OpenGLWin::blurShadowmap(int pass)
{
	for (int i = 0; i < pass; i++){
		m_glFunctions->glUniform1i(m_shaderModeLocation, 4); //Blur Pass 3
		m_glFunctions->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_shadowMapBlurFBO);
		m_glFunctions->glViewport(0, 0, 1024, 1024); //blur coeff???
		m_glFunctions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_glFunctions->glUniform2f(m_scaleUniform, 1.0 / 1024.0, 0.0); //horizontally

		//Bind shadow map to be blurred
		m_glFunctions->glActiveTexture(GL_TEXTURE1);
		m_glFunctions->glBindTexture(GL_TEXTURE_2D, m_shadowMapTexture);

		//blur horizontally
		m_glFunctions->glBindVertexArray(m_quadVAO);
		m_glFunctions->glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
		m_glFunctions->glBindVertexArray(0);


		//blur vertically
		m_glFunctions->glUniform1i(m_shaderModeLocation, 4); //Blur Pass 3
		m_glFunctions->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_shadowMapFBO);
		m_glFunctions->glViewport(0, 0, 1024, 1024); //blur coeff???
		m_glFunctions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_glFunctions->glUniform2f(m_scaleUniform, 0.0, 1.0 / 1024.0); //vertically

		//Bind shadow map to be blurredblurred
		m_glFunctions->glActiveTexture(GL_TEXTURE1);
		m_glFunctions->glBindTexture(GL_TEXTURE_2D, m_shadowMapBlurTexture);

		m_glFunctions->glBindVertexArray(m_quadVAO);
		m_glFunctions->glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
		m_glFunctions->glBindVertexArray(0);
	}
}

void OpenGLWin::initDeferredShading()
{
	//FBO
	m_glFunctions->glGenFramebuffers(1, &m_deferredShadingFBO);

	//Depth/Stencil Texture
	m_glFunctions->glGenTextures(1, &m_deferredShadingDepthTexture);
	//Bind created texture to make it current
	m_glFunctions->glBindTexture(GL_TEXTURE_2D, m_deferredShadingDepthTexture);
	m_glFunctions->glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 1024, 1024, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);
	m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//Final Texture
	m_glFunctions->glGenTextures(1, &m_deferredShadingFinalTexture);
	//Bind created texture to make it current
	m_glFunctions->glBindTexture(GL_TEXTURE_2D, m_deferredShadingFinalTexture);
	m_glFunctions->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 1024, 0, GL_RGBA, GL_FLOAT, 0);
	m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//////////////////////////////////////////////////////////////////////////
	//Create G-Buffer Textures
	//////////////////////////////////////////////////////////////////////////
	//Diffuse Texture
	m_glFunctions->glGenTextures(1, &m_diffuseTexture);
	//Bind created texture to make it current
	m_glFunctions->glBindTexture(GL_TEXTURE_2D, m_diffuseTexture);
	m_glFunctions->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 1024, 1024, 0, GL_RGBA, GL_FLOAT, 0);
	m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	//Position Texture
	m_glFunctions->glGenTextures(1, &m_positionTexture);
	//Bind created texture to make it current
	m_glFunctions->glBindTexture(GL_TEXTURE_2D, m_positionTexture);
	m_glFunctions->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 1024, 1024, 0, GL_RGBA, GL_FLOAT, 0);
	m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//Normals Texture
	m_glFunctions->glGenTextures(1, &m_normalsTexture);
	//Bind created texture to make it current
	m_glFunctions->glBindTexture(GL_TEXTURE_2D, m_normalsTexture);
	m_glFunctions->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 1024, 1024, 0, GL_RGBA, GL_FLOAT, 0);
	m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//////////////////////////////////////////////////////////////////////////
	//Bind FBO and bind the render buffer to depth attachment
	m_glFunctions->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_deferredShadingFBO);
	m_glFunctions->glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, m_deferredShadingDepthTexture, 0);
	m_glFunctions->glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, m_deferredShadingFinalTexture, 0);
	//Attach G-Buffer textures to FBO
	m_glFunctions->glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_diffuseTexture, 0);
	m_glFunctions->glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, m_positionTexture, 0);
	m_glFunctions->glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, m_normalsTexture, 0);

	//Check if framebuffer is OK
	GLenum fboStatus = m_glFunctions->glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE){
		qDebug() << "FrameBuffer error: " + fboStatus;
		return;
	}

	//Unbind to render scene
	m_glFunctions->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	//////////////////////////////////////////////////////////////////////////
	// Create vao/vbo for quad
	m_glFunctions->glGenVertexArrays(1, &m_quadVAO);
	m_glFunctions->glBindVertexArray(m_quadVAO);

	//////////////////////////////////////////////////////////////////////////
	//Vertex VBO
	//Create VBO on the GPU to store the vertex data
	m_glFunctions->glGenBuffers(1, &m_quadVBO);
	//Bind VBO to make it current
	m_glFunctions->glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
	//Set the usage type, allocate VRAM and send the vertex data to the GPU
	m_glFunctions->glBufferData(GL_ARRAY_BUFFER, m_quadVertices.size() * sizeof(Vector3), &m_quadVertices[0], GL_STATIC_DRAW);

	//Sets up which shader attribute will received the data. How many elements will form a vertex, type etc
	m_glFunctions->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	//Enable the shader attribute to receive data
	m_glFunctions->glEnableVertexAttribArray(0);
	m_glFunctions->glBindVertexArray(0);
}

void OpenGLWin::updateDeferredShadingTextures(float width, float height)
{
	if (m_isDeferredShading){
		//Depth/Stencil Texture
		m_glFunctions->glBindTexture(GL_TEXTURE_2D, m_deferredShadingDepthTexture);
		m_glFunctions->glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);
		m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		//Final Texture
		m_glFunctions->glBindTexture(GL_TEXTURE_2D, m_deferredShadingFinalTexture);
		m_glFunctions->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, 0);
		m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		//Diffuse Texture
		m_glFunctions->glBindTexture(GL_TEXTURE_2D, m_diffuseTexture);
		m_glFunctions->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
		m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		//Position Texture
		//Bind created texture to make it current
		m_glFunctions->glBindTexture(GL_TEXTURE_2D, m_positionTexture);
		m_glFunctions->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
		m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		//Normals Texture
		//Bind created texture to make it current
		m_glFunctions->glBindTexture(GL_TEXTURE_2D, m_normalsTexture);
		m_glFunctions->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
		m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		m_glFunctions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
}

void OpenGLWin::addLightToDsScene(float x, float y, float z, float r, float g, float b, float scaleFactor)
{
	//Transform
	Transform* lightT = new Transform;
	lightT->translate(x, y, z);
	lightT->setScale(scaleFactor);

	//Light
	Light* lightNode = new Light(m_glFunctions);
	//Store the scale as max light radius in light node
	lightNode->setMaxLightRadius(scaleFactor);
	lightNode->useShaderProgram(m_uberShaderProgram->getShaderProgramID());
	lightNode->setLightColor(r, g, b);

	//Attach mesh and light to light transform
	lightT->addChildNode(lightNode);
	lightT->addChildNode(m_dsPointLightM);

	//Push them to their respective lists
	m_dsLightTransformList.push_back(lightT);
	m_dsLightList.push_back(lightNode);
	//Increase Lights counter
	m_dsLightCounter++;
}

void OpenGLWin::drawSkybox()
{
	//Set shader branch to skybox shader
	m_glFunctions->glUniform1i(m_shaderModeLocation, 11);

	//Attach skybox mesh
	m_skyboxT->addChildNode(m_skyboxM);
	//Render
	m_skyboxT->draw(m_frustum, m_projection, m_view);
	//Deattach skybox mesh
	m_skyboxT->removeChildNode(m_skyboxM);
}

void OpenGLWin::dsDrawGBufferTextures()
{
	//Quad render of G-Buffer Textures
	m_glFunctions->glUniform1i(m_shaderModeLocation, 8); //Quad Pass

	//////////////////////////////////////////////////////////////////////////
	//Enable Scissor box to only the clear the color buffer and depth buffer for it
	m_glFunctions->glEnable(GL_SCISSOR_TEST);
	m_glFunctions->glScissor(0, m_height*0.20, m_width *0.15, m_height*0.20);
	m_glFunctions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_glFunctions->glDisable(GL_SCISSOR_TEST);
	m_glFunctions->glViewport(0, m_height*0.20, m_width *0.15, m_height*0.20);

	m_glFunctions->glActiveTexture(GL_TEXTURE1);
	m_glFunctions->glBindTexture(GL_TEXTURE_2D, m_diffuseTexture);

	m_glFunctions->glBindVertexArray(m_quadVAO);
	m_glFunctions->glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
	m_glFunctions->glBindVertexArray(0);
	//////////////////////////////////////////////////////////////////////////
	//Enable Scissor box to only the clear the color buffer and depth buffer for it
	m_glFunctions->glEnable(GL_SCISSOR_TEST);
	m_glFunctions->glScissor(0, 0, m_width *0.15, m_height*0.20);
	m_glFunctions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_glFunctions->glDisable(GL_SCISSOR_TEST);
	m_glFunctions->glViewport(0, 0, m_width *0.15, m_height*0.20);

	m_glFunctions->glActiveTexture(GL_TEXTURE1);
	m_glFunctions->glBindTexture(GL_TEXTURE_2D, m_positionTexture);

	m_glFunctions->glBindVertexArray(m_quadVAO);
	m_glFunctions->glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
	m_glFunctions->glBindVertexArray(0);
	//////////////////////////////////////////////////////////////////////////
	//Enable Scissor box to only the clear the color buffer and depth buffer for it
	m_glFunctions->glEnable(GL_SCISSOR_TEST);
	m_glFunctions->glScissor(m_width *0.15, 0, m_width *0.15, m_height*0.20);
	m_glFunctions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_glFunctions->glDisable(GL_SCISSOR_TEST);
	m_glFunctions->glViewport(m_width *0.15, 0, m_width *0.15, m_height*0.20);

	m_glFunctions->glActiveTexture(GL_TEXTURE1);
	m_glFunctions->glBindTexture(GL_TEXTURE_2D, m_normalsTexture);

	m_glFunctions->glBindVertexArray(m_quadVAO);
	m_glFunctions->glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
	m_glFunctions->glBindVertexArray(0);
	//////////////////////////////////////////////////////////////////////////
}

void OpenGLWin::dsCopyFinalTextureToScreen()
{
	//Blit the final scene texture to the main frame buffer so something shows up on the screen
	m_glFunctions->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	m_glFunctions->glBindFramebuffer(GL_READ_FRAMEBUFFER, m_deferredShadingFBO);
	m_glFunctions->glReadBuffer(GL_COLOR_ATTACHMENT4);

	m_glFunctions->glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void OpenGLWin::dsDrawSceneWithAmbientLight()
{
	//Render fullscreen quad with Diffuse Texture*Ambient Light
	m_glFunctions->glUniform1i(m_shaderModeLocation, 7);

	m_glFunctions->glBindVertexArray(m_quadVAO);
	m_glFunctions->glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
	m_glFunctions->glBindVertexArray(0);
}

void OpenGLWin::dsGeometryPass()
{
	m_glFunctions->glUniform1i(m_shaderModeLocation, 5);

	//Clear color buffer for final texture that will store the rendered light meshes 
	m_glFunctions->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_deferredShadingFBO);
	m_glFunctions->glDrawBuffer(GL_COLOR_ATTACHMENT4);
	m_glFunctions->glClear(GL_COLOR_BUFFER_BIT);

	//Supply glDrawBuffers with an array of buffers to enable MRT functionality. Change state to render to G-Buffer
	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	m_glFunctions->glDrawBuffers(3, DrawBuffers);

	m_glFunctions->glDepthMask(GL_TRUE);
	m_glFunctions->glEnable(GL_DEPTH_TEST);
	m_glFunctions->glViewport(0, 0, m_width, m_height);
	// Clear the buffer with the current clearing color
	m_glFunctions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Render scene to G-Buffer
	m_root->draw(m_frustum, m_projection, m_view);
	m_glFunctions->glDepthMask(GL_FALSE);
}

void OpenGLWin::dsStencilAndLightPass()
{
	//Loop Stencil pass and Light Pass for each light in scene
	for (int i = 0; i < m_dsLightCounter; i++){
		m_glFunctions->glUniform1i(m_shaderModeLocation, 9);
		//Don't want to render to any color attachment. Only want to update stencil buffer
		m_glFunctions->glDrawBuffer(GL_NONE);
		m_glFunctions->glEnable(GL_DEPTH_TEST);
		m_glFunctions->glDisable(GL_CULL_FACE);
		m_glFunctions->glClear(GL_STENCIL_BUFFER_BIT);
		//Set stencil pass to always pass.(Just want to do the stencil operations)
		m_glFunctions->glStencilFunc(GL_ALWAYS, 0, 0);
		//When depth test fails for back face, increase the stencil value
		m_glFunctions->glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
		//When depth test fails for front face, decrease the stencil value
		m_glFunctions->glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

		//Render light to update stencil buffer - Draw is first called from a Light node which updates shader uniforms and then the Light mesh
		m_dsLightTransformList[i]->draw(m_frustum, m_projection, m_view);

		//////////////////////////////////////////////////////////////////////////
		//Light Pass - Render sphere representing point light and sample G-Buffer for texturing
		m_glFunctions->glUniform1i(m_shaderModeLocation, 6);

		m_glFunctions->glDrawBuffer(GL_COLOR_ATTACHMENT4);
		//Only render fragments with stencil value that is not 0. All positive stencil values are inside the light volume
		m_glFunctions->glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
		m_glFunctions->glDisable(GL_DEPTH_TEST);
		m_glFunctions->glEnable(GL_CULL_FACE);
		m_glFunctions->glCullFace(GL_FRONT);

		//Send screen size to shader uniform
		m_glFunctions->glUniform2f(m_screenSizeLocation, m_width, m_height);
		//Send the view matrix to shader uniform so we can transform points to view space for light calculation
		m_glFunctions->glUniformMatrix4fv(m_fragmentViewMatrixLocation, 1, GL_TRUE, &m_view[0][0]);
		m_glFunctions->glEnable(GL_BLEND);
		m_glFunctions->glBlendEquation(GL_FUNC_ADD);
		m_glFunctions->glBlendFunc(GL_ONE, GL_ONE);

		m_glFunctions->glViewport(0, 0, m_width, m_height);

		//Associate G-Buffer textures to samplers
		m_glFunctions->glActiveTexture(GL_TEXTURE2);
		m_glFunctions->glBindTexture(GL_TEXTURE_2D, m_diffuseTexture);
		m_glFunctions->glActiveTexture(GL_TEXTURE3);
		m_glFunctions->glBindTexture(GL_TEXTURE_2D, m_positionTexture);
		m_glFunctions->glActiveTexture(GL_TEXTURE4);
		m_glFunctions->glBindTexture(GL_TEXTURE_2D, m_normalsTexture);

		//Render light - Draw is first called from a Light node which updates shader uniforms and then the Light mesh
		m_dsLightTransformList[i]->draw(m_frustum, m_projection, m_view, 6);
	}
}

void OpenGLWin::drawShapesScene()
{
	m_glFunctions->glUniform1i(m_shaderModeLocation, 10);

	m_playerT->addChildNode(m_pointLight); //Point Light
	m_root->update(m_view);
	m_playerT->removeChildNode(m_pointLight); //Point Light

	//Extracting the view frustum planes
	m_frustum.extractFrustum(m_view, m_projection);

	//Init the counter for rendred objects to the amount of objects created
	m_frustum.shapesRendered = m_shapesAddedToScene;

	m_glFunctions->glViewport(0, 0, m_width, m_height);
	// Clear the screen
	m_glFunctions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_pointLight->draw(m_frustum, m_projection, m_view);
	m_root->draw(m_frustum, m_projection, m_view, 10);
}

void OpenGLWin::drawSolarSystemScene()
{
	//Sun Rotation
	m_sunT->rotate((1 * m_deltaTime / 100), 0, 1, 0);

	//Planet1 Rotation
	m_planet1T->rotate((5 * m_deltaTime / 100), 0, 1, 0);
	m_planet1T->rotate((1 * m_deltaTime / 100), 0, 1, 0, World);

	//Planet2 Rotation
	m_planet2T->rotate((5 * m_deltaTime / 100), 0, 1, 0);
	m_planet2T->rotate((2 * m_deltaTime / 100), 0, 1, 0, World);

	//Planet3 Rotation
	m_planet3T->rotate((10 * m_deltaTime / 100), 0, 1, 0);
	m_planet3T->rotate((3 * m_deltaTime / 100), 0, 1, 0, World);

	//Moon Rotation
	m_moonT->rotate((5 * m_deltaTime / 100), 0, 1, 0);
	m_moonT->rotate((40 * m_deltaTime / 100), 0, 1, 0, World);

	m_glFunctions->glUniform1i(m_shaderModeLocation, 10);

	//Update Scengraph
	m_sunT->addChildNode(m_pointLight); //Sun Light
	m_root->update(m_view);
	m_sunT->removeChildNode(m_pointLight); //Sun Light

	//Extracting the view frustum planes
	m_frustum.extractFrustum(m_view, m_projection);

	//Init the counter for rendred objects to the amount of objects created
	m_frustum.shapesRendered = m_shapesAddedToScene;

	m_glFunctions->glViewport(0, 0, m_width, m_height);
	// Clear the screen
	m_glFunctions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_pointLight->draw(m_frustum, m_projection, m_view);
	m_root->draw(m_frustum, m_projection, m_view, 10);
}

void OpenGLWin::drawSubdivisionScene()
{
	m_glFunctions->glUniform1i(m_shaderModeLocation, 10);

	m_playerT->addChildNode(m_pointLight); //Point Light
	m_root->update(m_view);
	m_playerT->removeChildNode(m_pointLight); //Point Light

	//Extracting the view frustum planes
	m_frustum.extractFrustum(m_view, m_projection);

	//Init the counter for rendred objects to the amount of objects created
	m_frustum.shapesRendered = m_shapesAddedToScene;

	m_glFunctions->glViewport(0, 0, m_width, m_height);
	// Clear the screen
	m_glFunctions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_pointLight->draw(m_frustum, m_projection, m_view);
	m_root->draw(m_frustum, m_projection, m_view, 10);
}

void OpenGLWin::shadowMapPass1()
{
	m_glFunctions->glUniform1i(m_shaderModeLocation, 1);

	//Hang camera at light
	m_playerT->removeChildNode(m_cameraList[0]);
	m_shadowMapPointLightT->addChildNode(m_cameraList[0]);

	//Update model matrices of objects while finding the view matrix
	m_root->update(lightView);

	m_glFunctions->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_shadowMapFBO);
	m_glFunctions->glViewport(0, 0, 1024, 1024);
	// Clear the buffer with the current clearing color
	m_glFunctions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_root->draw(m_frustum, m_projection, m_view, -1, lightView); //Shadow Pass 1
}

void OpenGLWin::shadowMapPass2()
{
	m_glFunctions->glUniform1i(m_shaderModeLocation, 2); //Shadow Pass 2

	m_glFunctions->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);	
	m_glFunctions->glViewport(0, 0, m_width, m_height);
	// Clear the screen
	m_glFunctions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Bind Shadowmap Texture to the texture unit the shadow sampler is associated to
	m_glFunctions->glActiveTexture(GL_TEXTURE1);
	m_glFunctions->glBindTexture(GL_TEXTURE_2D, m_shadowMapTexture);

	//Hang camera on player
	m_shadowMapPointLightT->removeChildNode(m_cameraList[0]);
	m_playerT->addChildNode(m_cameraList[0]);

	//Attach Light and update its data along with the scenegraph
	m_shadowMapPointLightT->addChildNode(m_pointLight); //Point Light
	m_root->update(m_view);
	m_shadowMapPointLightT->removeChildNode(m_pointLight); //Point Light

	//Extracting the view frustum planes
	m_frustum.extractFrustum(m_view, m_projection);

	//Init the counter for rendred objects to the amount of objects created
	m_frustum.shapesRendered = m_shapesAddedToScene;

	m_pointLight->draw(m_frustum, m_projection, m_view);
	m_root->draw(m_frustum, m_projection, m_view, 2, lightView); //Shadow Pass 2

	//////////////////////////////////////////////////////////////////////////
	//Render a mesh where the light is
	m_glFunctions->glUniform1i(m_shaderModeLocation, 13);

	m_shadowMapPointLightT1->addChildNode(m_shadowMapPointLightM);
	m_shadowMapPointLightT1->draw(m_frustum, m_projection, m_view, 13);
	m_shadowMapPointLightT1->removeChildNode(m_shadowMapPointLightM);
}

void OpenGLWin::drawShadowmapTexture()
{
	//////////////////////////////////////////////////////////////////////////
	//Quad render
	m_glFunctions->glUniform1i(m_shaderModeLocation, 3); //Quad Pass

	//Enable Scissor box to only the clear the color buffer and depth buffer for it
	m_glFunctions->glEnable(GL_SCISSOR_TEST);
	m_glFunctions->glScissor(0, 0, m_width *0.15, m_height*0.20);
	m_glFunctions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_glFunctions->glDisable(GL_SCISSOR_TEST);
	m_glFunctions->glViewport(0, 0, m_width *0.15, m_height*0.20);

	m_glFunctions->glActiveTexture(GL_TEXTURE1);
	m_glFunctions->glBindTexture(GL_TEXTURE_2D, m_shadowMapTexture);

	m_glFunctions->glBindVertexArray(m_quadVAO);
	m_glFunctions->glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
	m_glFunctions->glBindVertexArray(0);
	//////////////////////////////////////////////////////////////////////////
}