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

		ofEasyCam camera;

		DancerMeshFBX dancerMesh;
		ofShader phongShader;

		ofPlanePrimitive floor;
	
		ofParameter<ofColor> globalAmbient;
		ofMaterialExt floorMaterial;
		ofMaterialExt dancerMaterial;

		deque<ofMesh> meshes;
		ofParameter<float> timeBetweenCopies;
		ofParameter<int> maxCopies;
		ofParameter<int> numMeshesToDraw;
		ofParameter<ofVec3f> offset;
		ofParameter<ofColor> startColor;
		ofParameter<ofColor> endColor;
	
		vector<ofLightExt*> lights;

		ofxPanel gui;
		bool drawGui;

		float lastTimeCopied;
	
		ofTrueTypeFontExt fontSmall;
};