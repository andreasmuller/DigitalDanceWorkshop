//
//  ParticleSystemGPU.cpp
//  ParticlesGPU
//
//  Created by Andreas Müller on 11/01/2015.
//
//

#include "ParticleSystemInstancedGeometryGPU.h"

//-----------------------------------------------------------------------------------------
//
void ParticleSystemInstancedGeometryGPU::init( int _texSize, string _settingsPath )
{
	string xmlSettingsPath = _settingsPath; //"Settings/Particles.xml";
	
	textureSize = 128;
	maxAge = 0.5;
	
	// Load shaders
	ofSetLogLevel( OF_LOG_VERBOSE );
	particleDraw.load("Shaders/Spheres/GL3/DrawInstancedGeometry");
	particleUpdate.load("Shaders/Spheres/GL3/Update");
	
	float radius = 0.01;
	singleParticleMesh = ofSpherePrimitive( radius, 6, OF_PRIMITIVE_TRIANGLES ).getMesh();
	
	particleDataFbo.allocateAsData(textureSize, textureSize, GL_RGBA32F, 2);
	
	// Initialise the starting and static data
	vector<ofVec4f> startPositionsAndAge;
	
	for( int x = 0; x < textureSize*textureSize; x++ )
	{
		ofVec3f pos = ofVec3f(0,1000,0); // move them way off to start with
		float startAge = ofRandom( maxAge ); // position is not very important, but age is, by setting the lifetime randomly somewhere in the middle we can get a steady stream emitting
		startPositionsAndAge.push_back( ofVec4f(pos.x, pos.y, pos.z, startAge) );
	}
	
	
	// Upload it to the source texture
	particleDataFbo.source()->getTexture(0).loadData( (float*)&startPositionsAndAge[0].x,	 textureSize, textureSize, GL_RGBA );
	
	ofDisableTextureEdgeHack();
	
	ofDisableArbTex();
	spawnPosTexture.allocate( textureSize, textureSize, GL_RGBA32F, GL_RGBA, GL_FLOAT );
	spawnVelTexture.allocate( textureSize, textureSize, GL_RGBA32F, GL_RGBA, GL_FLOAT );
	
	ofEnableTextureEdgeHack();
	
	// If we do any interpolaton we can't use these as data arrays
	spawnPosTexture.setTextureMinMagFilter( GL_NEAREST, GL_NEAREST );
	spawnVelTexture.setTextureMinMagFilter( GL_NEAREST, GL_NEAREST );
	
	
}

//-----------------------------------------------------------------------------------------
//
void ParticleSystemInstancedGeometryGPU::update( float _time, float _timeStep, vector<MeshPoint>& _uniqueSpawnPoints )
{
	
	// Randomly grab from the unique spawn points to make data for every particle, even if it doesn't use it this frame
	spawnVelScratch.clear();
	spawnPosScratch.clear();
	for( int i = 0; i < textureSize*textureSize; i++ )
	{
		int randID = floor(ofRandom(_uniqueSpawnPoints.size()));
		
		spawnPosScratch.push_back( _uniqueSpawnPoints.at(randID).pos );		
		spawnVelScratch.push_back( _uniqueSpawnPoints.at(randID).vel ); // We could mix in some velocity in the direction of the normal here
	}
	
	spawnPosTexture.loadData( (float*)&spawnPosScratch[0].x,	 textureSize, textureSize, GL_RGBA );
	spawnVelTexture.loadData( (float*)&spawnVelScratch[0].x,	 textureSize, textureSize, GL_RGBA );
	
	updateParticles( _time, _timeStep );
}


//-----------------------------------------------------------------------------------------
//
void ParticleSystemInstancedGeometryGPU::draw( vector<ofLightExt*>& _lights )
{
	drawParticles( _lights );
}


//-----------------------------------------------------------------------------------------
//
void ParticleSystemInstancedGeometryGPU::updateParticles( float _time, float _timeStep )
{
	ofDisableTextureEdgeHack(); // Important on devices that don't support NPOT textures!
	
	ofEnableBlendMode( OF_BLENDMODE_DISABLED ); // Important! We just want to write the data as is to the target fbo
	ofSetColor( ofColor::white );
	
	particleDataFbo.dest()->begin();
	
		particleDataFbo.dest()->activateAllDrawBuffers(); // if we have multiple color buffers in our FBO we need this to activate all of them
	
		particleUpdate.begin();
	
			particleUpdate.setUniformTexture( "particlePosAndAgeTexture",	particleDataFbo.source()->getTexture(0), 1 );
			//particleUpdate.setUniformTexture( "particleOldPosAndAgeTexture", particleDataFbo.dest()->getTexture(0), 2 );
			particleUpdate.setUniformTexture( "particleVelTexture", particleDataFbo.source()->getTexture(1), 2 );

			particleUpdate.setUniformTexture( "spawnPositionTexture", spawnPosTexture, 3 );
			particleUpdate.setUniformTexture( "spawnVelocityTexture", spawnVelTexture, 4 );
	
			particleUpdate.setUniform1f( "maxAge", maxAge );
	
			particleDataFbo.source()->draw(0,0);
		
		particleUpdate.end();
		
	particleDataFbo.dest()->end();
	
	particleDataFbo.swap();
}

//-----------------------------------------------------------------------------------------
//
void ParticleSystemInstancedGeometryGPU::drawParticles( vector<ofLightExt*>& _lights  )
{
	drawParticles( &particleDraw, _lights, ofGetCurrentMatrix(OF_MATRIX_MODELVIEW), ofGetCurrentMatrix(OF_MATRIX_PROJECTION), ofGetCurrentNormalMatrix() );
}

//-----------------------------------------------------------------------------------------
//
void ParticleSystemInstancedGeometryGPU::drawParticles( ofShader* _shader,
														  vector<ofLightExt*>& _lights,
														  ofMatrix4x4 _modelView,
														  ofMatrix4x4 _projection,
														  ofMatrix4x4 _normalMatrix )
{
	ofMatrix4x4 modelViewProjection = _modelView * _projection;
	
	
	ofSetColor( ofColor::white );
	ofEnableBlendMode( OF_BLENDMODE_ALPHA );
	
	_shader->begin();

		ofLightExt::setParams( _shader, _lights, _modelView, false );
	
		_shader->setUniformTexture("particlePosAndAgeTexture", particleDataFbo.source()->getTexture(0),			1 );
		_shader->setUniformTexture("particleVelTexture", particleDataFbo.source()->getTexture(1),				2 );
	
		_shader->setUniformTexture( "spawnPositionTexture", spawnPosTexture,									4 );
		_shader->setUniformTexture( "spawnVelocityTexture", spawnVelTexture,									5 );
	
		_shader->setUniform2f("resolution", particleDataFbo.source()->getWidth(), particleDataFbo.source()->getHeight() );
	
		_shader->setUniform1f( "maxAge", maxAge );
	
		//_shader->setUniformMatrix4f("modelViewMatrix", _modelView );
		//_shader->setUniformMatrix4f("projectionMatrix", _projection );
		//_shader->setUniformMatrix4f("modelViewProjectionMatrix", modelViewProjection );

		_shader->setUniformMatrix4f("normalMatrix", _normalMatrix );
	
	
		// Calling begin() on the material sets the OpenGL state that we then read in the shader
		//particleMaterial.begin(); // this binds a shader under the programmable renderer
	
			singleParticleMesh.drawInstanced( OF_MESH_FILL, textureSize*textureSize );

		//particleMaterial.end();
	
	_shader->end();
}


