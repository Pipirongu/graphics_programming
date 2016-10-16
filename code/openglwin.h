#ifndef OPENGLWIN_H
#define OPENGLWIN_H

//#include <QtWidgets/QWidget>
#include "ui_openglwin.h"

//OpenGL
#include <QtOpenGL/QGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLDebugLogger>

//Nodes
#include "Transform.h"
#include "HalfEdgeMesh.h"
#include "Mesh.h"
#include "Light.h"
//Texture
#include "Texture.h"
//Shader
#include "ShaderProgram.h"

//For updating openGL window and user input
#include <QTimer>
//For FPS Counter - prevent updating to frequently
#include <QTime>
//For deltaTime - how long it takes to render a frame
#include <QElapsedTimer>

//For user input
#include <QKeyEvent>

//For time seed
#include <ctime>

#include "ViewFrustumCheck.h"

/// <remarks>
///Used by setSceneType function to set the render window to a specific scene type
/// </remarks>
enum SceneType
{
	Shapes,
	SolarSystem,
	Subdivision,
	Shadowmap,
	Deferred_Shading
};


/// <remarks>
///OpenGL Render Window
/// </remarks>
class OpenGLWin : public QGLWidget
{
	Q_OBJECT

public:
	/// <summary>Constructor</summary>
	/// <param name="format">Properties for openGL</param>
	/// <returns></returns>
	OpenGLWin(const QGLFormat& format);
	/// <summary>Destructor</summary>
	/// <returns></returns>
	~OpenGLWin();

	/// <summary>Sets the field of view of the projection matrix</summary>
	/// <param name="fov">Field of View angle</param>
	/// <returns>void</returns>
	void setFov(float fov);
	/// <summary>Sets the type of render window</summary>
	/// <param name="scene">Enum (Shapes, SolarSystem, Subdivision, Shadowmap, Deferred Shading)</param>
	/// <returns>void</returns>
	void setSceneType(SceneType scene);

protected:
	/// <summary>Initialize Render Window</summary>
	/// <returns>void</returns>
	void initializeGL();
	/// <summary>Called whenever the render window needs to update or when updateGL is called</summary>
	/// <returns>void</returns>
	void paintGL();
	/// <summary>Called when the window has resized, also called once when the widget is created</summary>
	/// <param name="w">width of the window</param>
	/// <param name="h">height</param>
	/// <returns>void</returns>
	void resizeGL(int w, int h);

signals:
	/// <summary>Signal: Emitted when closeEvent is called. To tell the GUI to reset and disable parts of the GUI</summary>
	/// <returns>void</returns>
	void windowDestroyed();
	/// <summary>Signal: Emitted to reset the scale slider in the GUI</summary>
	/// <returns>void</returns>
	void resetScaleSlider();
	/// <summary>Signal:: Emitted to update the GUI display with information (fps, objects added, objects rendered)</summary>
	/// <param name="text">Text to put in the GUI</param>
	/// <returns>void</returns>
	void setGUIText(QString text);
	
	/// <summary>Signal: Emitted to enable/disable the state of the comboBox for wireframe bounding sphere</summary>
	/// <param name="toggle">0 or 1</param>
	/// <returns>void</returns>
	void enableWireframeBVComboBox(int toggle);

