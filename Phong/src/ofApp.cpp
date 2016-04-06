#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() 
{
	fontSmall.load("Fonts/DIN.otf", 8);

    ofDisableArbTex();

	ofBackground(ofColor::black);

	// Initialise some lights
	lights.push_back( new ofLightExt() );
	lights.push_back( new ofLightExt() );
	lights.push_back(new ofLightExt());
	
	// Init gui
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
	gui.setPosition( ofGetWidth() - gui.getWidth() - 10, 10 );

	// Shader
	lightingShader.load("Shaders/BlinnPhongRadius/GL3/BlinnPhongRadius");

	// Camera
	float tmpHeight = 1.93;
	camera.setAutoDistance(false);
	camera.setNearClip(0.01f);
	camera.setPosition(0, tmpHeight, 3);
	camera.lookAt(ofVec3f(0, tmpHeight*0.8, 0));

	// Set up floor
	floor.set(200, 200, 2, 2);
	floor.rotate(-90, ofVec3f(1, 0, 0));
	floor.move(ofVec3f(0, 0, 0));

	drawGui = false;
}

//--------------------------------------------------------------
void ofApp::update() 
{   
	ofSetGlobalAmbientColor(globalAmbient.get());


}

//--------------------------------------------------------------
void ofApp::draw()
{
	float t = ofGetElapsedTimef();
	
	ofEnableDepthTest();

    camera.begin();

		lightingShader.begin();
	
			ofLightExt::setParams( &lightingShader, lights, ofGetCurrentMatrix(OF_MATRIX_MODELVIEW) );

			// Draw floor
			floorMaterial.setParams( &lightingShader );
			floor.draw();

			for( int i = 0; i < 100; i++ )
			{
				ofSeedRandom( i << 24 );
				//dancerMaterial.setDiffuseColor( ofFloatColor::fromHsb( ofRandom(1), 0.7, 1.0 ) );
				dancerMaterial.setParams( &lightingShader );
				ofVec3f pos = ofVec3f(0,1,0);
				//pos += (MathUtils::randomPointOnSphere() * ofVec3f(1,ofRandom(1),1));
				pos += (MathUtils::noiseVelocity( ofVec3f(ofRandom(0.3),ofRandom(0.3),ofRandom(0.3)), t * ofRandom(0.4,0.55)) ) * 0.5;
				ofDrawSphere( pos, ofRandom(0.06, 0.14) );
			}

		lightingShader.end();
	
		if( drawGui )
		{
			for( unsigned int i = 0; i < lights.size(); i++ ) { lights.at(i)->draw( 0.2 ); }
		}
	
    camera.end();
	
	ofDisableDepthTest();

	if (drawGui)
	{
		gui.draw();
		fontSmall.drawStringShadowed(ofToString(ofGetFrameRate(), 1), ofGetWidth() - 25, ofGetHeight() - 5);
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	if (key == OF_KEY_TAB)
	{
		drawGui = !drawGui;
	}
	if (key == 'f')
	{
		ofToggleFullscreen();
	}
}