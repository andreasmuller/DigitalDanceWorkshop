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
	lights.push_back( new ofLightExt() );
	
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
	streamerMaterial.addToPanel( &gui, "Streamer Material" );
	
	
	gui.loadFromFile(mainSettingsPath);
	gui.minimizeAll();
	gui.setPosition( ofGetWidth() - gui.getWidth() - 10, 10 );

	// Shader
	lightingShader.load("Shaders/BlinnPhongRadius/GL3/BlinnPhongRadius");
	
	// Load model
	string filename = "Models/WomanLowPoly_DanceStep8.fbx";
	ofMatrix4x4 meshBaseTransform = ofMatrix4x4::newScaleMatrix(0.01, 0.01, 0.01);
	meshBaseTransform.translate(0, 0.22, 0);
	
	// Here is a bit of a gotcha!
	//
	// For this to work your mesh needs to contain one set of texture unique coordinates.
	// These meshes have ploygons for the eyes that are meant to use a different texture, so they have their own texture coordinates
	// that work with that texture, we need to make sure these are filtered out when we make our triangle mesh, so we add the name of the
	// mesh to a skip list, you can later draw that mesh separately if you want
	dancerMesh.meshSkipList.push_back( "MHX:Low-Poly" ); // This is the name of the eye mesh
	
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
	
	// Init sticky points
	
	// These correspond to UVs that sit on the characters hand
	ofVec2f srcPixelTexSize(512, 512);
	stickyPoints.push_back(MeshPoint(ofVec2f(336, 45) / srcPixelTexSize));
	stickyPoints.push_back(MeshPoint(ofVec2f(243, 45) / srcPixelTexSize));

	dancerMesh.findTriangleParamsForStickyPoints( stickyPoints );
	
	// Init the lathe meshes
	stickyPointHistory.resize(stickyPoints.size());
	lathedMeshes.resize(stickyPoints.size() );
	for (int i = 0; i < lathedMeshes.size(); i++)
	{
		lathedMeshes[i].circumferencePoints = Lathe::getCirclePoints( 20, ofVec2f(0.01, 0.01) );  // a small circle for our circumference
		lathedMeshes[i].heightPoints		= Lathe::getCircularInOutHeightPoints( 200, 5, 1, 	0, 0.01, 0.99, 1.0 ); // tube shape for our length points
	}
}

//--------------------------------------------------------------
void ofApp::update() 
{   
	ofSetGlobalAmbientColor(globalAmbient.get());

	if (!ofGetKeyPressed(' '))
	{
		time += ofGetLastFrameTime();

		dancerMesh.update(time);
		dancerMesh.updateStickyPoints(stickyPoints); // Also update the sticky points
	
		int lengthResolution = 800;					// The number of slices we'll compute for our tube
		int maxHistoryLength = lengthResolution / 8; // how many control points will we keep
		float minDistance = 0.005;					// Don't add a point if it's closer than this
		
		// Update the max amount of points the PositionAndNormalHistory will keep
		for (int i = 0; i < stickyPointHistory.size(); i++) { stickyPointHistory.at(i).setMaxLength( maxHistoryLength); }

	//MIN( lengthResolution, history.getPositions().size()*8)
		
		for (int i = 0; i < stickyPoints.size(); i++)
		{
			PositionAndNormalHistory& history = stickyPointHistory.at(i);
			history.add( stickyPoints.at(i).pos, stickyPoints.at(i).normal, minDistance );
			
			if (history.getPositions().size() > 4)
			{
				lathedMeshes.at(i).updateMesh( history.getPositions(), lengthResolution,  ofVec3f(0,1,0) ); // history.getNormals()[0]
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofEnableDepthTest();

    camera.begin();

		lightingShader.begin();
	
			ofLightExt::setParams( &lightingShader, lights, ofGetCurrentMatrix(OF_MATRIX_MODELVIEW) );

			// Draw floor
			floorMaterial.setParams( &lightingShader );
			floor.draw();

			// Draw dancer
			dancerMaterial.setParams( &lightingShader );
			dancerMesh.triangleMesh.draw();
	
			for ( int i = 0; i < lathedMeshes.size(); i++ )
			{
				streamerMaterial.setDiffuseColor( ofFloatColor::fromHsb( ofMap( i, 0, lathedMeshes.size(), 0, 1), 0.8, 1.0 ) );
				streamerMaterial.setParams( &lightingShader );
				lathedMeshes[i].mesh.draw();
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