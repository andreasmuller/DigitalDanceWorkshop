#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxFBX.h"

#include "Math/MathUtils.h"
#include "Utils/ofTrueTypeFontExt.h"

#include "DancerMesh/DancerMeshFBX.h"
#include "ParticleSystemInstancedGeometryGPU.h"

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
		vector<MeshPoint> uniqueSpawnPoints;
		ofImage emissionMask;
		ParticleSystemInstancedGeometryGPU particles;

		ofPlanePrimitive floor;

		ofMaterialExt floorMaterial;
		ofMaterialExt dancerMaterial;

		float lastTimeCopied;

		vector<ofLightExt*> lights;

		ofxPanel gui;
		bool drawGui;

		ofTrueTypeFontExt fontSmall;
    
};
