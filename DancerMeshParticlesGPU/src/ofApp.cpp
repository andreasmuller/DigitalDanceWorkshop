#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() 
{
	fontSmall.load("Fonts/DIN.otf", 8);

    ofDisableArbTex();

	lights.push_back( new ofLightExt() );
	lights.push_back( new ofLightExt() );
	for( int i = 0; i < lights.size(); i++ ) { lights.at(i)->enable(); }
	
	ofBackground(ofColor::black);

	string mainSettingsPath = "Settings/Main.xml";
	gui.setup("Main", mainSettingsPath);
	
	for( int i = 0; i < lights.size(); i++ )
	{
		lights.at(i)->addToPanel( &gui, "Light " + ofToString(i), 60, true );
	}
	
	gui.loadFromFile(mainSettingsPath);
	gui.minimizeAll();
	//gui.setPosition( ofGetWidth() - gui.getWidth() - 10, 10 );

	particles.init( 32 );
	
	string filename = "Models/TallWomanLowPoly_Aachan.fbx";

	ofMatrix4x4 meshBaseTransform = ofMatrix4x4::newScaleMatrix(0.01, 0.01, 0.01);
	meshBaseTransform.translate(0, 0.1, 0);

	dancerMesh.load( filename );
	dancerMesh.setBaseTransform( meshBaseTransform );

	// Mask out areas to emit particles by loading something in here, needs to match the UVs of your model
	emissionMask.load( "EmissionMasks/EmissionMaskBackAndArms.png");
	
	float tmpHeight = 1.93;
	camera.setAutoDistance(false);
	camera.setNearClip(0.01f);
	camera.setPosition(0, tmpHeight, 3);
	camera.lookAt(ofVec3f(0, tmpHeight*0.8, 0));

	floor.set(200, 200, 2, 2);
	floor.rotate(-90, ofVec3f(1, 0, 0));
	floor.move(ofVec3f(0, 0, 0));

	floorMaterial.setAmbientColor(ofFloatColor::black );
	floorMaterial.setDiffuseColor(ofFloatColor(0.8, 0.8, 0.8, 1.0));
	floorMaterial.setSpecularColor(ofFloatColor(0.8, 0.8, 0.8, 1.0));
	floorMaterial.setShininess(5);

	dancerMaterial.setAmbientColor(ofFloatColor(0.1));
	dancerMaterial.setDiffuseColor(ofFloatColor(1, 1, 1));
	dancerMaterial.setSpecularColor(ofFloatColor(1,1,1));
	dancerMaterial.setShininess(15);

	lastTimeCopied = 0;

	drawGui = false;
}

//--------------------------------------------------------------
void ofApp::update() 
{    
	float t = ofGetElapsedTimef();

	dancerMesh.update( ofGetElapsedTimef() );
	
	int numUniquePoints = dancerMesh.triangleMesh.getNumVertices() / 10;
	dancerMesh.updateRandomPoints( numUniquePoints, uniqueSpawnPoints, emissionMask );
	particles.update( t, 1.0/60.0, uniqueSpawnPoints );
	
	ofSetWindowTitle(ofToString(ofGetFrameRate(), 1));
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofBackgroundGradient(ofColor(40), ofColor(0), OF_GRADIENT_CIRCULAR);

	ofEnableDepthTest();

    camera.begin();
    
		ofEnableLighting();
		
			floorMaterial.begin();
				floor.draw();
			floorMaterial.end();

			// TEMP, everything should use the custom phong shader
			dancerMaterial.begin();

				dancerMesh.triangleMesh.draw();

			dancerMaterial.end();

			//DancerMesh::drawVelocities( uniqueSpawnPoints, 1.0 );
	
			particles.draw( lights );
	
		ofDisableLighting();
	
	
		ofSetColor( lights.at(0)->getDiffuseColor() );
		if(lights.at(0)->getIsEnabled()) ofDrawSphere( lights.at(0)->getPosition(), 0.1 );

		ofSetColor(lights.at(1)->getDiffuseColor());
		if (lights.at(1)->getIsEnabled()) ofDrawSphere(lights.at(1)->getPosition(), 0.1);
    
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




