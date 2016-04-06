#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main()
{
	ofGLWindowSettings settings;
	settings.setGLVersion(3,3);
	settings.width  = 1360 * 0.85;
	settings.height = 768 * 0.85;
	settings.windowMode = OF_WINDOW;
	ofCreateWindow(settings);
	
	ofRunApp(new ofApp());
}