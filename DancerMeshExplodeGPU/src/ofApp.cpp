#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() 
{
	fontSmall.load("Fonts/DIN.otf", 8);

    ofDisableArbTex();

	ofBackground(ofColor::black);

	string mainSettingsPath = "Settings/Main.xml";
	gui.setup("Main", mainSettingsPath);

	gui.add( maxRotation.set("Max Rotation", 3, 0, 20));
	gui.add( triangleNormalVel.set("Triangle Normal Vel", 0.001, 0, 0.1));
	gui.add( triangleNormalDrag.set("Triangle Normal Drag", 1, 0, 1) );

	gui.add( noisePositionFrequency.set("Noise Pos Freq", 0.1, 0, 1) );
	gui.add( noiseMagnitude.set("Noise Magnitude", 0.001, 0, 0.1) );
	gui.add( noiseTimeFrequency.set("Noise Time Freq", 1, 0, 5) );
	gui.add( noisePersistence.set("Noise Persistence", 0.35, 0, 1) );
	
	gui.add( wind.set("Wind", ofVec3f(0,0,-0.001), ofVec3f(-0.1), ofVec3f(0.1)));
	
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
	timeBetweenCopies = 1.0;
	maxCopies = 6;

	for (int i = 0; i < maxCopies; i++)
	{
		meshes.push_back( new MeshShaderData() );
	}

	drawGui = false;
}

//--------------------------------------------------------------
void ofApp::update() 
{    
	float t = ofGetElapsedTimef();

	dancerMesh.update( ofGetElapsedTimef() );

	if (abs(t - lastTimeCopied) > timeBetweenCopies)
	{
		float meshMaxAge = meshes.size() * timeBetweenCopies;
		ofMesh tmpMesh = dancerMesh.triangleMesh;
	
		MeshShaderData* nextMesh = meshes.back();
		meshes.pop_back();
		nextMesh->newMesh( tmpMesh, meshMaxAge);
		meshes.push_front(nextMesh);

		lastTimeCopied = t;
	}

	for (int i = 0; i < meshes.size(); i++)
	{
		meshes.at(i)->maxRotation		 = maxRotation;
		meshes.at(i)->wind				 = wind;
		meshes.at(i)->triangleNormalVel  = triangleNormalVel;
		meshes.at(i)->triangleNormalDrag = triangleNormalDrag;
		
		meshes.at(i)->noisePositionFrequency = noisePositionFrequency;
		meshes.at(i)->noiseMagnitude		 = noiseMagnitude;
		meshes.at(i)->noiseTimeFrequency	 = noiseTimeFrequency;
		meshes.at(i)->noisePersistence		 = noisePersistence;
		
		meshes.at(i)->update();
	}
	
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

			//dancerMaterial.begin();

				dancerMesh.triangleMesh.draw();

				for ( int i = 0; i < meshes.size(); i++ )
				{
					meshes.at(i)->draw();
				}

			//dancerMaterial.end();
			
		ofDisableLighting();
    
		ofSetColor( light0.getDiffuseColor() );
		if(light0.getIsEnabled()) ofDrawSphere( light0.getPosition(), 0.1 );

		ofSetColor(light1.getDiffuseColor());
		if (light1.getIsEnabled()) ofDrawSphere(light1.getPosition(), 0.1);
    
    camera.end();
    
	ofDisableDepthTest();

	/*
	ofRectangle tmpRect(10, 10, 300, 100);
	
	ofSetColor( ofColor::darkSlateGrey );
	ofDrawRectangle(tmpRect );

	ofMesh tmpMesh;
	tmpMesh.setMode(OF_PRIMITIVE_LINE_STRIP);
	int res = 90;
	for (int i = 0; i < res; i++)
	{
		float frac = ofNormalize(i, 0, res - 1 );
		ofVec2f p;
		p.x = ofMap(frac, 0, 1, tmpRect.x, tmpRect.x + tmpRect.width);
		p.y = ofMap( MathUtils::linearStepOut(0.9, 1.0, frac), 0, 1,  tmpRect.y + tmpRect.height, tmpRect.y );
		tmpMesh.addVertex(p);
	}
	ofSetColor(ofColor::white);
	tmpMesh.draw();
	*/


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











