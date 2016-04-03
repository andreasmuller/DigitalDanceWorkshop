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
	randomPointsMaterial.addToPanel( &gui, "Random Points Material" );
	
	gui.loadFromFile(mainSettingsPath);
	gui.minimizeAll();
	gui.setPosition( ofGetWidth() - gui.getWidth() - 10, 10 );

	// Shader
	lightingShader.load("Shaders/BlinnPhongRadius/GL3/BlinnPhongRadius");
	
	// Load model
	string filename = "Models/TallWomanLowPoly_Aachan.fbx";
	ofMatrix4x4 meshBaseTransform = ofMatrix4x4::newScaleMatrix(0.01, 0.01, 0.01);
	meshBaseTransform.translate(0, 0.1, 0);
	dancerMesh.load( filename );
	dancerMesh.setBaseTransform( meshBaseTransform );

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

	time = 0;
	drawGui = false;
	
	// Load a mask we will use later when grabbing random points on the mesh
	randomMask.load("Masks/EmissionMaskBackAndArms.png");
}

//--------------------------------------------------------------
void ofApp::update() 
{   
	ofSetGlobalAmbientColor(globalAmbient.get());

	if (!ofGetKeyPressed(' '))
	{
		time += ofGetLastFrameTime();

		dancerMesh.update(time);
		
		// Show how seed works and masks
		int seed = 12345; //ofGetFrameNum();
		int maxAmount = 3500;
		int amount = ofMap( sinf(time), -1, 0.8, 1, maxAmount, true );
		dancerMesh.updateRandomPoints( amount, randomPoints, seed );
		//dancerMesh.updateRandomPoints( amount, randomPoints, randomMask, seed );
	}
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofEnableDepthTest();

    camera.begin();

		lightingShader.begin();
	
			ofLightExt::setParams( &lightingShader, lights, ofGetCurrentMatrix(OF_MATRIX_MODELVIEW) );
			lightingShader.setUniform1f("useVertexColors", 0.0f );

			// Draw floor
			floorMaterial.setParams( &lightingShader );
			floor.draw();

			// Draw dancer
			dancerMaterial.setParams( &lightingShader );
			dancerMesh.triangleMesh.draw();
	
			// Draw some spheres to show our random points
			randomPointsMaterial.setParams( &lightingShader );
			//lightingShader.setUniform1f("useVertexColors", 1.0f ); // When enabled we will use the vertex colors in sphereMesh
			vector<ofVec3f> tmpRandomPoints = MeshPoint::getPositions( randomPoints );
			DrawingHelpers::makeSpheresMesh( spheresMesh, tmpRandomPoints, 0.02f, 6, true );
			spheresMesh.draw();

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