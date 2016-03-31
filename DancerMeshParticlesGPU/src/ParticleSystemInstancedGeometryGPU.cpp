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

	ofLogLevel prevLogLevel = ofGetLogLevel();
	ofSetLogLevel( OF_LOG_VERBOSE );
	
	// Load palettes
	vector<string> palettePaths = FileUtils::getFilePathsInFolder( "Palettes", "png" );
	ofDisableArbTex();
	for( int i = 0; i < palettePaths.size(); i++ )
	{
		ageColorPalettes.push_back( ofImage() );
		ageColorPalettes.back().load( palettePaths.at(i) );
	}
	ofEnableArbTex();
	
	/*
	// If you add the ofxObjLoader addon this will load all the meshes in a particular folder
	vector<string> allowedExts; allowedExts.push_back("obj"); //allowedExts.push_back("ply");
	vector<string> meshPaths = FileUtils::getFilePathsInFolder("Models/ParticleModels/", allowedExts );
	bool generateNormals = true;
	bool flipFaces = false;
	int maxAngleSmooth = 0;
	for( int i = 0; i < meshPaths.size(); i++ )
	{
		particleMeshes.push_back( ofMesh() );
		ofxObjLoader::load( meshPaths.at(i), particleMeshes.back(), generateNormals, flipFaces, maxAngleSmooth );
	}
	 */
	
	frontFraceNormalSign = 1.0;
	
	#ifdef TARGET_OPENGLES
		gui.setDefaultHeight( 30 );
	#endif
	
	gui.setup( "Particles", xmlSettingsPath );
	
	gui.add( particleDensity.set("Particle Density", 3, 2, 256) );
	
	gui.add( particleMaxAge.set("Particle Max Age", 1.0f, 0.0f, 5.0f) );

	gui.add( particleMaxVel.set("Particle Max Vel", 0.3, 0.0f, 0.3f) );
	gui.add( oldVelToUse.set("Old Vel To Use", 0.0f, 0.0f, 1.0f) );
	
	// These shouldn't really be here
	gui.add( vertexVelEmissionStrength.set("Emission Vertex Vel", 0.0, 0.0f, 0.4f) );
	gui.add( vertexNormalEmissionStrength.set("Emission Vertex Normal", 0.0, 0.0f, 0.2f) );
	
	gui.add( noiseMagnitude.set("Noise Magnitude", 0.01, 0.0f, 0.1f) );
	gui.add( noisePositionScale.set("Noise Position Scale", 0.1f, 0.01f, 1.0f) );
	gui.add( noiseTimeScale.set("Noise Time Scale", 0.3, 0.001f, 2.0f) );
	gui.add( noisePersistence.set("Noise Persistence", 0.2, 0.001f, 1.0f) );
	gui.add( wind.set("Wind", ofVec3f(0), ofVec3f(-0.1), ofVec3f(0.1)) );

	gui.add( particleMeshSize.set("Particle Mesh Size", ofVec3f(0.03, 0.03, 0.2), ofVec3f(0), ofVec3f(1)) );
	gui.add( particleSizeRandomness.set("Particle Size Randomness", 0.1, 0, 1) );
	gui.add( particleMeshType.set("Particle Mesh Type", 0, 0, PARTICLE_SYSTEM_NUM_BUILT_IN_MESHES + (particleMeshes.size())) );
	
	//gui.add( particleStartScale.set("Start Scale", ofVec3f(1), ofVec3f(0), ofVec3f(4)) );
	//gui.add( particleEndScale.set("End Scale", ofVec3f(1), ofVec3f(0), ofVec3f(4)) );

	gui.add( particleStartScale.set("Start Scale", 	1, 0, 4) );
	gui.add( particleEndScale.set(  "End Scale", 	1, 0, 4) );
	
	gui.add( startColor.set("Start Color",				 ofColor::white,		ofColor(0,0,0,0), ofColor::white) );
	gui.add( endColor.set("End Color",					 ofColor::white,		ofColor(0,0,0,0), ofColor::white) );
	gui.add( paletteIndex.set( "Age Color Palette",		 0, 0, ageColorPalettes.size()-1) );
	
	gui.add( materialAmbient.set("Material Ambient",   	 ofColor::black, ofColor(0,0,0,0), ofColor::white) );
	gui.add( materialSpecular.set("Material Specular",   ofColor::white, ofColor(0,0,0,0), ofColor::white) );
	gui.add( materialEmissive.set("Material Emmissive",  ofColor::black, ofColor(0,0,0,0), ofColor::white) );
	gui.add( materialShininess.set("Material Shininess",  60,  0, 127) );
	
	gui.loadFromFile( xmlSettingsPath );
	gui.minimizeAll();

	gui.setPosition( ofGetWidth() - gui.getWidth() - 10, 10 );

	particleDensity.addListener( this, &ParticleSystemInstancedGeometryGPU::particleDensityChanged );
	particleMeshSize.addListener( this, &ParticleSystemInstancedGeometryGPU::particleSizeChanged );
	particleMeshType.addListener( this, &ParticleSystemInstancedGeometryGPU::particleTypeChanged );
	particleMaxAge.addListener(   this, &ParticleSystemInstancedGeometryGPU::particleMaxAgeChanged );

	ofSetLogLevel( prevLogLevel );
	
	// Load shaders
	ofSetLogLevel( OF_LOG_VERBOSE );
	particleDraw.load("Shaders/Particles/GL3/DrawInstancedGeometry");
	particleUpdate.load("Shaders/Particles/GL3/Update");
	
	int tmp = 0;
	particleTypeChanged( tmp );
	
	particleDensityChanged( tmp );
	
	lastUpdateTime = 0;
	timeStep = 1.0 / 60.0;
}