	/// <summary>Signal: Emitted to change current index of the comboBox for wireframe</summary>
	/// <param name="toggle">0 or 1</param>
	/// <returns>void</returns>
	void updateWireframeComboBox(int toggle);
	/// <summary>Signal: Emitted to change current index of the comboBox for view frustum culling</summary>
	/// <param name="toggle">0 or 1</param>
	/// <returns>void</returns>
	void updateFrustumComboBox(int toggle);
	/// <summary>Signal: Emitted to change current index of the comboBox for wireframe bounding sphere</summary>
	/// <param name="toggle">0 or 1</param>
	/// <returns>void</returns>
	void updateWireframeBVComboBox(int toggle);
	/// <summary>Signal: Emitted to change current index of the comboBox for wireframe original mesh</summary>
	/// <param name="toggle">0 or 1</param>
	/// <returns>void</returns>
	void updateWireframeOriginalMeshHEComboBox(int toggle);

private slots:
	/// <summary>Called when an openGL error occurs. Prints the error message and waits for user input</summary>
	/// <param name="message">the error message</param>
	/// <returns>void</returns>
	void onMessageLogged(QOpenGLDebugMessage message);
	/// <summary>Custom mouseEvent which is polled in the render loop. This is to avoid the stuttering QT's input event handler has</summary>
	/// <returns>void</returns>
	void mousePress();
	/// <summary>Custom keyPressEvent which is polled in the render loop. This is to avoid the stuttering QT's input event handler has</summary>
	/// <returns>void</returns>
	void keyPress();
	/// <summary>Deletes the latest shape added in the shapes scene</summary>
	/// <returns>void</returns>
	void deleteLatestShape();

	/// <summary>Enable/Disable wireframe mode for all meshes</summary>
	/// <param name="toggle">0 or 1</param>
	/// <returns>void</returns>
	void enableWireframe(int toggle);
	/// <summary>Enable/Disable View Frustum Culling for all meshes</summary>
	/// <param name="toggle">0 or 1</param>
	/// <returns>void</returns>
	void enableFrustumCulling(int toggle);
	/// <summary>Enable/Disable rendering of wireframe bounding sphere</summary>
	/// <param name="toggle">0 or 1</param>
	/// <returns>void</returns>
	void enableWireframeBV(int toggle);
	/// <summary>Enable/Disable rendering of wireframe original mesh</summary>
	/// <param name="toggle">0 or 1</param>
	/// <returns>void</returns>
	void enableWireframeOriginalMeshHE(int toggle);

	/// <summary>Adds a random shape(pyramid, cube or sphere) with random position to the scene</summary>
	/// <returns>void</returns>
	void addRandomShapeToScene();
	/// <summary>Scales the latest added shape</summary>
	/// <param name="factor">Scale Factor</param>
	/// <returns>void</returns>
	void setScaleLatestShape(int factor);

	/// <summary>If the scenegraph is of type Subdivision this function will subdivide all the meshes in the scenegraph</summary>
	/// <returns>void</returns>
	void subdivideScenegraph();
	/// <summary>Calls function for a halfedgemesh to save the mesh to obj file</summary>
	/// <returns>void</returns>
	void saveHalfEdgeMeshToFile();

private:
	Ui::OpenGLWinClass ui;

	//Handles for shader uniforms
	GLuint m_shaderModeLocation; //Uniform to select a shader branch from the uber-shader
	GLuint m_textureSamplerLocation;
	GLuint m_shadowMapSamplerLocation;
	GLuint m_scaleUniform;
	GLuint m_screenSizeLocation;
	GLuint m_fragmentViewMatrixLocation;
	GLuint m_maxLightRadiusLocation;

	//G-Buffer sampler handles
	GLuint m_diffuseSamplerLocation;
	GLuint m_positionSamplerLocation;
	GLuint m_normalsSamplerLocation;


	//FBO and its Textures for Deferred Shading
	GLuint m_deferredShadingFBO;
	GLuint m_deferredShadingDepthTexture; //For depth and stencil
	GLuint m_deferredShadingFinalTexture; //Will contain the rendered light meshes. Needed for to access depth/stencil buffer of FBO

	//G-Buffer textures
	GLuint m_diffuseTexture;
	GLuint m_positionTexture;
	GLuint m_normalsTexture;

	//Fullscreen Quad
	GLuint m_quadVAO;
	GLuint m_quadVBO;

	//Shadow map FBOs
	GLuint m_shadowMapFBO;
	GLuint m_shadowMapBlurFBO;

	//Shadowmap Textures and depth buffer
	GLuint m_shadowMapTexture;
	GLuint m_shadowMapBlurTexture;
	GLuint m_shadowmapDepthBuffer; //need to bind a buffer to the depth attachment of fbo. Or no shadows.

