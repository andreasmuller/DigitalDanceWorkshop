#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxFBX.h"

#include "Math/MathUtils.h"
#include "Utils/ofTrueTypeFontExt.h"

#include "DancerMesh/DancerMeshFBX.h"

#include "ofLightExt.h"
#include "ofMaterialExt.h"
#include "MeshShaderData.h"

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

		ofMaterialExt floorMaterial;
		ofMaterialExt dancerMaterial;

		deque<MeshShaderData*> meshes;

		ofParameter<float> maxRotation;
		ofParameter<float> triangleNormalVel;
		ofParameter<float> triangleNormalDrag;
	
		ofParameter<ofVec3f> wind;

		float timeBetweenCopies;
		float lastTimeCopied;
		int maxCopies;

		ofLightExt light0;
		ofLightExt light1;

		ofxPanel gui;
		bool drawGui;

		ofTrueTypeFontExt fontSmall;
    
};
