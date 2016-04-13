#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() 
{
	fontSmall.load("Fonts/DIN.otf", 8);

    ofDisableArbTex();

	ofBackground(ofColor::black);

	for( int i = 0; i < 3; i++ )
	{
		lights.push_back( new ofLightExt() );
		lights.back()->enable();
	}
	
	// GUI
	string mainSettingsPath = "Settings/Main.xml";
	gui.setup("Main", mainSettingsPath);

	gui.add( maxCopies.set("Max Copies", 40, 2, 200) );
	gui.add( numMeshesToDraw.set("Num Meshes To Draw", 40, 2, 200) );
	gui.add( offset.set("Offset", ofVec3f(0,0,10), ofVec3f(-20), ofVec3f(20)) );
	//gui.add( timeBetweenCopies.set("Time Between Copies", 1 / 30.0, 0.00001, 1) );
	timeBetweenCopies = 0;
	
	gui.add( startColor.set("Dancer Start Color", ofColor::black, ofColor(0,0,0,0), ofColor(255)) );
	gui.add( endColor.set("Dancer End Color", ofColor::black, ofColor(0,0,0,0), ofColor(255)) );
	dancerMaterial.addToPanel( &gui, "Dancer Material" );
	
	gui.add( globalAmbient.set("Global Ambient", ofColor::black, ofColor(0,0,0,0), ofColor(255)) );
	
	for( int i = 0; i < lights.size(); i++ )
	{
		lights.at(i)->addToPanel( &gui, "Light " + ofToString(i) );
	}
	
	floorMaterial.addToPanel( &gui, "Floor Material" );
	
	gui.loadFromFile(mainSettingsPath);
	gui.minimizeAll();
	gui.setPosition( 10, 10 );

	// Load shader
	phongShader.load("Shaders/BlinnPhongRadius/GL3/BlinnPhongRadius");
	
	// Load model
	//string filename = "Models/ManLowPoly_DanceStep3.fbx";
	string filename = "Models/WomanLowPoly_DanceStep3.fbx";
	ofMatrix4x4 meshBaseTransform = ofMatrix4x4::newScaleMatrix(0.01, 0.01, 0.01);
	meshBaseTransform.translate(0, 0.24, -8); // Note that we're shifting the mesh here, in case you change the fbx input
	dancerMesh.load( filename );
	dancerMesh.setBaseTransform( meshBaseTransform );

	// Camera
	float tmpHeight = 1.93;
	camera.setAutoDistance(false);
	camera.setNearClip(0.01f);
	camera.setPosition(-3, tmpHeight, -4);
	camera.lookAt(ofVec3f(0, tmpHeight*0.8, 1));

	// Set up a floor mesh
	floor.set(200, 200, 2, 2);
	floor.rotate(-90, ofVec3f(1, 0, 0));
	floor.move(ofVec3f(0, 0, 0));

	lastTimeCopied = 0;
	
	meshes.resize( maxCopies ); // allocate empty meshes, this way our logic for which ones to draw is super easy

	drawGui = false;
}

//--------------------------------------------------------------
void ofApp::update() 
{    
	float t = ofGetElapsedTimef();

	ofSetGlobalAmbientColor( globalAmbient.get() );
	dancerMesh.update( ofGetElapsedTimef() );

	// Copy meshes
	if (abs(t - lastTimeCopied) > timeBetweenCopies)
	{
		ofMesh tmpMesh = dancerMesh.triangleMesh;
		meshes.push_front( tmpMesh );
		while (meshes.size() > maxCopies) { meshes.pop_back(); }
		lastTimeCopied = t;
	}

	ofSetWindowTitle(ofToString(ofGetFrameRate(), 1));
}

//--------------------------------------------------------------
void ofApp::draw()
{
	float t = ofGetElapsedTimef();
	float mx = ofNormalize( ofGetMouseX(), 0, ofGetWidth() );
	float my = ofNormalize( ofGetMouseY(), 0, ofGetHeight() );
	
	//ofBackgroundGradient(ofColor(40), ofColor(0), OF_GRADIENT_CIRCULAR);

	ofEnableDepthTest();

    camera.begin();
	
		phongShader.begin();
	
			ofLightExt::setParams( &phongShader, lights, ofGetCurrentMatrix( OF_MATRIX_MODELVIEW ) );
			floorMaterial.setParams( &phongShader, false );
			floor.draw();

				for( int i = 0; i < numMeshesToDraw; i++ )
				{
					int meshIndex = ofMap( i, 0, numMeshesToDraw-1, 0, meshes.size()-1 );
					float colorFrac = ofMap(i, 0, numMeshesToDraw - 1, 0, 1);
				
					dancerMaterial.setDiffuseColor( startColor.get().getLerped( endColor.get(), colorFrac) );
					dancerMaterial.setParams( &phongShader, false );
				
					ofPushMatrix();
						ofTranslate( ofVec3f(0).getInterpolated( offset, ofNormalize( meshIndex, 0, meshes.size()-1)) );
						meshes.at( meshIndex ).draw();
					ofPopMatrix();
				}
	
		phongShader.end();
	
		if (drawGui)
		{
			for( int i = 0; i < lights.size(); i++ )
			{
				ofSetColor( lights.at(i)->getDiffuseColor() );
				if(lights.at(i)->getIsEnabled()) ofDrawSphere( lights.at(i)->getPosition(), 0.1 );
			}
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
	if (key == 'f')
	{
		ofToggleFullscreen();
	}
}