	//Holds the 6 planes representing the view frustum. Has fuctions to extract the planes and test sphere to plane intersection
	ViewFrustumCheck m_frustum;

	//Flags to help decide arguments for Functions which toggles on/off features
	bool m_wireframeToggle;
	bool m_frustumCheckToggle;
	bool m_wireframeBVToggle;
	bool m_originalMeshHEToggle;

	//Flag to keep track if the render window is in focus to prevent the input to lock and player automatically moves
	bool m_isFocus;

	//Flags to check what kind of rendering scene this is
	bool m_isShapes;
	bool m_isSolarSystem;
	bool m_isSubdivision;
	bool m_isShadowmap;
	bool m_isDeferredShading;

	//Mouse input flag - to bypass Qt's event listeners
	bool m_isMousePress;

	//Counter for the amount of shapes in the scene
	unsigned int m_shapesAddedToScene;
	//Counter for the amount of subdivision
	unsigned int m_subdivisionCounter;
	//Light Counter for deferred shading scene
	unsigned int m_dsLightCounter;

	//Vectors for camera control
	Vector3 m_position; //Stores the position
	Vector3 m_direction; //Stores the direction
	Vector3 m_right; //The right vector
	Vector3 m_up; //Up vector will be calculated using direction and right vectors

	//Horizontal Angle( left and right ) 
	float m_horizontalAngle;
	//Vertical Angle( left and right ) 
	float m_verticalAngle;

	///////////////////////////////////
	//Used to call native openGL functions
	QOpenGLFunctions_3_3_Core* m_glFunctions;

	//Stores keys pressed by user - to bypass Qt's event listeners
	QSet<Qt::Key> m_keysPressed;

	//Sensitivity of keyboard input 
	float m_keyboardSensitivity;
	//Sensitivity of mouse input
	float m_mouseSensitivity;

	//openGL error logger
	QOpenGLDebugLogger* m_debugLogger;

	//Width of the GL Window
	int m_width;
	//Height of the GL Window
	int m_height;
	//Field of View
	float m_fov;

	//Near plane
	float m_near;
	//Far plane
	float m_far;

	//Stores the string for FPS to be displayed in window title. Updates with a timer
	QString m_fpsTimeWindowTitle;
	//Stores the string for FPS to be displayed in QT Gui. Updates with a timer
	QString m_fpsTimeGUI;
	//Used to update FPS time. Prevents it to display new values to frequently
	QTime m_fpsTimer;
	//Timer used to update the render loop
	QTimer m_timer;
	//Timer to update Color of deferred shading Lights(Too prevent spamming)
	QTime m_dsColorTimer;
	//deltaTime timer - time it takes to render a frame. With high precision
	QElapsedTimer m_elapsedTimer;
	//Stores the deltaTime - time it takes to render a frame (Used to make transformation frame independant)
	float m_deltaTime;

	//////////////////////////////////////////////////////////////////////////
	//Root Node
	Transform* m_root;

	//Skybox
	Transform* m_skyboxT;
	Mesh* m_skyboxM;
 	
	//The Player Transform
	Transform* m_playerT;

	//////////////////////////////////////////////////////////////////////////
	//Shape Meshes
	Mesh* m_pyramidM;
	Mesh* m_cubeM;
	Mesh* m_sphereM;

	//Solar System Transforms
	//Sun Transform
	Transform* m_sunT;
	//Planet1 Transform
	Transform* m_planet1T;
	//Planet2 Transform
	Transform* m_planet2T;
	//Planet3 Transform
	Transform* m_planet3T;
	//Moon Transform
	Transform* m_moonT;

	//Solar System Meshes
	//Sun Mesh
	Mesh* m_sunM;
	//Planet1 Mesh
	Mesh* m_planet1M;
	//Planet2 Mesh
	Mesh* m_planet2M;
	//Planet3 Mesh
	Mesh* m_planet3M;
	//Moon Mesh
	Mesh* m_moonM;

