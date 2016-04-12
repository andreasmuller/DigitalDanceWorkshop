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
	
	
		int						textureSize;
	
		FboPingPong				particleDataFbo;
		ofVboMesh				singleParticleMesh;
		ofTexture				spawnPosTexture;
		ofTexture				spawnVelTexture;
	
		float					maxAge;
	
		ofMaterial				particleMaterial;
	
		ofShader				particleUpdate;
		ofShader				particleDraw;
	
	protected:
	
		vector<ofVec4f>			spawnVelScratch;
		vector<ofVec4f>			spawnPosScratch;

};
