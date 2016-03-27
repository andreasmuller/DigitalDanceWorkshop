#pragma once

#include "ofMain.h"
#include "ofxGui.h"
//#include "ofxAutoReloadedShader.h"

#include "Math/MathUtils.h"

#include "Utils/Cameras/ofxFirstPersonCamera.h"
#include "Utils/DrawingHelpers.h"
#include "Utils/ofTrueTypeFontExt.h"
#include "Utils/FboPingPong.h"

#include "ofMaterialExt.h"
#include "ofLightExt.h"

class ofApp : public ofBaseApp
{
	public:
		
		void setup();
		void update();
		void draw();
	
		void drawScene( float _time, ofShader* _shader, ofMatrix4x4 _modelView );
	
		void keyPressed(int key);


		ofxFirstPersonCamera	camera;
	
		ofMaterialExt			material;
		vector<ofLightExt*>	lights;
	
		ofShader				lightingShader;
		ofShader*				linearDepthShader;
	
		ofTrueTypeFontExt		fontSmall;
		ofTrueTypeFontExt		fontMedium;
		ofTrueTypeFontExt		fontLarge;

		ofxPanel				gui;

		ofParameter<ofColor>	sceneAmbient;
	
	/*
		ofParameter<ofColor>	lightDiffuse1;
		ofParameter<ofColor>	lightDiffuse2;
		ofParameter<ofColor>	lightDiffuse3;
		ofParameter<ofColor>	lightDiffuse4;

		ofParameter<float>		lightRadius1;
		ofParameter<float>		lightRadius2;
		ofParameter<float>		lightRadius3;
		ofParameter<float>		lightRadius4;
	*/
	
	/*
		ofxPanel				guiMaterial;
	
		ofParameter<ofColor>	materialDiffuse;
		ofParameter<ofColor>	materialSpecular;
		ofParameter<float>		materialShininess;

		ofxPanel				guiShadowMap;
	
	
		ofParameter<float>		shadowMapLightFov;
		ofParameter<float>		shadowMapLightNear;
		ofParameter<float>		shadowMapLightFar;
		ofParameter<float>		shadowMapLightBlurFactor;
		ofParameter<int>		shadowMapLightBlurNumPasses;
		ofParameter<float>		shadowCoeffecient;
	*/

		bool					drawGui;
};