	//HalfEdge Transform and Mesh
	Transform* m_subdivisionCubeT;
	HalfEdgeMesh* m_subdivisionCubeM;

	//Shadowmap Transform
	Transform* m_shadowmapT;
	//Shadowmap Mesh
	Mesh* m_shadowmapM;

	//Deferred Shading Transform
	Transform* m_deferredShadingT;
	//Deferred Shading Mesh
	Mesh* m_deferredShadingM;
	//Mesh for Point Light - Deferred Shading
	Mesh* m_dsPointLightM;

	//Shadow Map Light transforms
	Transform* m_shadowMapPointLightT;
	//A mesh is attached to this just for the purpose of rendering at mesh where the light is located
	Transform* m_shadowMapPointLightT1;
	//Shadow Map Light Mesh(Rendered where the light is)
	Mesh* m_shadowMapPointLightM;


	//Sun Light
	Light* m_pointLight;

	//////////////////////////////////////////////////////////////////////////
	//Skybox
	Texture* m_skyboxTexture;

	//Shape Textures
	Texture* m_pyramidTexture;
	Texture* m_cubeTexture;
	Texture* m_sphereTexture;

	//SolarSystem Textures
	Texture* m_sunTexture;
	Texture* m_planet1Texture;
	Texture* m_planet2Texture;
	Texture* m_planet3Texture;
	Texture* m_moonTexture;

	//Subdivision Textures
	Texture* m_subdivisionCubeTexture;

	//Shadowmap Textures
	Texture* m_shadowmapTexture;

	//Deferred Shading Textures
	Texture* m_deferredShadingTexture;

	//////////////////////////////////////////////////////////////////////////
	//Holds the transform for the latest shape added. For scaling the latest added object
	Transform* m_latestShapeAdded;

	//Holds the Transforms so they call be deallocated easier
	std::vector<Transform*> m_transformList;
	//Holds the Meshes so they call be deallocated easier
	std::vector<Mesh*> m_meshList;

	//Holds the HalfEdge Meshes so they call be deallocated easier
	std::vector<HalfEdgeMesh*> m_halfEdgeMeshList;

	//Holds all the textures to deallocate easier
	std::vector<Texture*> m_textureList;
	//Holds all the Shader Programs to deallocate easier
	std::vector<ShaderProgram*> m_shaderProgramList;

	//Holds all the cameras 
	std::vector<Transform*> m_cameraList;

	//List of Light transforms for deferred shading scene
	std::vector<Transform*> m_dsLightTransformList;
	//List of Lights for deferred shading scene
	std::vector<Light*> m_dsLightList;

	//Vertex list for fullscreen Quad
	std::vector<Vector3> m_quadVertices;

	//////////////////////////////////////////////////////////////////////////
	//Light View Matrix
	Matrix44 lightView;
	//View Matrix
	Matrix44 m_view;
	//Projection Matrix
	Matrix44 m_projection;

	//Shaders
	ShaderProgram* m_uberShaderProgram;

	//////////////////////////////////////////////////////////////////////////
	/// <summary>Listener to check if the window is focused or not - to prevent the player from auto moving</summary>
	/// <param name="event">Event</param>
	/// <returns>bool</returns>
	bool event(QEvent* event);
	/// <summary>Mouse input listener. Enables when user presses the mouse. Automatically reads input if setMouseTracking is enabled</summary>
	/// <param name="event">Event</param>
	/// <returns>void</returns>
	void mouseMoveEvent(QMouseEvent* event);
	/// <summary>Mouse input listener. Enables when user releases a mouse button</summary>
	/// <param name="event">Event</param>
	/// <returns>void</returns>
	void mouseReleaseEvent(QMouseEvent* event);
	/// <summary>Key Event Listener when user presses keyboard buttons</summary>
	/// <param name="event">Event</param>
	/// <returns>void</returns>
	void keyPressEvent(QKeyEvent* event);
	/// <summary>Key Event Listener when user releases keyboard buttons</summary>
	/// <param name="event">Event</param>
	/// <returns>void</returns>
	void keyReleaseEvent(QKeyEvent* event);
	/// <summary>Close Event Listener</summary>
	/// <param name="event">Event</param>
	/// <returns>void</returns>
	void closeEvent(QCloseEvent* event);

