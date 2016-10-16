#include "settingswin.h"

SettingsWin::SettingsWin(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//Sets the cursor when it's over the textEdit to arrow cursor
	ui.plainTextEdit->viewport()->setCursor(Qt::ArrowCursor);
	ui.plainTextEdit->setStyleSheet("background:transparent;");

	ui.plainTextEdit_2->viewport()->setCursor(Qt::ArrowCursor);
	ui.plainTextEdit_2->setStyleSheet("background:transparent;");

	m_renderWindow = NULL;
	move(350, 300);

	//Shape Scene
	connect(ui.pushButton_2, SIGNAL(clicked()), this, SLOT(createRandomShapesScene()));
	//Solarsystem Scene
	connect(ui.pushButton_3, SIGNAL(clicked()), this, SLOT(createSolarSystemScene()));
	//Subdivision Scene
	connect(ui.pushButton_6, SIGNAL(clicked()), this, SLOT(createSubdivisionScene()));
	//Shadowmap Scene
	connect(ui.pushButton_9, SIGNAL(clicked()), this, SLOT(createShadowmapScene()));
	//Deferred Shading Scene
	connect(ui.pushButton_10, SIGNAL(clicked()), this, SLOT(createDeferredShadingScene()));
}

SettingsWin::~SettingsWin()
{
}

void SettingsWin::setFov()
{
	float fov = (float)ui.horizontalSlider->value();
	if (m_renderWindow != NULL){
		m_renderWindow->setFov(fov);
	}
}

void SettingsWin::createRandomShapesScene()
{
	if (m_renderWindow == NULL){

		qDebug() << "Creating Render Window";
		// Specify an OpenGL 3.3 format using the Core profile.
		// That is, no old-school fixed pipeline functionality
		QGLFormat glFormat;
		glFormat.setVersion(3, 3);
		glFormat.setProfile(QGLFormat::CoreProfile); // Requires >=Qt-4.8.0
		glFormat.setSampleBuffers(true); //MSAA
		glFormat.setSamples(16);
		//Vsync off
		glFormat.setSwapInterval(0);

		m_renderWindow = new OpenGLWin(glFormat);
		m_renderWindow->setSceneType(Shapes);
		m_renderWindow->show();

		//OpenGL Version
		QString versionString(QLatin1String(reinterpret_cast<const char*>(glGetString(GL_VERSION))));
		qDebug() << "OpenGL Version:" << versionString << endl;


		/*
		Sets some GUI buttons and sliders to true
		*/

		//Field of View
		ui.horizontalSlider->setEnabled(true);
		//Field of View
		ui.spinBox->setEnabled(true);
		//Field of View
		ui.pushButton->setEnabled(true);
		//Scale Latest Shape
		ui.horizontalSlider_2->setEnabled(true);
		//Scale Latest Shape
		ui.spinBox_2->setEnabled(true);

		//Wireframe Mode
		ui.comboBox->setEnabled(true);
		//Viewfrustum Culling
		ui.comboBox_3->setEnabled(true);
		//Wireframe Bounding Volume
		ui.comboBox_4->setEnabled(true);

		//Delete Latest Random Shape from scene
		ui.pushButton_4->setEnabled(true);
		//Add Random Shape to Scene
		ui.pushButton_5->setEnabled(true);

		/*
		Sets some GUI buttons and sliders to false
		*/

		//Shape Scene button
		ui.pushButton_2->setEnabled(false);
		//Solar System button
		ui.pushButton_3->setEnabled(false);
		//Subdivision button
		ui.pushButton_6->setEnabled(false);
		//Shadowmap button
		ui.pushButton_9->setEnabled(false);
		//Deferred Shading button
		ui.pushButton_10->setEnabled(false);

		//////////////////////////////////////////////////////////////////////////

		//Field of View button
		connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(setFov()));

		/*
		When index of comboboxes change then call the slots
		*/

		//Wireframe mode
		connect(ui.comboBox, SIGNAL(currentIndexChanged(int)), m_renderWindow, SLOT(enableWireframe(int)));
		//Viewfrustum Culling
		connect(ui.comboBox_3, SIGNAL(currentIndexChanged(int)), m_renderWindow, SLOT(enableFrustumCulling(int)));
		//Wireframe Bounding Volume
		connect(ui.comboBox_4, SIGNAL(currentIndexChanged(int)), m_renderWindow, SLOT(enableWireframeBV(int)));

		//Toggles enabled state for Wireframe Bounding volume
		connect(m_renderWindow, SIGNAL(enableWireframeBVComboBox(int)), this, SLOT(enableWireframeBVComboBox(int)));

		//Scale latest shape
		connect(ui.horizontalSlider_2, SIGNAL(valueChanged(int)), m_renderWindow, SLOT(setScaleLatestShape(int)));

		//Delete Latest Random Shape from scene
		connect(ui.pushButton_4, SIGNAL(clicked()), m_renderWindow, SLOT(deleteLatestShape()));
		//Add Random Shape to Scene
		connect(ui.pushButton_5, SIGNAL(clicked()), m_renderWindow, SLOT(addRandomShapeToScene()));

		//Disable parts of GUI when signal is emitted
		connect(m_renderWindow, SIGNAL(windowDestroyed()), this, SLOT(disableGUI()));
		//Reset scale slider to default
		connect(m_renderWindow, SIGNAL(resetScaleSlider()), this, SLOT(setScaleSliderToDefault()));

		//Updates the FPS display
		connect(m_renderWindow, SIGNAL(setGUIText(QString)), ui.plainTextEdit_2, SLOT(setPlainText(QString)));

		/*
		Change the comboboxes when signal is emitted
		*/

		//Wireframe mode
		connect(m_renderWindow, SIGNAL(updateWireframeComboBox(int)), ui.comboBox, SLOT(setCurrentIndex(int)));
		//Viewfrustum Culling
		connect(m_renderWindow, SIGNAL(updateFrustumComboBox(int)), ui.comboBox_3, SLOT(setCurrentIndex(int)));
		//Wireframe Bounding Volume
		connect(m_renderWindow, SIGNAL(updateWireframeBVComboBox(int)), ui.comboBox_4, SLOT(setCurrentIndex(int)));
	}
	else{
		qDebug() << "Render Window already exists";
	}
}

