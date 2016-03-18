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
	timeBetweenCopies = 0.4;
	maxCopies = 6;

	drawGui = false;
}

//--------------------------------------------------------------
void ofApp::update() 
{    
	float t = ofGetElapsedTimef();

	dancerMesh.update( ofGetElapsedTimef() );

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

		//float meshIndexFrac = (meshIndex / (float)meshes.size());
		float meshTime = fmodf(t, timeBetweenCopies) + (meshIndex * timeBetweenCopies);
		float ageFrac = (meshTime / meshMaxAge);

		float forcesStrength = MathUtils::linearStep(meshMaxAge * 0.25, meshMaxAge * 0.6, meshTime );
		float triangleScale = (1.0f - MathUtils::linearStep(meshMaxAge * 0.6, meshMaxAge, meshTime)); //(1.0f - MathUtils::linearStep(_high1, _low1, _t)
		triangleScale = ofMap(triangleScale, 0, 1, 0.95, 1.0);

//triangleScale = 0.99;
//triangleScale = ageFrac

		//cout << "triangleScale" << meshIndex << ": " << triangleScale << endl;

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
				for (auto &mesh : meshes)
				{
					mesh.draw();
				}
			dancerMaterial.end();

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