	/// <summary>Initialize function to start listening for openGL error messages</summary>
	/// <returns>void</returns>
	void initErrorCheck();
	/// <summary>Updates the camera vectors, used by mousePress function</summary>
	/// <returns>void</returns>
	void updateCamera();

	/// <summary>Renderes the skybox(Should be called last in rendering pipeline because it's relies on the Depth Test)</summary>
	/// <returns>void</returns>
	void drawSkybox();
	/// <summary>Sets up FBOs for shadow map</summary>
	/// <returns>void</returns>
	void initShadowMap();
	/// <summary>Blurs the shadow map(Should be called after shadow pass 1)</summary>
	/// <param name="pass">The amount of blur iterations</param>
	/// <returns>void</returns>
	void blurShadowmap(int pass);
	/// <summary>Sets up FBO for deferred shading</summary>
	/// <returns>void</returns>
	void initDeferredShading();
	/// <summary>Updates the texture dimensions for deferred shading(Should be called whenever the window resizes)</summary>
	/// <param name="width">width</param>
	/// <param name="height">height</param>
	/// <returns>void</returns>
	void updateDeferredShadingTextures(float width, float height);

	/// <summary>Creates a Light and a Light Mesh for deferred shading. Mesh is scaled and the scale factor is saved in the Light as max light radius</summary>
	/// <param name="x">Translate in X</param>
	/// <param name="y">Translate in Y</param>
	/// <param name="z">Translate in Z</param>
	/// <param name="r">Color Red Channel</param>
	/// <param name="g">Color Green Channel</param>
	/// <param name="b">Color Blue Channel</param>
	/// <param name="scaleFactor">Scale factor to scale the mesh and to set the max light radius</param>
	/// <returns>void</returns>
	void addLightToDsScene(float x, float y, float z, float r, float g, float b, float scaleFactor);

	/// <summary>Deferred Shading: Geometry Pass, Render mesh information to textures</summary>
	/// <returns>void</returns>
	void dsGeometryPass();
	/// <summary>Deferred Shading: Stencil and Light pass, Renders only the pixels that should be lit. This is repeated for each light</summary>
	/// <returns>void</returns>
	void dsStencilAndLightPass();
	/// <summary>Deferred Shading: Render scene with ambient light. It is blended with the light meshes</summary>
	/// <returns>void</returns>
	void dsDrawSceneWithAmbientLight();
	/// <summary>Deferred Shading: Copy the final scene from the FBO to the main frame buffer(This pass is needed because we need use the FBO's depth buffer when rendering the light)</summary>
	/// <returns>void</returns>
	void dsCopyFinalTextureToScreen();
	/// <summary>Deferred Shading: Draw the G-Buffer textures as small minimaps</summary>
	/// <returns>void</returns>
	void dsDrawGBufferTextures();

	/// <summary>Renders the shapes Scene</summary>
	/// <returns>void</returns>
	void drawShapesScene();
	/// <summary>Renders the solarsystem Scene</summary>
	/// <returns>void</returns>
	void drawSolarSystemScene();
	/// <summary>Renders the subdivision Scene</summary>
	/// <returns>void</returns>
	void drawSubdivisionScene();

	/// <summary>Shadowmap: Pass 1 where the camera is attached to the light and the scene is rendered to a texture</summary>
	/// <returns>void</returns>
	void shadowMapPass1();
	/// <summary>Shadowmap: Pass 2 where the camera is reattached to the old position
	/// and the scene is rendered while comparing depth with depth from shadowmap to apply shadows</summary>
	/// <returns>void</returns>
	void shadowMapPass2();
	/// <summary>Shadowmap: Draw the shadowmap texture as small minimap</summary>
	/// <returns>void</returns>
	void drawShadowmapTexture();
};

#endif // OPENGLWIN_H
