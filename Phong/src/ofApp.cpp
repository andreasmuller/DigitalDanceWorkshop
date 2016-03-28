#include "ofApp.h"

//-----------------------------------------------------------------------------------------
//
void ofApp::setup()
{
	//ofSetLogLevel( OF_LOG_VERBOSE );
	ofSetLogLevel( OF_LOG_NOTICE );
	
	ofSetFrameRate( 60 );
	
	fontSmall.load("Fonts/DIN.otf", 8 );
	
	int numLights = 3;
	for( int i = 0; i < numLights; i++ )
	{
		ofSeedRandom(i << 24);
		
		lights.push_back( new ofLightExt() );
		
		lights.back()->setGlobalPosition( ofRandom(-4, 4), ofRandom(4, 6), ofRandom(-4, 4) );
		lights.back()->setDiffuseColor( ofFloatColor::fromHsb( i / (float)numLights, 1.0, 1.0) );
		lights.back()->setSpecularColor( ofFloatColor::white );
		lights.back()->setAmbientColor( ofFloatColor(0,0,0) );
		
		lights.back()->setRadius( 1.0 );

		lights.back()->enable();
	}
	ofSeedRandom();
	
	material.setAmbientColor( ofFloatColor::black );
	material.setDiffuseColor( ofFloatColor::grey );
	material.setSpecularColor( ofFloatColor::white );
	material.setShininess( 50 );
	
	ofxGuiSetDefaultWidth( 300 );
	
	// Main UI
	string xmlSettingsPath = "Settings/Main.xml";
	gui.setup( "Main", xmlSettingsPath );
	
	gui.add( sceneAmbient.set( "Scene Ambient", ofColor::black, ofColor(0,0,0,0), ofColor::white) );

	for( int i = 0; i < lights.size(); i++ )
	{
		lights.at(i)->addToPanel( &gui, "Light " + ofToString(i+1), 50 );
	}
	
	material.addToPanel( &gui, "World Material" );

	gui.loadFromFile( xmlSettingsPath );
	gui.minimizeAll();
	

	// Load the lighting shader	

	lightingShader.load( "Shaders/BlinnPhongRadius/GL3/BlinnPhongRadius" );
	
	camera.setNearClip(0.01f);
	camera.orbit( 170, -14, 10.0 );
	camera.setMovementMaxSpeed( 0.3f );

	drawGui = false;
}

//-----------------------------------------------------------------------------------------
//
void ofApp::update()
{
	ofSetGlobalAmbientColor( sceneAmbient.get() );
	
}

//-----------------------------------------------------------------------------------------
//
void ofApp::draw()
{
	float time = ofGetElapsedTimef();
	
	ofBackgroundGradient( ofColor(40,40,40), ofColor(0,0,0), OF_GRADIENT_CIRCULAR);	


	ofEnableDepthTest();
	ofEnableAlphaBlending();
	
	camera.begin();
	
		ofSetColor( ofColor::white );
	
		drawScene( time, &lightingShader, ofGetCurrentMatrix(OF_MATRIX_MODELVIEW) );
	
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
	if( drawGui )
	{
		gui.draw();
	}
	
	fontSmall.drawStringShadowed(ofToString(ofGetFrameRate(),2), ofGetWidth()-33, ofGetHeight()-6, ofColor::whiteSmoke, ofColor::black );
}

