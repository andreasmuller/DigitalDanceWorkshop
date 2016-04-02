#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxFBX.h"

#include "Math/MathUtils.h"
#include "Utils/DrawingHelpers.h"
#include "Utils/ofTrueTypeFontExt.h"

#include "DancerMesh/DancerMeshFBX.h"

#include "ofLightExt.h"
#include "ofMaterialExt.h"

#include "Utils/Lathe.h"

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
		ofMaterialExt streamerMaterial;
	
		ofShader lightingShader;

		ofParameter<ofColor> globalAmbient;

		vector<ofLightExt*> lights;
	
		vector<MeshPoint> stickyPoints;
		vector<PositionAndNormalHistory> stickyPointHistory;
		//vector<deque<ofVec3f>> stickyPointPosHistory;
		vector<LatheMesh> lathedMeshes;

		ofxPanel gui;
		bool drawGui;

		ofTrueTypeFontExt fontSmall;
    
};
