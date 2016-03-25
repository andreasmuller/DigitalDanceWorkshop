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
	
		void nuMeshesChanged( int& _amount );

		ofEasyCam camera;

		DancerMeshFBX dancerMesh;

		ofPlanePrimitive floor;

		ofMaterialExt floorMaterial;
		ofMaterialExt dancerMaterial;

		deque<MeshShaderData*> meshes;

		ofParameter<int> maxCopies;
		ofParameter<float> timeBetweenCopies;

		ofParameter<float> maxRotation;
		ofParameter<float> triangleNormalVel;
		ofParameter<float> triangleNormalDrag;
	
		ofParameter<float> noisePositionFrequency;
		ofParameter<float> noiseMagnitude;
		ofParameter<float> noiseTimeFrequency;
		ofParameter<float> noisePersistence;
	
		ofParameter<ofColor> startColor;
		ofParameter<ofColor> endColor;
	
		ofParameter<ofVec3f> wind;
	
		float lastTimeCopied;

		vector<ofLightExt*> lights;

		ofxPanel gui;
		bool drawGui;

		ofTrueTypeFontExt fontSmall;
    
};
