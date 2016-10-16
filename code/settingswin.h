#ifndef SETTINGSWIN_H
#define SETTINGSWIN_H

#include <QWidget>
#include "ui_settingswin.h"

#include "openglwin.h"

/// <remarks>
///GUI Window with settings
/// </remarks>
class SettingsWin : public QWidget
{
	Q_OBJECT

public:
	/// <summary>Constructor</summary>
	/// <param name="parent">parent for this class</param>
	/// <returns></returns>
	SettingsWin(QWidget* parent = 0);
	/// <summary>Destructor</summary>
	/// <returns></returns>
	~SettingsWin();

private slots:
	/// <summary>Gets the value from the fov slider and pass it to render window's setFov()</summary>
	/// <returns>void</returns>
	void setFov();
	/// <summary>Creates a render scene for shapes</summary>
	/// <returns>void</returns>
	void createRandomShapesScene();
	/// <summary>Creates a render scene for solar system</summary>
	/// <returns>void</returns>
	void createSolarSystemScene();
	/// <summary>Creates a render scene for subdivision</summary>
	/// <returns>void</returns>
	void createSubdivisionScene();
	/// <summary>Creates a render scene for shadowmap</summary>
	/// <returns>void</returns>
	void createShadowmapScene();
	/// <summary>Creates a render scene for deferred shading</summary>
	/// <returns>void</returns>
	void createDeferredShadingScene();
	/// <summary>Disables the GUI(sliders and relevant buttons)</summary>
	/// <returns>void</returns>
	void disableGUI();
	/// <summary>Resets the Scale Slider to value 1</summary>
	/// <returns>void</returns>
	void setScaleSliderToDefault();
	/// <summary>Enable/Disable the comboBox for Wireframe bounding sphere</summary>
	/// <param name="toggle">0 or 1</param>
	/// <returns>void</returns>
	void enableWireframeBVComboBox(int toggle);


private:
	Ui::SettingsWin ui;

	//Holds the openGL instance
	OpenGLWin* m_renderWindow;

	/// <summary>Key Event Listener, closes the program when ESC is pressed</summary>
	/// <param name="event">Event</param>
	/// <returns>void</returns>
	void keyPressEvent(QKeyEvent* event);
	/// <summary>Close Event Listener, closes the program</summary>
	/// <param name="event">Event</param>
	/// <returns>void</returns>
	void closeEvent(QCloseEvent* event);

};

#endif // SETTINGSWIN_H