//-----------------------------------------------------------------------------------------
//
void ParticleSystemInstancedGeometryGPU::update( float _time, float _timeStep, vector<MeshPoint>& _uniqueSpawnPoints )
{
	particleMaterial.setAmbientColor( materialAmbient.get() );
	particleMaterial.setSpecularColor( materialSpecular.get() );
	particleMaterial.setEmissiveColor( materialEmissive.get() );
	particleMaterial.setShininess( materialShininess );
	
	// Randomly grab from the unique spawn points to make data for every particle, even if it doesn't use it this frame
	spawnVelScratch.clear();
	spawnPosScratch.clear();
	for( int i = 0; i < textureSize*textureSize; i++ )
	{
		int randID = floor(ofRandom(_uniqueSpawnPoints.size()));
		
		spawnPosScratch.push_back( _uniqueSpawnPoints.at(randID).pos );		
		spawnVelScratch.push_back( _uniqueSpawnPoints.at(randID).vel ); // We could mix in some velocity in the direction of the normal here
	}
	
	uploadDataRGBA( spawnPosTexture, spawnPosScratch );
	uploadDataRGBA( spawnVelTexture, spawnVelScratch );
	
	lastUpdateTime = _time;
	timeStep = _timeStep;
	updateParticles( _time, _timeStep );
}


//-----------------------------------------------------------------------------------------
//
void ParticleSystemInstancedGeometryGPU::uploadDataRGBA( ofTexture& _tex, vector<ofVec4f>& _data )
{
	_tex.loadData( (float*)&_data[0].x,	 textureSize, textureSize, GL_RGBA );
}

//-----------------------------------------------------------------------------------------
//
void ParticleSystemInstancedGeometryGPU::draw( vector<ofLightExt*>& _lights )
{
	drawParticles( _lights );
}

//-----------------------------------------------------------------------------------------
//
void ParticleSystemInstancedGeometryGPU::reAllocate( int _textureSize )
{
	ofLogLevel prevLogLevel = ofGetLogLevel();
	ofSetLogLevel( OF_LOG_NOTICE );
	
	textureSize = _textureSize;
	numParticles = textureSize * textureSize;
	
	ofDisableTextureEdgeHack();
	particleDataFbo.allocateAsData(textureSize, textureSize, GL_RGBA32F, 2);
	ofEnableTextureEdgeHack();

	ofDisableTextureEdgeHack();
#ifdef TARGET_OPENGLES
	randomTexture.allocate(   textureSize, textureSize, GL_RGBA32F, false, GL_RGBA, GL_FLOAT );
	spawnPosTexture.allocate( textureSize, textureSize, GL_RGBA32F, false, GL_RGBA, GL_FLOAT );
	spawnVelTexture.allocate( textureSize, textureSize, GL_RGBA32F, false, GL_RGBA, GL_FLOAT );
#else
	ofDisableArbTex();
	randomTexture.allocate(   textureSize, textureSize, GL_RGBA32F, GL_RGBA, GL_FLOAT );
	spawnPosTexture.allocate( textureSize, textureSize, GL_RGBA32F, GL_RGBA, GL_FLOAT );
	spawnVelTexture.allocate( textureSize, textureSize, GL_RGBA32F, GL_RGBA, GL_FLOAT );
#endif
	ofEnableTextureEdgeHack();

	// If we do any interpolaton we can't use these as data arrays
	randomTexture.setTextureMinMagFilter( GL_NEAREST, GL_NEAREST );
	spawnPosTexture.setTextureMinMagFilter( GL_NEAREST, GL_NEAREST );
	spawnVelTexture.setTextureMinMagFilter( GL_NEAREST, GL_NEAREST );
	
	// Initialise the starting and static data
	vector<ofVec4f> randomData;
	for( int i = 0; i < numParticles; i++ )
	{
		randomData.push_back( ofVec4f(ofRandom(1), ofRandom(1), ofRandom(1), ofRandom(1)) );
	}

	// Upload it to the source texture
	randomTexture.loadData( (float*)&randomData[0].x, textureSize, textureSize, GL_RGBA );
	
	ofSetLogLevel( prevLogLevel );
}

