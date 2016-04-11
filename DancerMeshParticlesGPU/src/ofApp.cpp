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
	
	gui.loadFromFile(mainSettingsPath);
	gui.minimizeAll();

	particles.init( 32 );

	//string filename = "Models/ManLowPoly_DanceStep3.fbx";
	string filename = "Models/WomanLowPoly_DanceStep3.fbx";
	ofMatrix4x4 meshBaseTransform = ofMatrix4x4::newScaleMatrix(0.01, 0.01, 0.01);
	meshBaseTransform.translate(0, 0.21, 0); // move it up a bit
	dancerMesh.load( filename );
	dancerMesh.setBaseTransform( meshBaseTransform );

	// Mask out areas to emit particles by loading something in here, needs to match the UVs of your model
	emissionMask.load( "EmissionMasks/EmissionMaskBackAndArms.png");
	
	lightingShader.load( "Shaders/BlinnPhongRadius/GL3/BlinnPhongRadius" );
	
	float tmpHeight = 1.93;
	camera.setAutoDistance(false);
	camera.setNearClip(0.01f);
	camera.setPosition(0, tmpHeight, -1.0);
	camera.lookAt(ofVec3f(0, tmpHeight*0.8, 1));

	floor.set(200, 200, 2, 2);
	floor.rotate(-90, ofVec3f(1, 0, 0));
	floor.move(ofVec3f(0, 0, 0));

	drawGui = false;
}

//--------------------------------------------------------------
void ofApp::update() 
{    
	float t = ofGetElapsedTimef();

	ofSetGlobalAmbientColor( globalAmbient.get() );
	dancerMesh.update( ofGetElapsedTimef() );
	
	int numUniquePoints = dancerMesh.triangleMesh.getNumVertices() / 10;
	dancerMesh.updateRandomPoints( numUniquePoints, uniqueSpawnPoints, emissionMask );
	particles.update( t, 1.0/60.0, uniqueSpawnPoints );
	
	ofSetWindowTitle(ofToString(ofGetFrameRate(), 1));
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofEnableDepthTest();

    camera.begin();
    
		ofEnableLighting();
	
			lightingShader.begin();
	
				ofLightExt::setParams( &lightingShader, lights, ofGetCurrentMatrix(OF_MATRIX_MODELVIEW), false );
		
				floorMaterial.setParams( &lightingShader, false );
				floor.draw();

				dancerMaterial.setParams( &lightingShader, false );
				dancerMesh.triangleMesh.draw();

			lightingShader.end();
	
			//DancerMesh::drawVelocities( uniqueSpawnPoints, 1.0 );
	
			particles.draw( lights );
	
		ofDisableLighting();
	
	
		if( drawGui )
		{
			for( unsigned int i = 0; i < lights.size(); i++ )
			{
				lights.at(i)->draw( 0.2 );
			}
		}
	
    camera.end();
	
	ofDisableDepthTest();

	if (drawGui)
	{
		gui.draw();
		particles.drawGui();
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




