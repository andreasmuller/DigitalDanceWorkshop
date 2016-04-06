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

	lastTimeCopied = 0;
	timeBetweenCopies = 0.4;
	maxCopies = 6;
	
	time = 0;
	drawGui = false;
}

//--------------------------------------------------------------
void ofApp::update() 
{   
	ofSetGlobalAmbientColor(globalAmbient.get());

	if (!ofGetKeyPressed(' '))
	{
		time += ofGetLastFrameTime();

		dancerMesh.update(time);
		
		float t = time;
		if (abs(t - lastTimeCopied) > timeBetweenCopies)
		{
			ofMesh tmpMesh = dancerMesh.triangleMesh;
			meshes.push_front( tmpMesh );
			while (meshes.size() > maxCopies) { meshes.pop_back(); }
			lastTimeCopied = t;
		}
		
		float meshMaxAge = meshes.size()*timeBetweenCopies;
		
		for (int meshIndex = 0; meshIndex < meshes.size(); meshIndex++ )
		{
			ofSeedRandom(meshIndex * 16777216); // seed the rng with the mesh index, made into a much bigger number
			
			ofMesh& mesh = meshes.at(meshIndex);
			//cout << mesh.getNumVertices() << "	" << mesh.getNumNormals() << "	" << mesh.getNumIndices() << endl;
			float meshTime = fmodf(t, timeBetweenCopies) + (meshIndex * timeBetweenCopies);
			float ageFrac = (meshTime / meshMaxAge);
			
			float forcesStrength = MathUtils::linearStep(meshMaxAge * 0.25, meshMaxAge * 0.6, meshTime );
			float triangleScale = (1.0f - MathUtils::linearStep(meshMaxAge * 0.6, meshMaxAge, meshTime)); //(1.0f - MathUtils::linearStep(_high1, _low1, _t)
			triangleScale = ofMap(triangleScale, 0, 1, 0.95, 1.0);
			
			// We are just going to assume we're dealing with triangles
			int numTriangles = mesh.getNumVertices() / 3;
			for (int triangleIndex = 0; triangleIndex < numTriangles; triangleIndex++)
			{
				int i0 = (triangleIndex * 3);
				int i1 = (triangleIndex * 3) + 1;
				int i2 = (triangleIndex * 3) + 2;
				
				ofVec3f p0 = mesh.getVertex(i0);
				ofVec3f p1 = mesh.getVertex(i1);
				ofVec3f p2 = mesh.getVertex(i2);
				
				ofVec3f normal = -MathUtils::getTriangleNormal(p0, p1, p2);
				ofVec3f pos = (p0 + p1 + p2) / 3.0;
				
				ofVec3f v0 = (p0 - pos) * triangleScale;
				ofVec3f v1 = (p1 - pos) * triangleScale;
				ofVec3f v2 = (p2 - pos) * triangleScale;
				
				pos += normal * ofRandom(0.01, 0.02) * forcesStrength; // move in the direction of the triangle normal
				pos += ofVec3f(0, 0, 0.02); // Wind
				
				ofMatrix4x4 transform;
				transform.glRotate(ofRandom(-15, 15) * forcesStrength, 0, 1, 0);
				transform.glRotate(ofRandom(-15, 15) * forcesStrength, 0, 0, 1);
				
				p0 = pos + (v0 * transform);
				p1 = pos + (v1 * transform);
				p2 = pos + (v2 * transform);
				
				mesh.getVerticesPointer()[i0] = p0;
				mesh.getVerticesPointer()[i1] = p1;
				mesh.getVerticesPointer()[i2] = p2;
				
				mesh.getNormalsPointer()[i0] = normal;
				mesh.getNormalsPointer()[i1] = normal;
				mesh.getNormalsPointer()[i2] = normal;
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
	
			for (auto &mesh : meshes)
			{
				mesh.draw();
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