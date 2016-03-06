#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxFBX.h"

#include "Math/MathUtils.h"
#include "Utils/ofTrueTypeFontExt.h"

#include "DancerMesh/DancerMeshFBX.h"

class ofApp : public ofBaseApp 
{
	public:
    
		void setup();
		void update();
		void draw();

		void keyPressed(int key);

		ofEasyCam camera;

		DancerMeshFBX dancerMesh;

		ofPlanePrimitive floor;
		ofMaterial floorMaterial;
		ofMaterial dancerMaterial;

		ofLight light0;
		ofLight light1;

		ofxPanel gui;
		bool drawGui;

		ofTrueTypeFontExt fontSmall;
    
};
