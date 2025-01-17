#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxFBX.h"

#include "Math/MathUtils.h"
#include "Utils/ofTrueTypeFontExt.h"

#include "DancerMesh/DancerMeshFBX.h"

#include "ofLightExt.h"
#include "ofMaterialExt.h"

class ofApp : public ofBaseApp 
{
	public:
    
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
	
		float time;

		ofEasyCam camera;

		DancerMeshFBX dancerMesh;

		ofPlanePrimitive floor;

		ofMaterialExt floorMaterial;
		ofMaterialExt dancerMaterial;
	
		ofShader lightingShader;

		ofParameter<ofColor> globalAmbient;

		vector<ofLightExt*> lights;

		ofxPanel gui;
		bool drawGui;

		ofTrueTypeFontExt fontSmall;
    
};
