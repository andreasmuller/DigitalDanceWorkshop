#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() 
{
	fontSmall.load("Fonts/DIN.otf", 8);

    ofDisableArbTex();

	ofSetLogLevel( OF_LOG_VERBOSE );
	
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
	randomPointsMaterial.addToPanel( &gui, "Random Points Material" );
	
	gui.loadFromFile(mainSettingsPath);
	gui.minimizeAll();
	gui.setPosition( ofGetWidth() - gui.getWidth() - 10, 10 );

	// Shader
	lightingShader.load("Shaders/BlinnPhongRadius/GL3/BlinnPhongRadius");
	
	// Load model
	string filename = "Models/WomanLowPoly_Aachan.fbx";
	//string filename = "Models/ManLowPoly_Aachan.fbx";
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
	drawGui = true;
	
	// Load a mask we will use later when grabbing random points on the mesh
	randomMask.load("Masks/EmissionMaskBackAndArms.png");

	updateSpheresShader.load("Shaders/Spheres/GL3/Update");
	drawSpheresInstancedShader.load("Shaders/Spheres/GL3/DrawInstancedGeometry");
	
	radius = 0.03;
	sphereMesh = ofSpherePrimitive(radius, 20, OF_PRIMITIVE_TRIANGLES).getMesh();
	
	maxAge = 3;
	texSize = 32; // Has to be a power of two. I guess it doesn't anyomre, but I'm old school.
	numSpheres = texSize * texSize;

	spawnPositionsTex.allocate( texSize, texSize, GL_RGBA32F, GL_RGBA, GL_FLOAT );
	spawnPositionsTex.setTextureMinMagFilter( GL_NEAREST, GL_NEAREST );
	
	ofDisableTextureEdgeHack();
	sphereData.allocateAsData( texSize, texSize, GL_RGBA32F, 2 );
	
	vector<ofVec4f> tmpStartData;
	vector<ofVec4f> tmpZero;
	for( int i = 0; i < numSpheres; i++ )
	{
		ofVec3f pos(ofRandom(1),ofRandom(1),ofRandom(1)); //TEMP // far away somewhere else
		float age = ofRandom(maxAge); // by randomizing start age we get a steady stream of particles as they respawn
		
		tmpStartData.push_back( ofVec4f( pos.x, pos.y, pos.z, age) );
		tmpZero.push_back( ofVec4f(0,0,0,0) );
	}
	sphereData.source()->getTexture(0).loadData( (float*)&tmpStartData.at(0).x, texSize, texSize, GL_RGBA );
	sphereData.source()->getTexture(1).loadData( (float*)&tmpStartData.at(0).x, texSize, texSize, GL_RGBA );
	
	sphereData.source()->getTexture(1).loadData( (float*)&tmpZero.at(0).x,		texSize, texSize, GL_RGBA );
	sphereData.dest()->getTexture(1).loadData(	 (float*)&tmpZero.at(0).x,		texSize, texSize, GL_RGBA );
}

//--------------------------------------------------------------
void ofApp::update() 
{   
	ofSetGlobalAmbientColor(globalAmbient.get());

	if (!ofGetKeyPressed(' '))
	{
		time += ofGetLastFrameTime();

		dancerMesh.update(time);
		
		// Grab some random points, we don't need them all to be unique as the particles won't all respawn at the same time
		int seed = 12345; //ofGetFrameNum();
		int numUniqueRandomPoints = numSpheres / 8;
		dancerMesh.updateRandomPoints( numUniqueRandomPoints, randomPoints, seed );
		//dancerMesh.updateRandomPoints( numUniqueRandomPoints, randomPoints, randomMask, seed );
		
		spawnPositionsBuffer.clear();
		for( int i = 0; i < numSpheres; i++ )
		{
			int index = MAX(0, ofRandom(randomPoints.size()-1));
			ofVec3f pos = randomPoints.at(index).pos;
			spawnPositionsBuffer.push_back( ofVec4f(pos.x, pos.y, pos.z, 1.0) );
		}
		spawnPositionsTex.loadData( (float*)&spawnPositionsBuffer.at(0).x, texSize, texSize, GL_RGBA );
	}
}

