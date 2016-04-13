#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxFBX.h"

#include "Math/MathUtils.h"

#include "Utils/ofTrueTypeFontExt.h"
#include "Utils/FboPingPong.h"

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
	
	
		ofEasyCam				camera;

		DancerMeshFBX			dancerMesh;
		vector<MeshPoint>		uniqueSpawnPoints;
		ofImage					emissionMask;
		ofShader				lightingShader;

		int						textureSize;
		
		FboPingPong				spheresDataFbo;
		ofTexture				spawnPosTexture;
		ofTexture				spawnVelTexture;
		ofVboMesh				singleSphereMesh;
	
		float					maxAge;
	
		ofShader				spheresUpdate;
		ofShader				spheresDraw;
	
		ofPlanePrimitive 		floorPrim;

		ofMaterialExt			floorMaterial;
		ofMaterialExt			dancerMaterial;
		ofMaterialExt			spheresMaterial;

		ofParameter<ofColor>	globalAmbient;
		vector<ofLightExt*>		lights;

		ofxPanel				gui;
		bool					drawGui;
		ofTrueTypeFontExt		fontSmall;
	
		vector<ofVec4f>			spawnVelScratch;
		vector<ofVec4f>			spawnPosScratch;
};
