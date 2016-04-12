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
		vector<MeshPoint> uniqueSpawnPoints;
		ofImage emissionMask;
		ofShader 			lightingShader;

	
		int						textureSize;
		
		FboPingPong				particleDataFbo;
		ofVboMesh				singleParticleMesh;
		ofTexture				spawnPosTexture;
		ofTexture				spawnVelTexture;
		
		float					maxAge;
		
		ofMaterial				particleMaterial;
		
		ofShader				particleUpdate;
		ofShader				particleDraw;
	
		ofPlanePrimitive 		floor;

		ofMaterialExt floorMaterial;
		ofMaterialExt dancerMaterial;
		ofMaterialExt spheresMaterial;
	
		ofParameter<ofColor> globalAmbient;
		vector<ofLightExt*> lights;

		ofxPanel gui;
		bool drawGui;
	
		ofTrueTypeFontExt fontSmall;
	
		vector<ofVec4f>			spawnVelScratch;
		vector<ofVec4f>			spawnPosScratch;
};
