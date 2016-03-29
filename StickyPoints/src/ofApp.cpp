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


	ofVec2f srcPixelTexSize(512, 512);
	stickyPoints.push_back(MeshPoint(ofVec2f(336, 45) / srcPixelTexSize));
	stickyPoints.push_back(MeshPoint(ofVec2f(243, 45) / srcPixelTexSize));
	//vector<ofVec2f> randomUVs = DancerMesh::getRandomUVPointsWithinMaskedArea( 10, "Masks/EmissionMaskBackAndArms.png", ofColor::white, 50);
	//for (int i = 0; i < randomUVs.size(); i++) { stickyPoints.push_back( StickyPoint(randomUVs.at(i)) ); }
	
	dancerMesh.findTriangleParamsForStickyPoints( stickyPoints );

	stickyPointPosHistory.resize(stickyPoints.size());
	lathedMeshes.resize(stickyPoints.size() );
	for (int i = 0; i < lathedMeshes.size(); i++)
	{
		lathedMeshes[i].circumferencePoints = Lathe::getCirclePoints( 20, ofVec2f(0.05, 0.05) );
		lathedMeshes[i].heightPoints		= Lathe::getCircularInOutHeightPoints( 200, 5, 1, 	0, 0.01, 0.99, 1.0 );
	}

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

	drawGui = false;
}

//--------------------------------------------------------------
void ofApp::update() 
{    
	float t = ofGetElapsedTimef();

	dancerMesh.update( ofGetElapsedTimef() );
	dancerMesh.updateStickyPoints(stickyPoints);

	int latheResolution = 200;
	int maxHistoryLength = latheResolution / 3;
	float minDistance = 0.01;

	for (int i = 0; i < stickyPoints.size(); i++)
	{
		deque<ofVec3f>& positions = stickyPointPosHistory.at(i);
		for (int j = 0; j < positions.size(); j++) 
		{ 
			ofVec3f tmpWindOffset = MathUtils::noiseVelocity( positions[j] * 2.0f, t * 0.1 ) * 0.001;
			positions[j] += tmpWindOffset;
		}
	}

	for (int i = 0; i < stickyPoints.size(); i++)
	{
		deque<ofVec3f>& positions = stickyPointPosHistory.at(i);

		ofVec3f lastPoint(9999); 
		if (positions.size() > 0) lastPoint = positions.back();
		if(stickyPoints.at(i).pos.distance(lastPoint) > minDistance) positions.push_front( stickyPoints.at(i).pos );
		while (positions.size() > maxHistoryLength) { positions.pop_back();  }

		if (positions.size() > 1)
		{
			lathedMeshes.at(i).updateMesh( positions, latheResolution, stickyPoints.at(i).normal );
		}
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

			dancerMaterial.begin();
				dancerMesh.triangleMesh.draw();
			dancerMaterial.end();

			for ( int i = 0; i < lathedMeshes.size(); i++ )
			{
				streamerMaterial.setDiffuseColor( ofFloatColor::fromHsb( ofMap( i, 0, lathedMeshes.size(), 0, 1), 1.0, 1.0 ) );
				streamerMaterial.begin();
					lathedMeshes[i].mesh.draw();
				streamerMaterial.end();
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