//-----------------------------------------------------------------------------------------
//
void ParticleSystemInstancedGeometryGPU::resetParticles()
{
	cout << "ParticleSystemInstancedGeometryGPU::resetParticles   particleMaxAge: " << particleMaxAge << endl;
	
	// Initialise the starting and static data
	vector<ofVec4f> startPositionsAndAge;
	
	for( int y = 0; y < textureSize; y++ )
	{
		for( int x = 0; x < textureSize; x++ )
		{
			ofVec3f pos = (MathUtils::randomPointOnSphere() * 0.5) + ofVec3f(0,1000,0); // move them way off to start with
			float startAge = ofRandom( particleMaxAge ); // position is not very important, but age is, by setting the lifetime randomly somewhere in the middle we can get a steady stream emitting
			startPositionsAndAge.push_back( ofVec4f(pos.x, pos.y, pos.z, startAge) );
		}
	}
	
	// Upload it to the source texture
	particleDataFbo.source()->getTexture(0).loadData( (float*)&startPositionsAndAge[0].x,	 textureSize, textureSize, GL_RGBA );
}

//-----------------------------------------------------------------------------------------
//
void ParticleSystemInstancedGeometryGPU::drawGui()
{
	gui.draw();
	ofRectangle tmpTextureRect( gui.getPosition() + ofVec3f(0, gui.getHeight() + 10), randomTexture.getWidth(), randomTexture.getHeight() );

	//ofRectangle tmpTextureRect( gui.getPosition() + ofVec3f(0, 10), randomTexture.getWidth(), randomTexture.getHeight() );
	
	ofSetColor( ofColor::white );
	randomTexture.draw( tmpTextureRect );
	ofNoFill(); ofDrawRectangle( tmpTextureRect ); ofFill();
	
	ofRectangle dataDrawRect( tmpTextureRect.getPosition() + ofVec2f(0,tmpTextureRect.getHeight() + 10), particleDataFbo.source()->getWidth(), particleDataFbo.source()->getHeight() );
	
	particleDataFbo.source()->getTexture(0).draw( dataDrawRect ); // Pos and Age
	ofNoFill(); ofDrawRectangle( dataDrawRect ); ofFill();
	
	// vel
	if( particleDataFbo.source()->getNumTextures() > 1 )
	{
		ofRectangle dataDrawRectVel = dataDrawRect;
		dataDrawRectVel.x += dataDrawRect.width + 10;
		
		particleDataFbo.source()->getTexture(1).draw( dataDrawRectVel ); // vel
		ofNoFill(); ofDrawRectangle( dataDrawRectVel ); ofFill();
	}
	
	dataDrawRect.y += dataDrawRect.height + 10;
	spawnPosTexture.draw( dataDrawRect );
	ofNoFill(); ofDrawRectangle( dataDrawRect ); ofFill();

	dataDrawRect.y += dataDrawRect.height + 10;
	spawnVelTexture.draw( dataDrawRect );
	ofNoFill(); ofDrawRectangle( dataDrawRect ); ofFill();
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
	
			particleUpdate.setUniform1f("time", _time );
			particleUpdate.setUniform1f("timeStep", _timeStep );
			
			particleUpdate.setUniform1f("particleMaxAge", particleMaxAge );
			particleUpdate.setUniform1f("particleMaxVel", particleMaxVel );
	
			particleUpdate.setUniform1f("oldVelToUse", oldVelToUse );
	
			particleUpdate.setUniform1f("noisePositionScale", noisePositionScale );
			particleUpdate.setUniform1f("noiseTimeScale", noiseTimeScale );
			particleUpdate.setUniform1f("noisePersistence", noisePersistence );
			particleUpdate.setUniform1f("noiseMagnitude", noiseMagnitude );
			particleUpdate.setUniform3fv("wind", wind.get().getPtr() );
			
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
	
	ofFloatColor particleStartCol = startColor.get();
	ofFloatColor particleEndCol = endColor.get();

	ofSetColor( ofColor::white );
	ofEnableBlendMode( OF_BLENDMODE_ALPHA );
	
	_shader->begin();

		ofLightExt::setParams( _shader, _lights, _modelView, false );
	
		_shader->setUniformTexture("particlePosAndAgeTexture", particleDataFbo.source()->getTexture(0),			1 );
		_shader->setUniformTexture("particleVelTexture", particleDataFbo.source()->getTexture(1),				2 );
	
		if( ageColorPalettes.size() > 0 )
		{
			_shader->setUniformTexture( "ageColorPalette", ageColorPalettes.at(paletteIndex),					3 );
		}
	
		_shader->setUniformTexture( "spawnPositionTexture", spawnPosTexture,									4 );
		_shader->setUniformTexture( "spawnVelocityTexture", spawnVelTexture,									5 );
		_shader->setUniformTexture( "randomTexture", randomTexture,												6 );
	
		_shader->setUniform2f("resolution", particleDataFbo.source()->getWidth(), particleDataFbo.source()->getHeight() );
		_shader->setUniform1f("time", lastUpdateTime );
		_shader->setUniform1f("timeStep", timeStep );
	
		_shader->setUniformMatrix4f("modelViewMatrix", _modelView );
		_shader->setUniformMatrix4f("projectionMatrix", _projection );
		_shader->setUniformMatrix4f("modelViewProjectionMatrix", modelViewProjection );

		_shader->setUniformMatrix4f("normalMatrix", _normalMatrix );
	
		_shader->setUniform1f("frontFraceNormalSign", frontFraceNormalSign );
	
		_shader->setUniform1f("particleMaxAge", particleMaxAge );
	
		_shader->setUniform1f("particleStartScale", particleStartScale.get() );
		_shader->setUniform1f("particleEndScale", particleEndScale.get() );
	
		_shader->setUniform1f("particleSizeRandomness", 1 - particleSizeRandomness.get() );
	
		_shader->setUniform4fv("particleStartColor", particleStartCol.v );
		_shader->setUniform4fv("particleEndColor", particleEndCol.v );
	
		_shader->setUniform4fv("materialEmissive", ofFloatColor(materialEmissive.get()).v );
		_shader->setUniform4fv("materialAmbient",  ofFloatColor(materialAmbient.get()).v );
		_shader->setUniform4fv("materialSpecular", ofFloatColor(materialSpecular.get()).v );
	
		_shader->setUniform1f("materialShininess", materialShininess );
	
		// Calling begin() on the material sets the OpenGL state that we then read in the shader
		//particleMaterial.begin(); // this binds a shader under the programmable renderer
	
			singleParticleMesh.drawInstanced( OF_MESH_FILL, numParticles );

		//particleMaterial.end();
	
	_shader->end();
}