void SettingsWin::createSolarSystemScene()
{
	if (m_renderWindow == NULL){

		qDebug() << "Creating Render Window";
		// Specify an OpenGL 3.3 format using the Core profile.
		// That is, no old-school fixed pipeline functionality
		QGLFormat glFormat;
		glFormat.setVersion(3, 3);
		glFormat.setProfile(QGLFormat::CoreProfile); // Requires >=Qt-4.8.0
		glFormat.setSampleBuffers(true); //MSAA
		glFormat.setSamples(16);
		//Vsync off
		glFormat.setSwapInterval(0);

		m_renderWindow = new OpenGLWin(glFormat);
		m_renderWindow->setSceneType(SolarSystem);
		m_renderWindow->show();

		//OpenGL Version
		QString versionString(QLatin1String(reinterpret_cast<const char*>(glGetString(GL_VERSION))));
		qDebug() << "OpenGL Version:" << versionString << endl;

		/*
		Sets some GUI buttons and sliders to true
		*/

		//Field of View
		ui.horizontalSlider->setEnabled(true);
		//Field of View
		ui.spinBox->setEnabled(true);
		//Field of View
		ui.pushButton->setEnabled(true);

		//Wireframe Mode
		ui.comboBox->setEnabled(true);
		//Viewfrustum Culling
		ui.comboBox_3->setEnabled(true);
		//Wireframe Bounding Volume
 		ui.comboBox_4->setEnabled(true);
		//Deferred Shading button
		ui.pushButton_10->setEnabled(false);

		/*
		Sets some GUI buttons and sliders to false
		*/

		//Shape Scene button
		ui.pushButton_2->setEnabled(false);
		//Solar System button
		ui.pushButton_3->setEnabled(false);
		//Subdivision button
		ui.pushButton_6->setEnabled(false);
		//Shadowmap button
		ui.pushButton_9->setEnabled(false);

		//////////////////////////////////////////////////////////////////////////

		//Field of View button
		connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(setFov()));

		/*
		When index of comboboxes change then call the slots
		*/

		//Wireframe mode
		connect(ui.comboBox, SIGNAL(currentIndexChanged(int)), m_renderWindow, SLOT(enableWireframe(int)));
		//Viewfrustum Culling
		connect(ui.comboBox_3, SIGNAL(currentIndexChanged(int)), m_renderWindow, SLOT(enableFrustumCulling(int)));
		//Wireframe Bounding Volume
		connect(ui.comboBox_4, SIGNAL(currentIndexChanged(int)), m_renderWindow, SLOT(enableWireframeBV(int)));

		//Toggles enabled state for Wireframe Bounding volume
		connect(m_renderWindow, SIGNAL(enableWireframeBVComboBox(int)), this, SLOT(enableWireframeBVComboBox(int)));

		//Disable parts of GUI when signal is emitted
		connect(m_renderWindow, SIGNAL(windowDestroyed()), this, SLOT(disableGUI()));

		//Updates the FPS display
		connect(m_renderWindow, SIGNAL(setGUIText(QString)), ui.plainTextEdit_2, SLOT(setPlainText(QString)));

		/*
		Change the comboboxes when signal is emitted
		*/

		//Wireframe mode
		connect(m_renderWindow, SIGNAL(updateWireframeComboBox(int)), ui.comboBox, SLOT(setCurrentIndex(int)));
		//Viewfrustum Culling
		connect(m_renderWindow, SIGNAL(updateFrustumComboBox(int)), ui.comboBox_3, SLOT(setCurrentIndex(int)));
		//Wireframe Bounding Volume
		connect(m_renderWindow, SIGNAL(updateWireframeBVComboBox(int)), ui.comboBox_4, SLOT(setCurrentIndex(int)));
	}
	else{
		qDebug() << "Render Window already exists";
	}
}