//--------------------------------------------------------------
void ofApp::draw()
{
	
	// I usually do these kinds of shader updates in the draw loop as I can be pretty sure all the graphics stuff
	// will be set up correcly then, whatever odd device you are running on.
	
	ofDisableTextureEdgeHack(); // Important on devices that don't support NPOT textures!
	ofDisableDepthTest();
	ofSetColor( ofColor::white );
	ofEnableBlendMode( OF_BLENDMODE_DISABLED ); // white color, no blending, we want the data we write in the shader to be undisturbed
	updateSpheresShader.begin();
	
	cout << "maxAge: " << maxAge << endl;
	
		updateSpheresShader.setUniform1f("maxAge", maxAge);
		updateSpheresShader.setUniform1f("timeStep", 1.0 / 60.0f );

		updateSpheresShader.setUniform2f("resolution", texSize, texSize );
	
		updateSpheresShader.setUniformTexture("posAndAgeTexture", sphereData.source()->getTexture(0), 1);
		updateSpheresShader.setUniformTexture("velTexture",		  sphereData.source()->getTexture(1), 2);
		updateSpheresShader.setUniformTexture("spawnPositionsTexture",	spawnPositionsTex,			  3);
	
		// Bind the target FBO and activate all the draw buffers it has
		sphereData.dest()->begin();
		sphereData.dest()->activateAllDrawBuffers();
	
			sphereData.source()->draw(0,0);
	
		sphereData.dest()->end();
	
		sphereData.swap(); // The latest data is now in ->source()
	
	updateSpheresShader.end();

	// Let's draw the scene
	ofEnableDepthTest();

    camera.begin();

		lightingShader.begin();
	
			ofLightExt::setParams( &lightingShader, lights, ofGetCurrentMatrix(OF_MATRIX_MODELVIEW), false );
			lightingShader.setUniform1f("useVertexColors", 0.0f );

			// Draw floor
			floorMaterial.setParams( &lightingShader );
			floor.draw();

			// Draw dancer
			dancerMaterial.setParams( &lightingShader );
			//dancerMesh.triangleMesh.draw();
	
/*
			for( int i = 0; i < spawnPositionsBuffer.size(); i++ )
			{
				ofVec4f p = spawnPositionsBuffer.at(i);
				ofDrawSphere( p.x, p.y, p.z, 0.1 );
			}
*/
	
		lightingShader.end();
	
		// Now let's set up the instanced drawing shader
		drawSpheresInstancedShader.begin();
	
			ofLightExt::setParams( &drawSpheresInstancedShader, lights, ofGetCurrentMatrix(OF_MATRIX_MODELVIEW), false );

			drawSpheresInstancedShader.setUniform2f("resolution", texSize, texSize );
	
			drawSpheresInstancedShader.setUniformTexture("posAndAgeTexture",	sphereData.source()->getTexture(0), 1);
			drawSpheresInstancedShader.setUniformTexture("oldPosAndAgeTexture", sphereData.dest()->getTexture(0),	2);

// TEMP
drawSpheresInstancedShader.setUniformTexture("spawnPosTexture", spawnPositionsTex,	3);
	
			drawSpheresInstancedShader.setUniformMatrix4f("normalMatrix", ofGetCurrentNormalMatrix() );
	
			drawSpheresInstancedShader.setUniform1f("maxAge", maxAge);
			drawSpheresInstancedShader.setUniform1f("timeStep", 1.0 / 60.0f );

			randomPointsMaterial.setParams( &drawSpheresInstancedShader );
			sphereMesh.drawInstanced(OF_MESH_FILL, numSpheres );
	
		drawSpheresInstancedShader.end();
	
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
		
		int size = 64;
		ofVec2f pos = gui.getPosition() + ofVec2f(0,gui.getHeight()+10);
		
		sphereData.source()->getTexture(0).draw(pos.x,pos.y,size,size);
		pos.y += size + 10;
		sphereData.source()->getTexture(1).draw(pos.x,pos.y,size,size);
		pos.y += size + 10;
		spawnPositionsTex.draw(pos.x,pos.y,size,size);

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