/*
//-----------------------------------------------------------------------------------------
//
void ParticleSystemInstancedGeometryGPU::updateSpawnPositions( vector<ofVec4f>& _spawnPositions )
{
	spawnPosTexture.loadData( (float*)&_spawnPositions[0].x,	 textureSize, textureSize, GL_RGBA );
}

//-----------------------------------------------------------------------------------------
//
void ParticleSystemInstancedGeometryGPU::updateSpawnVelocities( vector<ofVec4f>& _spawnVelocities)
{
	spawnVelTexture.loadData( (float*)&_spawnVelocities[0].x,	 textureSize, textureSize, GL_RGBA );
}
*/
 
//-----------------------------------------------------------------------------------------
//
int ParticleSystemInstancedGeometryGPU::getNumUniqueSpawnPositionsWanted()
{
	return numParticles / 8;
}

//-----------------------------------------------------------------------------------------
//
void ParticleSystemInstancedGeometryGPU::particleDensityChanged( int& _density )
{
	int texSize = particleDensity;
	cout << "ParticleSystemInstancedGeometryGPU::particleDensityChanged allocating " << texSize << endl;
	
	reAllocate( texSize );
	resetParticles();
}

//-----------------------------------------------------------------------------------------
//
void ParticleSystemInstancedGeometryGPU::particleMaxAgeChanged( float& _type)
{
	resetParticles();
}