void SettingsWin::createSubdivisionScene()
{
	if (m_renderWindow == NULL){

		qDebug() << "Creating Render Window";
		// Specify an OpenGL 3.3 format using the Core profile.
		// That is, no old-school fixed pipeline functionality
		QGLFormat glFormat;
		glFormat.setVersion(3, 3);
		glFormat.setProfile(QGLFormat::CoreProfile); // Requires >=Qt-4.8.0
		glFormat.setSampleBuffers(true); //MSAA
		glFormat.setSamples(16);
		//Vsync off
		glFormat.setSwapInterval(0);

		m_renderWindow = new OpenGLWin(glFormat);
		m_renderWindow->setSceneType(Subdivision);
		m_renderWindow->show();

		//OpenGL Version
		QString versionString(QLatin1String(reinterpret_cast<const char*>(glGetString(GL_VERSION))));
		qDebug() << "OpenGL Version:" << versionString << endl;


		/*
		Sets some GUI buttons and sliders to true
		*/

		//Field of View
		ui.horizontalSlider->setEnabled(true);
		//Field of View
		ui.spinBox->setEnabled(true);
		//Field of View
		ui.pushButton->setEnabled(true);

		//Subdivide
		ui.pushButton_7->setEnabled(true);
		//Save mesh to file
		ui.pushButton_8->setEnabled(true);

		//Wireframe Mode
		ui.comboBox->setEnabled(true);
		//Viewfrustum Culling
		ui.comboBox_3->setEnabled(true);
		//Wireframe Bounding Volume
		ui.comboBox_4->setEnabled(true);
		//Wireframe Original Mesh
		ui.comboBox_5->setEnabled(true);
		//Deferred Shading button
		ui.pushButton_10->setEnabled(false);

		/*
		Sets some GUI buttons and sliders to false
		*/

		//Shape Scene button
		ui.pushButton_2->setEnabled(false);
		//Solar System button
		ui.pushButton_3->setEnabled(false);
		//Subdivision button
		ui.pushButton_6->setEnabled(false);
		//Shadowmap button
		ui.pushButton_9->setEnabled(false);

		//////////////////////////////////////////////////////////////////////////

		//Field of View button
		connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(setFov()));

		/*
		When index of comboboxes change then call the slots
		*/

		//Wireframe mode
		connect(ui.comboBox, SIGNAL(currentIndexChanged(int)), m_renderWindow, SLOT(enableWireframe(int)));
		//Viewfrustum Culling
		connect(ui.comboBox_3, SIGNAL(currentIndexChanged(int)), m_renderWindow, SLOT(enableFrustumCulling(int)));
		//Wireframe Bounding Volume
		connect(ui.comboBox_4, SIGNAL(currentIndexChanged(int)), m_renderWindow, SLOT(enableWireframeBV(int)));
		//WIreframe Original Mesh
		connect(ui.comboBox_5, SIGNAL(currentIndexChanged(int)), m_renderWindow, SLOT(enableWireframeOriginalMeshHE(int)));

		//Toggles enabled state for Wireframe Bounding volume
		connect(m_renderWindow, SIGNAL(enableWireframeBVComboBox(int)), this, SLOT(enableWireframeBVComboBox(int)));

		//Subdivide
		connect(ui.pushButton_7, SIGNAL(clicked()), m_renderWindow, SLOT(subdivideScenegraph()));
		//Save mesh to file
		connect(ui.pushButton_8, SIGNAL(clicked()), m_renderWindow, SLOT(saveHalfEdgeMeshToFile()));

		//Disable parts of GUI when signal is emitted
		connect(m_renderWindow, SIGNAL(windowDestroyed()), this, SLOT(disableGUI()));

		//Updates the FPS display
		connect(m_renderWindow, SIGNAL(setGUIText(QString)), ui.plainTextEdit_2, SLOT(setPlainText(QString)));

		/*
		Change the comboboxes when signal is emitted
		*/

		//Wireframe mode
		connect(m_renderWindow, SIGNAL(updateWireframeComboBox(int)), ui.comboBox, SLOT(setCurrentIndex(int)));
		//Viewfrustum Culling
		connect(m_renderWindow, SIGNAL(updateFrustumComboBox(int)), ui.comboBox_3, SLOT(setCurrentIndex(int)));
		//Wireframe Bounding Volume
		connect(m_renderWindow, SIGNAL(updateWireframeBVComboBox(int)), ui.comboBox_4, SLOT(setCurrentIndex(int)));
		//Wireframe Original Mesh
		connect(m_renderWindow, SIGNAL(updateWireframeOriginalMeshHEComboBox(int)), ui.comboBox_5, SLOT(setCurrentIndex(int)));
	}
	else{
		qDebug() << "Render Window already exists";
	}
}

