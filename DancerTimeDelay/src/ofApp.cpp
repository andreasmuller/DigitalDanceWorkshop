#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() 
{
	fontSmall.load("Fonts/DIN.otf", 8);

    ofDisableArbTex();

	ofBackground(ofColor::black);

	string mainSettingsPath = "Settings/Main.xml";
	gui.setup("Main", mainSettingsPath);

	gui.loadFromFile(mainSettingsPath);
	gui.minimizeAll();
	gui.setPosition( ofGetWidth() - gui.getWidth() - 10, 10 );

	string filename = "Models/TallWomanLowPoly_Aachan.fbx";

	ofMatrix4x4 meshBaseTransform = ofMatrix4x4::newScaleMatrix(0.01, 0.01, 0.01);
	meshBaseTransform.translate(0, 0.1, 0);

	dancerMesh.load( filename );
	dancerMesh.setBaseTransform( meshBaseTransform );

	float tmpHeight = 1.93;
	camera.setAutoDistance(false);
	camera.setNearClip(0.01f);
	camera.setPosition(0, tmpHeight, 3);
	camera.lookAt(ofVec3f(0, tmpHeight*0.8, 0));


	light0.setPointLight();
	light0.setPosition( 6, 9, -5);
	light0.setAttenuation(1, 0.085, 0);
	light0.enable();

	light1.setPointLight();
	light1.setPosition(-6, 10, 4);
	light1.setAttenuation(1, 0.085, 0);
	light1.enable();

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
	timeBetweenCopies = 1.0 / 30;
	maxCopies = 40;
	
	meshes.resize( maxCopies ); // allocate empty meshes, this way our logic for which ones to draw is super easy

	drawGui = false;
}

//--------------------------------------------------------------
void ofApp::update() 
{    
	float t = ofGetElapsedTimef();

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
	
	ofBackgroundGradient(ofColor(40), ofColor(0), OF_GRADIENT_CIRCULAR);

	ofEnableDepthTest();

    camera.begin();
    
		ofEnableLighting();
		
			floorMaterial.begin();
				floor.draw();
			floorMaterial.end();

	
				//dancerMesh.triangleMesh.draw();

				//int numMeshesToDraw = ofMap( ofGetMouseX(), 0, ofGetWidth(), 1, 50 );
				int numMeshesToDraw = meshes.size();
				ofVec3f offset(0,0, mx * -10 );
				ofColor startColor = ofColor::red;
				ofColor endColor = ofColor::blue;
				for( int i = 0; i < numMeshesToDraw; i++ )
				{
					int meshIndex = ofMap( i, 0, numMeshesToDraw-1, 0, meshes.size()-1 );
					
					dancerMaterial.begin();
			
						dancerMaterial.setDiffuseColor( startColor.getLerped( endColor, ofNormalize( meshIndex, 0,  numMeshesToDraw-1) ) );
										
						ofPushMatrix();
							ofTranslate( ofVec3f(0).getInterpolated( offset, ofNormalize( meshIndex, 0, meshes.size()-1)) );
							meshes.at( meshIndex ).draw();
						ofPopMatrix();
					
					dancerMaterial.end();
				}
	
	
	

		ofDisableLighting();
    
		ofSetColor( light0.getDiffuseColor() );
		if(light0.getIsEnabled()) ofDrawSphere( light0.getPosition(), 0.1 );

		ofSetColor(light1.getDiffuseColor());
		if (light1.getIsEnabled()) ofDrawSphere(light1.getPosition(), 0.1);
    
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
	if (key == ' ')
	{
	}
}