//-----------------------------------------------------------------------------------------
//
void ParticleSystemInstancedGeometryGPU::particleTypeChanged( int& _type)
{
	ofPrimitiveMode primitiveMode = OF_PRIMITIVE_TRIANGLES;
	ofMesh tmpMesh;
	
	ofVec3f tmpSize = particleMeshSize.get();
	
	if( particleMeshType.get() == 0 )
	{
		ofBoxPrimitive box( tmpSize.x, tmpSize.y, tmpSize.z, 1, 1, 1 ); // we gotta face in the -Z direction
		tmpMesh = box.getMesh();
		
		frontFraceNormalSign = 1;
	}
	else if( particleMeshType.get() == 1 )
	{
		ofConePrimitive cone( tmpSize.x, tmpSize.z,  7, 1, 1, primitiveMode );
		tmpMesh = cone.getMesh();
		
		ofMatrix4x4 tmpMat = ofMatrix4x4::newRotationMatrix( -90, ofVec3f(1,0,0) );
		for( int i = 0; i < tmpMesh.getNumVertices(); i++ )
		{
			tmpMesh.getVerticesPointer()[i] = tmpMat * tmpMesh.getVerticesPointer()[i];
			if( i < tmpMesh.getNumNormals() ) tmpMesh.getNormalsPointer()[i] = tmpMat * tmpMesh.getNormalsPointer()[i];
		}
		
		frontFraceNormalSign = 1;
	}
	else if( particleMeshType.get() == 2 )
	{
		ofCylinderPrimitive cylinder( tmpSize.x, tmpSize.z,  7, 1, 1, true, primitiveMode );
		tmpMesh = cylinder.getMesh();
		
		ofMatrix4x4 tmpMat = ofMatrix4x4::newRotationMatrix( -90, ofVec3f(1,0,0) );
		for( int i = 0; i < tmpMesh.getNumVertices(); i++ )
		{
			tmpMesh.getVerticesPointer()[i] = tmpMat * tmpMesh.getVerticesPointer()[i];
			if( i < tmpMesh.getNumNormals() ) tmpMesh.getNormalsPointer()[i] = tmpMat * tmpMesh.getNormalsPointer()[i];
		}
		
		frontFraceNormalSign = 1;
	}
	else if( particleMeshType.get() == 3 )
	{
		ofSpherePrimitive sphere( tmpSize.x, 6, primitiveMode );
		tmpMesh = sphere.getMesh();
		
		frontFraceNormalSign = -1;
	}
	else if( particleMeshType.get() > PARTICLE_SYSTEM_NUM_BUILT_IN_MESHES )
	{
		int meshIndex = particleMeshType.get() - PARTICLE_SYSTEM_NUM_BUILT_IN_MESHES - 1;
		
		cout << "meshIndex " << meshIndex << " particleMeshes.size() " << particleMeshes.size() << endl;
		
		tmpMesh = particleMeshes[meshIndex];
		for( int i = 0; i < tmpMesh.getNumVertices(); i++ )
		{
			tmpMesh.getVerticesPointer()[i] = tmpMesh.getVerticesPointer()[i] * tmpSize;
		}
		
		for( int i = 0; i < tmpMesh.getNumNormals(); i++ )
		{
			tmpMesh.getNormalsPointer()[i] = (tmpMesh.getNormalsPointer()[i] * tmpSize).getNormalized();
		}
		
		cout << "New mesh " << tmpMesh.getNumVertices() << "	" << tmpMesh.getNumNormals() << "	" << tmpMesh.getNumIndices() << endl;
		
		frontFraceNormalSign = 1;
	}
	
	singleParticleMesh = tmpMesh;
}

//-----------------------------------------------------------------------------------------
//
void ParticleSystemInstancedGeometryGPU::particleSizeChanged( ofVec3f& _size )
{
	int tmpType = particleMeshType.get();
	particleTypeChanged( tmpType );
	
}