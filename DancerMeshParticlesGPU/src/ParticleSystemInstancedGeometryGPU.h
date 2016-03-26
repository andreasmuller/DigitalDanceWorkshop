//
//  ParticleSystemGPU.h
//  ParticlesGPU
//
//  Created by Andreas Müller on 11/01/2015.
//
//

#pragma once

#include "ofMain.h"
#include "ofxGui.h"

#include "Math/MathUtils.h"

#include "Utils/FileUtils.h"
#include "Utils/FboPingPong.h"

#include "ofLightExt.h"
#include "DancerMesh/MeshPoint.h"

#define PARTICLE_SYSTEM_NUM_BUILT_IN_MESHES 3

//-----------------------------------------------------------------------------------------
//
class ParticleSystemInstancedGeometryGPU
{
	public:
	
		void init( int _texSize, string _settingsPath = "Settings/Particles.xml" );
		void update( float _time, float _timeStep, vector<MeshPoint>& _uniqueSpawnPoints );
		void draw( vector<ofLightExt*>& _lights );

	
		void updateParticles( float _time, float _timeStep );
	
		void drawParticles( vector<ofLightExt*>& _lights  );
		void drawParticles( ofShader* _shader, vector<ofLightExt*>& _lights, ofMatrix4x4 _modelViewMatrix, ofMatrix4x4 _projectionMatrix, ofMatrix4x4 _normalMatrix );
	
		void reAllocate( int _textureSize );
	
		//void updateSpawnPositions( vector<ofVec4f>& _spawnPositions );
		//void updateSpawnVelocities( vector<ofVec4f>& _spawnPositions );
		int  getNumUniqueSpawnPositionsWanted();
	
		void resetParticles();
	
		void uploadDataRGBA( ofTexture& _tex, vector<ofVec4f>& _data );
	
		void drawGui();

		void particleDensityChanged( int& _density );
		void particleMaxAgeChanged( float& _type );
		void particleTypeChanged( int& _type );
		void particleSizeChanged( ofVec3f& _size );
	
		int						numParticles;
		int						textureSize;
	
		float					lastUpdateTime;
		float					timeStep;
	
		FboPingPong				particleDataFbo;
	
		ofVboMesh				singleParticleMesh;
	
		ofTexture				randomTexture;
		ofTexture				spawnPosTexture;
		ofTexture				spawnVelTexture;
	
		ofMaterial				particleMaterial;
	
		ofShader				particleUpdate;
		ofShader				particleDraw;

		ofxPanel				gui;
		ofParameter<int>		particleDensity;
		ofParameter<float>		particleMaxAge;
	
		ofParameter<float>		particleMaxVel;
	
		ofParameter<float>		noisePositionScale;
		ofParameter<float>		noiseMagnitude;
		ofParameter<float>		noiseTimeScale;
		ofParameter<float>		noisePersistence;
		ofParameter<float>		oldVelToUse;

		ofParameter<float>		vertexVelEmissionStrength;			// These shouldn't really be here
		ofParameter<float>		vertexNormalEmissionStrength;

		ofParameter<float>		particleStartScale;
		ofParameter<float>		particleEndScale;
		ofParameter<float>		particleSizeRandomness;

		ofParameter<ofVec3f>	particleMeshSize;
		ofParameter<int>		particleMeshType;

		ofParameter<ofVec3f>	wind;
		
		ofParameter<ofColor>	startColor;
		ofParameter<ofColor>	endColor;
		ofParameter<int>		paletteIndex;
	
		//ofParameter<ofColor>	materialDiffuse; // We will provide our own diffuse per particle
		ofParameter<ofColor>	materialAmbient;
		ofParameter<ofColor>	materialSpecular;
		ofParameter<ofColor>	materialEmissive;
	
		ofParameter<float>		materialShininess;
	
		vector<ofMesh>			particleMeshes;
	
		vector<ofImage>			ageColorPalettes;

	protected:
	
		float					frontFraceNormalSign;
	
		vector<ofVec4f>			spawnVelScratch;
		vector<ofVec4f>			spawnPosScratch;

};