void SettingsWin::createShadowmapScene()
{
	if (m_renderWindow == NULL){

		qDebug() << "Creating Render Window";
		// Specify an OpenGL 3.3 format using the Core profile.
		// That is, no old-school fixed pipeline functionality
		QGLFormat glFormat;
		glFormat.setVersion(3, 3);
		glFormat.setProfile(QGLFormat::CoreProfile); // Requires >=Qt-4.8.0
		glFormat.setSampleBuffers(true); //MSAA
		glFormat.setSamples(16);
		//Vsync off
		glFormat.setSwapInterval(0);

		m_renderWindow = new OpenGLWin(glFormat);
		m_renderWindow->setSceneType(Shadowmap);
		m_renderWindow->show();

		//OpenGL Version
		QString versionString(QLatin1String(reinterpret_cast<const char*>(glGetString(GL_VERSION))));
		qDebug() << "OpenGL Version:" << versionString << endl;


		/*
		Sets some GUI buttons and sliders to true
		*/

		//Field of View
		ui.horizontalSlider->setEnabled(true);
		//Field of View
		ui.spinBox->setEnabled(true);
		//Field of View
		ui.pushButton->setEnabled(true);

		//Wireframe Mode
		ui.comboBox->setEnabled(true);
		//Viewfrustum Culling
		ui.comboBox_3->setEnabled(true);
		//Wireframe Bounding Volume
		ui.comboBox_4->setEnabled(true);

		/*
		Sets some GUI buttons and sliders to false
		*/

		//Shape Scene button
		ui.pushButton_2->setEnabled(false);
		//Solar System button
		ui.pushButton_3->setEnabled(false);
		//Subdivision button
		ui.pushButton_6->setEnabled(false);
		//Shadowmap button
		ui.pushButton_9->setEnabled(false);
		//Deferred Shading button
		ui.pushButton_10->setEnabled(false);

		//////////////////////////////////////////////////////////////////////////

		//Field of View button
		connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(setFov()));

		/*
		When index of comboboxes change then call the slots
		*/

		//Wireframe mode
		connect(ui.comboBox, SIGNAL(currentIndexChanged(int)), m_renderWindow, SLOT(enableWireframe(int)));
		//Viewfrustum Culling
		connect(ui.comboBox_3, SIGNAL(currentIndexChanged(int)), m_renderWindow, SLOT(enableFrustumCulling(int)));
		//Wireframe Bounding Volume
		connect(ui.comboBox_4, SIGNAL(currentIndexChanged(int)), m_renderWindow, SLOT(enableWireframeBV(int)));

		//Toggles enabled state for Wireframe Bounding volume
		connect(m_renderWindow, SIGNAL(enableWireframeBVComboBox(int)), this, SLOT(enableWireframeBVComboBox(int)));

		//Disable parts of GUI when signal is emitted
		connect(m_renderWindow, SIGNAL(windowDestroyed()), this, SLOT(disableGUI()));

		//Updates the FPS display
		connect(m_renderWindow, SIGNAL(setGUIText(QString)), ui.plainTextEdit_2, SLOT(setPlainText(QString)));

		/*
		Change the comboboxes when signal is emitted
		*/

		//Wireframe mode
		connect(m_renderWindow, SIGNAL(updateWireframeComboBox(int)), ui.comboBox, SLOT(setCurrentIndex(int)));
		//Viewfrustum Culling
		connect(m_renderWindow, SIGNAL(updateFrustumComboBox(int)), ui.comboBox_3, SLOT(setCurrentIndex(int)));
		//Wireframe Bounding Volume
		connect(m_renderWindow, SIGNAL(updateWireframeBVComboBox(int)), ui.comboBox_4, SLOT(setCurrentIndex(int)));
	}
	else{
		qDebug() << "Render Window already exists";
	}
}

