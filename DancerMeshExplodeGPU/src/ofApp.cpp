#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() 
{
	fontSmall.load("Fonts/DIN.otf", 8);

    ofDisableArbTex();

	ofBackground(ofColor::black);

	lights.push_back( new ofLightExt() );
	lights.push_back( new ofLightExt() );
	lights.push_back(new ofLightExt());
	
	string mainSettingsPath = "Settings/Main.xml";
	gui.setup("Main", mainSettingsPath);

	gui.add( maxCopies.set("Copies", 10, 1, 100));
	gui.add( timeBetweenCopies.set("Time between Copies", 1, 0, 4));
	
	gui.add( maxRotation.set("Max Rotation", 3, 0, 20));
	gui.add( triangleNormalVel.set("Triangle Normal Vel", 0.001, 0, 0.1));
	gui.add( triangleNormalDrag.set("Triangle Normal Drag", 1, 0, 1) );

	gui.add( noisePositionFrequency.set("Noise Pos Freq", 0.1, 0, 1) );
	gui.add( noiseMagnitude.set("Noise Magnitude", 0.001, 0, 0.1) );
	gui.add( noiseTimeFrequency.set("Noise Time Freq", 1, 0, 5) );
	gui.add( noisePersistence.set("Noise Persistence", 0.35, 0, 1) );
	
	gui.add( wind.set("Wind", ofVec3f(0,0,-0.001), ofVec3f(-0.1), ofVec3f(0.1)));

	gui.add( startColor.set("Start Color", ofColor::black, ofColor(0,0,0,0), ofColor::white) );
	gui.add( endColor.set("End Color", ofColor::white, ofColor(0,0,0,0), ofColor::white) );

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

	lightingShader.load("Shaders/BlinnPhongRadius/GL3/BlinnPhongRadius");
	
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

	floor.set(200, 200, 2, 2);
	floor.rotate(-90, ofVec3f(1, 0, 0));
	floor.move(ofVec3f(0, 0, 0));

	lastTimeCopied = 0;

	int tmp = maxCopies;
	nuMeshesChanged( tmp );

	drawGui = false;
}

//--------------------------------------------------------------
void ofApp::nuMeshesChanged( int& _amount )
{
	for( int i = 0; i < meshes.size(); i++ )
	{
		delete meshes[i];
	}
	
	meshes.clear();
	for (int i = 0; i < maxCopies; i++)
	{
		meshes.push_back( new MeshShaderData() );
	}
}


//--------------------------------------------------------------
void ofApp::update() 
{    
	float t = ofGetElapsedTimef();

	ofSetGlobalAmbientColor( globalAmbient.get() );
	
	dancerMesh.update( ofGetElapsedTimef() );

	if (abs(t - lastTimeCopied) > timeBetweenCopies && meshes.size() > 0 )
	{
		ofMesh tmpMesh = dancerMesh.triangleMesh;
	
		MeshShaderData* nextMesh = meshes.back();
		meshes.pop_back();
		nextMesh->newMesh( tmpMesh );
		meshes.push_front(nextMesh);

		lastTimeCopied = t;
	}

	float meshMaxAge = meshes.size() * timeBetweenCopies;
	
	for (int i = 0; i < meshes.size(); i++)
	{
		meshes.at(i)->meshMaxAge		 = meshMaxAge;
		meshes.at(i)->maxRotation		 = maxRotation;
		meshes.at(i)->wind				 = wind;
		meshes.at(i)->triangleNormalVel  = triangleNormalVel;
		meshes.at(i)->triangleNormalDrag = triangleNormalDrag;
		
		meshes.at(i)->noisePositionFrequency = noisePositionFrequency;
		meshes.at(i)->noiseMagnitude		 = noiseMagnitude;
		meshes.at(i)->noiseTimeFrequency	 = noiseTimeFrequency;
		meshes.at(i)->noisePersistence		 = noisePersistence;
		
		meshes.at(i)->startColor			= startColor;
		meshes.at(i)->endColor				= endColor;
		
		meshes.at(i)->material				= dancerMaterial;
		
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

		lightingShader.begin();
	
			ofLightExt::setParams( &lightingShader, lights, ofGetCurrentMatrix(OF_MATRIX_MODELVIEW) );
			floorMaterial.setParams( &lightingShader );
			floor.draw();

			dancerMaterial.setParams( &lightingShader );
			dancerMesh.triangleMesh.draw();

		lightingShader.end();
	
		for ( int i = 0; i < meshes.size(); i++ )
		{
			meshes.at(i)->draw( lights );
		}

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