//-----------------------------------------------------------------------------------------
//
void ofApp::drawScene( float _time, ofShader* _shader, ofMatrix4x4 _modelView )
{
	
	_shader->begin();

		//ofxShadowMapLight::setParams( _shader, lights, _modelView, _shadowMapTexUnit, false );
		ofLightExt::setParams( _shader, lights, _modelView );
		material.setParams( _shader, false );

		
		_shader->setUniform1f("useVertexColors", 0 );
	
		ofMesh mesh;
		ofMesh sphereMesh = ofSpherePrimitive(0.6, 30).getMesh();
		
		for( unsigned int i = 0; i < 10; i++ )
		{
			ofSeedRandom( i << 24 );
			
			ofVec3f noisePos( ofRandom(-10, 10), ofRandom(-10, 10), ofRandom(-10, 10) );
			noisePos += ofVec3f(1,0,0) * _time * 0.1;
			
			ofVec3f drawPos;
			drawPos.x = ofSignedNoise(noisePos.x, noisePos.y, noisePos.z ) * 8.0;
			drawPos.y = ofNoise(noisePos.y, noisePos.z, noisePos.y ) * 5.0;
			drawPos.z = ofSignedNoise(noisePos.z, noisePos.x, noisePos.y ) * 8.0;
			
			ofMesh tmpSphereMesh = sphereMesh;
			for( unsigned int i = 0; i < tmpSphereMesh.getNumVertices(); i++ )
			{
				tmpSphereMesh.getVertices()[i] += drawPos;
			}
			
			//ofDrawSphere( drawPos, 0.6 );
			mesh.append( tmpSphereMesh );
		}
		
		ofSeedRandom();
	
		// Make some triangles to try the double sided material
		vector<ofVec3f> singleTriangleVertices;
		float triangleSize = 0.5;
		singleTriangleVertices.push_back( ofVec3f( -triangleSize, 0, -triangleSize ) );
		singleTriangleVertices.push_back( ofVec3f(  triangleSize, 0, -triangleSize ) );
		singleTriangleVertices.push_back( ofVec3f(  triangleSize, 0,  triangleSize ) );

		vector<ofVec3f> singleTriangleNormals;
		for( int i = 0; i < singleTriangleVertices.size(); i++ )
		{
			singleTriangleNormals.push_back( ofVec3f(0,1,0) );
		}

		vector<ofFloatColor> singleTriangleColors;
		for( int i = 0; i < singleTriangleVertices.size(); i++ )
		{
			singleTriangleColors.push_back( ofFloatColor::red );
		}
	
		ofMesh triangleMesh;
		triangleMesh.setMode( OF_PRIMITIVE_TRIANGLES );
	
		int numTriangles = 10;
		for( int triangleIndex = 0; triangleIndex < numTriangles; triangleIndex++ )
		{
			ofSeedRandom( triangleIndex << 24 );
			
			float angY = ofRandom(360) + _time * 2.61;
			float angZ = ofRandom(360) + _time * 2.55;
			
			ofMatrix4x4 tmpRot;
			tmpRot.glRotateRad( angY, 	0, 1, 0 );
			tmpRot.glRotateRad( angZ, 	0, 0, 1 );
	
			ofVec3f pos( ofRandom(-3,3), ofRandom(0,4), ofRandom(-3,3) );
			
			for( int i = 0; i < singleTriangleVertices.size(); i++ )
			{
				triangleMesh.addVertex( pos + (tmpRot * singleTriangleVertices.at(i)) );
			}

			for( int i = 0; i < singleTriangleNormals.size(); i++ )
			{
				triangleMesh.addNormal( (tmpRot * singleTriangleNormals.at(i)) );
			}
	
			ofFloatColor tmpCol = ofFloatColor::fromHsb( ofRandom(1), 0.6, 0.9 );
			for( int i = 0; i < singleTriangleColors.size(); i++ )
			{
				triangleMesh.addColor( tmpCol );
			}
			
		}
		ofSeedRandom();

		_shader->setUniform1f("useVertexColors", 1 );
		triangleMesh.draw();

	
		_shader->setUniform1f("useVertexColors", 0 );
		
		// Draw the vertices already in world space for now
		float size = 10;
		ofMesh floorMesh;
		floorMesh.setMode( OF_PRIMITIVE_TRIANGLES );
		floorMesh.addVertex( ofVec3f( -size, 0, -size ) );
		floorMesh.addVertex( ofVec3f(  size, 0, -size ) );
		floorMesh.addVertex( ofVec3f(  size, 0,  size ) );

		floorMesh.addVertex( ofVec3f( -size, 0, -size ) );
		floorMesh.addVertex( ofVec3f(  size, 0,  size ) );
		floorMesh.addVertex( ofVec3f( -size, 0,  size ) );
		for( unsigned int i = 0; i < floorMesh.getNumVertices(); i++ ) { floorMesh.addNormal( ofVec3f(0,1,0) ); }
		
		floorMesh.draw();
		
		mesh.draw();
	
	_shader->end();
	
}

//-----------------------------------------------------------------------------------------
//
void ofApp::keyPressed(int key)
{
	if( key == OF_KEY_TAB )
	{
		drawGui = !drawGui;
	}
	else if( key == 'f' )
	{
		ofToggleFullscreen();
	}
	else if( key == OF_KEY_LEFT  )
	{
		
	}
	else if( key == OF_KEY_RIGHT  )
	{
		
	}
}