void SettingsWin::createDeferredShadingScene()
{
	if (m_renderWindow == NULL){

		qDebug() << "Creating Render Window";
		// Specify an OpenGL 3.3 format using the Core profile.
		// That is, no old-school fixed pipeline functionality
		QGLFormat glFormat;
		glFormat.setVersion(3, 3);
		glFormat.setProfile(QGLFormat::CoreProfile); // Requires >=Qt-4.8.0
		glFormat.setSampleBuffers(true); //MSAA
		glFormat.setSamples(16);
		//Vsync off
		glFormat.setSwapInterval(0);

		m_renderWindow = new OpenGLWin(glFormat);
		m_renderWindow->setSceneType(Deferred_Shading);
		m_renderWindow->show();

		//OpenGL Version
		QString versionString(QLatin1String(reinterpret_cast<const char*>(glGetString(GL_VERSION))));
		qDebug() << "OpenGL Version:" << versionString << endl;


		/*
		Sets some GUI buttons and sliders to true
		*/

		//Field of View
		ui.horizontalSlider->setEnabled(true);
		//Field of View
		ui.spinBox->setEnabled(true);
		//Field of View
		ui.pushButton->setEnabled(true);

		//Wireframe Mode
		ui.comboBox->setEnabled(true);
		//Viewfrustum Culling
		ui.comboBox_3->setEnabled(true);
		//Wireframe Bounding Volume
		ui.comboBox_4->setEnabled(true);

		/*
		Sets some GUI buttons and sliders to false
		*/

		//Shape Scene button
		ui.pushButton_2->setEnabled(false);
		//Solar System button
		ui.pushButton_3->setEnabled(false);
		//Subdivision button
		ui.pushButton_6->setEnabled(false);
		//Shadowmap button
		ui.pushButton_9->setEnabled(false);
		//Deferred Shading button
		ui.pushButton_10->setEnabled(false);

		//////////////////////////////////////////////////////////////////////////

		//Field of View button
		connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(setFov()));

		/*
		When index of comboboxes change then call the slots
		*/

		//Wireframe mode
		connect(ui.comboBox, SIGNAL(currentIndexChanged(int)), m_renderWindow, SLOT(enableWireframe(int)));
		//Viewfrustum Culling
		connect(ui.comboBox_3, SIGNAL(currentIndexChanged(int)), m_renderWindow, SLOT(enableFrustumCulling(int)));
		//Wireframe Bounding Volume
		connect(ui.comboBox_4, SIGNAL(currentIndexChanged(int)), m_renderWindow, SLOT(enableWireframeBV(int)));

		//Toggles enabled state for Wireframe Bounding volume
		connect(m_renderWindow, SIGNAL(enableWireframeBVComboBox(int)), this, SLOT(enableWireframeBVComboBox(int)));

		//Disable parts of GUI when signal is emitted
		connect(m_renderWindow, SIGNAL(windowDestroyed()), this, SLOT(disableGUI()));

		//Updates the FPS display
		connect(m_renderWindow, SIGNAL(setGUIText(QString)), ui.plainTextEdit_2, SLOT(setPlainText(QString)));

		/*
		Change the comboboxes when signal is emitted
		*/

		//Wireframe mode
		connect(m_renderWindow, SIGNAL(updateWireframeComboBox(int)), ui.comboBox, SLOT(setCurrentIndex(int)));
		//Viewfrustum Culling
		connect(m_renderWindow, SIGNAL(updateFrustumComboBox(int)), ui.comboBox_3, SLOT(setCurrentIndex(int)));
		//Wireframe Bounding Volume
		connect(m_renderWindow, SIGNAL(updateWireframeBVComboBox(int)), ui.comboBox_4, SLOT(setCurrentIndex(int)));
	}
	else{
		qDebug() << "Render Window already exists";
	}
}

