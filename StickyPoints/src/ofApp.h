#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxFBX.h"

#include "Math/MathUtils.h"
#include "Utils/ofTrueTypeFontExt.h"

#include "DancerMesh/DancerMeshFBX.h"

#include "Utils/Lathe.h"

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
	
		ofMaterial streamerMaterial;
		ofMaterial dancerMaterial;

		vector<StickyPoint> stickyPoints;

		vector<deque<ofVec3f>> stickyPointPosHistory;
		vector<LatheMesh> lathedMeshes;

		ofLight light0;
		ofLight light1;

		ofxPanel gui;
		bool drawGui;

		ofTrueTypeFontExt fontSmall;
    
};
