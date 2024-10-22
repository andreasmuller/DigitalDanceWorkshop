#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() 
{
	fontSmall.load("Fonts/DIN.otf", 8);

    ofDisableArbTex();

	lights.push_back( new ofLightExt() );
	lights.push_back( new ofLightExt() );
	lights.push_back( new ofLightExt() );
	for( int i = 0; i < lights.size(); i++ ) { lights.at(i)->enable(); }
	
	ofBackground(ofColor::black);

	string mainSettingsPath = "Settings/Main.xml";
	gui.setup("Main", mainSettingsPath);
	
	gui.add( globalAmbient.set("Global Ambient", ofColor::black, ofColor(0,0,0,0), ofColor(255)));
	for( int i = 0; i < lights.size(); i++ )
	{
		lights.at(i)->addToPanel( &gui, "Light " + ofToString(i), 60, true );
	}
	
	floorMaterial.addToPanel( &gui, "Floor Material" );
	dancerMaterial.addToPanel( &gui, "Dancer Material" );
	spheresMaterial.addToPanel( &gui, "Spheres Material" );
	
	gui.loadFromFile(mainSettingsPath);
	gui.minimizeAll();


	//string filename = "Models/ManLowPoly_DanceStep3.fbx";
	string filename = "Models/WomanLowPoly_DanceStep3.fbx";
	ofMatrix4x4 meshBaseTransform = ofMatrix4x4::newScaleMatrix(0.01, 0.01, 0.01);
	meshBaseTransform.translate(0, 0.22, 0); // move it up a bit
	dancerMesh.load( filename );
	dancerMesh.setBaseTransform( meshBaseTransform );

	// Mask out areas to emit particles by loading something in here, needs to match the UVs of your model
	//emissionMask.load( "EmissionMasks/EmissionMaskBackAndArms.png");
	
	lightingShader.load( "Shaders/BlinnPhongRadius/GL3/BlinnPhongRadius" );
	
	float tmpHeight = 1.93;
	camera.setAutoDistance(false);
	camera.setNearClip(0.01f);
	camera.setPosition(0, tmpHeight, -1.0);
	camera.lookAt(ofVec3f(0, tmpHeight*0.8, 1));

	floorPrim.set(200, 200, 2, 2);
	floorPrim.rotate(-90, ofVec3f(1, 0, 0));
	floorPrim.move(ofVec3f(0, 0, 0));

	textureSize = 128;
	maxAge = 1.1;
	
	// Load shaders
	ofSetLogLevel( OF_LOG_VERBOSE );
	spheresDraw.load("Shaders/Spheres/GL3/DrawInstancedGeometry");
	spheresUpdate.load("Shaders/Spheres/GL3/Update");
	
	
	drawGui = false;
}

//--------------------------------------------------------------
void ofApp::update() 
{    
	float t = ofGetElapsedTimef();

	ofSetGlobalAmbientColor( globalAmbient.get() );
	dancerMesh.update( t );
	
	// Grab a couple of random points. We don't need as many as we have spheres, as all particles won't spawn at the same time
	int numUniquePoints = dancerMesh.triangleMesh.getNumVertices() / 20;
	dancerMesh.updateRandomPoints( numUniquePoints, uniqueSpawnPoints, emissionMask );


	
	ofSetWindowTitle(ofToString(ofGetFrameRate(), 1));
}

//--------------------------------------------------------------
void ofApp::draw()
{

	
	
	// Now draw the scene
	ofEnableDepthTest();

    camera.begin();

		lightingShader.begin();

			ofLightExt::setParams( &lightingShader, lights, ofGetCurrentMatrix(OF_MATRIX_MODELVIEW), false );
	
			floorMaterial.setParams( &lightingShader, false );
			floorPrim.draw();

			dancerMaterial.setParams( &lightingShader, false );
			dancerMesh.triangleMesh.draw();

		lightingShader.end();

		// Use our instanced drawing shader for the spheres
		ofSetColor( ofColor::white );
		ofEnableBlendMode( OF_BLENDMODE_ALPHA );
	
	/*
		// We haven't allocated any of the textures yet, so it'll crash
	 
		spheresDraw.begin();
		
			ofLightExt::setParams( &spheresDraw, lights, ofGetCurrentMatrix(OF_MATRIX_MODELVIEW), false );
			
			spheresDraw.setUniformTexture("spheresPosAndAgeTexture", spheresDataFbo.source()->getTexture(0),			1 );
			spheresDraw.setUniformTexture("spheresVelTexture",		 spheresDataFbo.source()->getTexture(1),				2 );
			
			spheresDraw.setUniformTexture( "spawnPositionTexture", spawnPosTexture,									4 );
			spheresDraw.setUniformTexture( "spawnVelocityTexture", spawnVelTexture,									5 );
			
			spheresDraw.setUniform2f("resolution", spheresDataFbo.source()->getWidth(), spheresDataFbo.source()->getHeight() );
			spheresDraw.setUniform1f( "maxAge", maxAge );
			spheresDraw.setUniformMatrix4f("normalMatrix", ofGetCurrentNormalMatrix() );

			spheresMaterial.setParams( &spheresDraw, false );
		
			singleSphereMesh.drawInstanced( OF_MESH_FILL, textureSize*textureSize );
	
		spheresDraw.end();
	*/

		if( drawGui )
		{
			for( unsigned int i = 0; i < lights.size(); i++ ) { lights.at(i)->draw( 0.2 ); }
		}
	
    camera.end();
	
	ofDisableDepthTest();

	if (drawGui)
	{
		gui.draw();
		fontSmall.drawStringShadowed(ofToString(ofGetFrameRate(), 1), ofGetWidth() - 30, ofGetHeight() - 15);
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	if (key == OF_KEY_TAB)
	{
		drawGui = !drawGui;
	}
	else if (key == ' ')
	{
	}
	else if (key == 'f')
	{
		ofToggleFullscreen();
	}
}