void SettingsWin::disableGUI()
{
	//Deallocate the render window
	delete m_renderWindow;
	m_renderWindow = NULL;
	//Clear FPS display
	ui.plainTextEdit_2->clear();
	
	//Disable Add random shape button
	if (ui.pushButton_5->isEnabled()){
		ui.pushButton_5->setEnabled(false);
	}
	//Disable Subdivide button
	if (ui.pushButton_7->isEnabled()){
		ui.pushButton_7->setEnabled(false);
	}
	//Disable Save mesh to file button
	if (ui.pushButton_8->isEnabled()){
		ui.pushButton_8->setEnabled(false);
	}
	//Disable Scale latest shape slider and reset value
	if (ui.horizontalSlider_2->isEnabled()){
		ui.horizontalSlider_2->setEnabled(false);
		ui.horizontalSlider_2->setValue(1);
	}
	//Disable Scale latest shape spinbox and reset value
	if (ui.spinBox_2->isEnabled()){
		ui.spinBox_2->setEnabled(false);
		ui.spinBox_2->setValue(1);
	}
	//Disable Delete latest shape button
	if (ui.pushButton_4->isEnabled()){
		ui.pushButton_4->setEnabled(false);
	}
	//Enable Shapes Scene button
	ui.pushButton_2->setEnabled(true);
	//Enable Solarsystem Scene button
	ui.pushButton_3->setEnabled(true);
	//Enable Subdivision Scene button
	ui.pushButton_6->setEnabled(true);
	//Enable Shadowmap button
	ui.pushButton_9->setEnabled(true);
	//Enable Deferred Shading button
	ui.pushButton_10->setEnabled(true);

	//Reset fov to default
	ui.horizontalSlider->setValue(60);

	//Reset Wireframe mode to off
	ui.comboBox->setCurrentIndex(0);
	//Reset Viewfrustum Culling mode to on
	ui.comboBox_3->setCurrentIndex(1);
	//Reset Wireframe Bounding Volume mode to off
	ui.comboBox_4->setCurrentIndex(0);
	//Reset Wireframe Original Mesh mode to off
	ui.comboBox_5->setCurrentIndex(0);

	//Disable Field of View Slider
	ui.horizontalSlider->setEnabled(false);
	//Disable Field of View spinbox
	ui.spinBox->setEnabled(false);
	//Disable Field of View button
	ui.pushButton->setEnabled(false);

	//Disable All comboboxes
	ui.comboBox->setEnabled(false);
	ui.comboBox_3->setEnabled(false);
	ui.comboBox_4->setEnabled(false);
	ui.comboBox_5->setEnabled(false);
}

void SettingsWin::setScaleSliderToDefault()
{
	ui.horizontalSlider_2->setValue(1);
}

void SettingsWin::enableWireframeBVComboBox(int toggle)
{
	ui.comboBox_4->setEnabled(toggle);
}

void SettingsWin::keyPressEvent( QKeyEvent *event )
{
	if(event->key() == Qt::Key_Escape){
		exit(1);
	}
}

void SettingsWin::closeEvent( QCloseEvent *event )
{
	exit(1);
}