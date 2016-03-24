#pragma once

#include "ofMain.h"
#include "Utils/FboPingPong.h"

#include "ofMaterialExt.h"
#include "ofLightExt.h"

// -------------------------------------------------------------------------------------------------------------
//
class MeshShaderData
{
	public:

		// ------------------------------------------------
		MeshShaderData()
		{
			meshAge = 0;
			meshMaxAge = 1;
			timeReceivedMesh = 0;

			maxRotation.set("Max Rotation", 3, 0, 20);
			triangleNormalVel.set("Triangle Normal Vel", 0.001, 0, 0.1);
			triangleNormalDrag.set("Triangle Normal Drag", 1, 0, 1) ;
			
			noisePositionFrequency.set("Noise Pos Freq", 0.1, 0, 1);
			noiseMagnitude.set("Noise Magnitude", 0.001, 0, 0.1);
			noiseTimeFrequency.set("Noise Time Freq", 1, 0, 5);
			noisePersistence.set("Noise Persistence", 0.3, 0, 1);
			
			wind.set("Wind", ofVec3f(0,0,-0.01), ofVec3f(-1), ofVec3f(1));
		}
		
		// ------------------------------------------------
		void addToUI( ofxPanel& _panel )
		{
			_panel.add( maxRotation );
		}

		// ------------------------------------------------
		void newMesh(ofMesh& _triangleMesh, float _meshMaxAge = 1.0f )
		{
			meshMaxAge = _meshMaxAge;

			// We are blindly assuming that we're getting a triangle mesh, we should really be 
			// checking mesh primitive type and go by indices if they exist
			if (_triangleMesh.getNumVertices() % 3 != 0)
			{
				return;
			}

			trianglePos.clear();

			triangleV0.clear();
			triangleV1.clear();
			triangleV2.clear();

			int numTriangles = _triangleMesh.getNumVertices() / 3;
			int textureSize = ceil(sqrtf(numTriangles));

			pointsMesh.clear();
			pointsMesh.setMode( OF_PRIMITIVE_POINTS ); 

			for (int i = 0; i < numTriangles; i++)
			{
				int i0 = (i * 3) + 0;
				int i1 = (i * 3) + 1;
				int i2 = (i * 3) + 2;

				ofVec3f v0 = _triangleMesh.getVertex(i0);
				ofVec3f v1 = _triangleMesh.getVertex(i1);
				ofVec3f v2 = _triangleMesh.getVertex(i2);

				ofVec3f midPoint = (v0 + v1 + v2) / 3.0f;

				ofVec3f localV0 = v0 - midPoint;
				ofVec3f localV1 = v1 - midPoint;
				ofVec3f localV2 = v2 - midPoint;

				ofVec2f uv;
				uv.x = (i % textureSize) / (float)textureSize;
				uv.y = floor(i / textureSize) / (float)textureSize;

				pointsMesh.addVertex(midPoint); // This vertex position could really be anything as we will later read it from our data FBO
				pointsMesh.addTexCoord( uv );	// We'll use this though when sampling from the data textures

				trianglePos.push_back(midPoint);

				triangleV0.push_back(localV0);
				triangleV1.push_back(localV1);
				triangleV2.push_back(localV2);
			}

			// Upload the data to textures

			// Add some padding 
			while (trianglePos.size() < (textureSize*textureSize)) { trianglePos.push_back(ofVec3f(0)); }
			while (triangleV0.size()  < (textureSize*textureSize)) { triangleV0.push_back(ofVec3f(0)); }
			while (triangleV1.size()  < (textureSize*textureSize)) { triangleV1.push_back(ofVec3f(0)); }
			while (triangleV2.size()  < (textureSize*textureSize)) { triangleV2.push_back(ofVec3f(0)); }

			// Allocate data textures and FBO if needed
			bool allocate = !posAndAngles.isAllocated();
			if(posAndAngles.isAllocated() ) 
			{ 
				if ((int)posAndAngles.source()->getWidth() != textureSize) { allocate = true; } 
			}

			if ( allocate )
			{
				posAndAngles.allocateAsData(textureSize, textureSize, GL_RGBA32F, 2); // We could allocate more buffers here if we wanted to store vel or other data

				v0.allocate(textureSize, textureSize, GL_RGBA32F, false);
				v1.allocate(textureSize, textureSize, GL_RGBA32F, false);
				v2.allocate(textureSize, textureSize, GL_RGBA32F, false);
				
				v0.setTextureMinMagFilter( GL_NEAREST, GL_NEAREST );
				v1.setTextureMinMagFilter( GL_NEAREST, GL_NEAREST );
				v2.setTextureMinMagFilter( GL_NEAREST, GL_NEAREST );

				// Some random values for each 'particle' to use, never leave home without them 
				randomValues.clear();
				for (int i = 0; i < (textureSize*textureSize); i++) { randomValues.push_back( ofVec4f(ofRandom(1.0f), ofRandom(1.0f), ofRandom(1.0f), ofRandom(1.0f)) );  }

				startAngles.clear();
				for (int i = 0; i < (textureSize*textureSize); i++) { startAngles.push_back(ofVec4f(0)); }

				random.allocate(textureSize, textureSize, GL_RGBA32F, false);
				random.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
				random.loadData(&randomValues.at(0).x, textureSize, textureSize, GL_RGBA);

				//cout << "Allocated to " << textureSize << endl;
			}

			// Upload
			posAndAngles.source()->getTexture(0).loadData( &trianglePos.at(0).x, textureSize, textureSize, GL_RGB);
			posAndAngles.source()->getTexture(1).loadData( &startAngles.at(0).x, textureSize, textureSize, GL_RGBA);

			v0.loadData( &triangleV0.at(0).x, textureSize, textureSize, GL_RGB );
			v1.loadData( &triangleV1.at(0).x, textureSize, textureSize, GL_RGB );
			v2.loadData( &triangleV2.at(0).x, textureSize, textureSize, GL_RGB );

			ofLogLevel tmpLevel = ofGetLogLevel();
			ofSetLogLevel( OF_LOG_VERBOSE );

			// TODO: This shader should really be shared among all MeshShaderData instances
			if ( !drawShader.isLoaded() )
			{
				drawShader.load("Shaders/MeshExplode/GL3/Draw.vert", "Shaders/MeshExplode/GL3/Draw.frag", "Shaders/MeshExplode/GL3/Draw.geom");
				updateShader.load("Shaders/MeshExplode/GL3/Update");
			}
			ofSetLogLevel(tmpLevel);

			timeReceivedMesh = ofGetElapsedTimef();
		}

		// ------------------------------------------------
		void update()
		{
			if (!posAndAngles.isAllocated())
			{
				return;
			}

			float t = ofGetElapsedTimef();
			meshAge = t - timeReceivedMesh;

			ofDisableTextureEdgeHack(); // Important on devices that don't support NPOT textures!
			ofSetColor(ofColor::white);
			ofEnableBlendMode( OF_BLENDMODE_DISABLED );

			posAndAngles.dest()->begin();

				posAndAngles.dest()->activateAllDrawBuffers(); // if we have multiple color buffers in our FBO we need this to activate all of them

				updateShader.begin();
				
					updateShader.setUniformTexture("posTex", posAndAngles.source()->getTexture(0), 0);
					updateShader.setUniformTexture("angTex", posAndAngles.source()->getTexture(1), 1);

					updateShader.setUniformTexture("vertex0Tex", v0, 2);
					updateShader.setUniformTexture("vertex1Tex", v1, 3);
					updateShader.setUniformTexture("vertex2Tex", v2, 4);

					updateShader.setUniformTexture("randomTex", random, 5);

					updateShader.setUniform1f( "time", t );
			
					updateShader.setUniform1f( "meshAge", meshAge );
					updateShader.setUniform1f("meshMaxAge", meshMaxAge);

					updateShader.setUniform1f("maxRotation", ofDegToRad(maxRotation) );
			
					updateShader.setUniform1f("triangleNormalVel", triangleNormalVel);
					updateShader.setUniform1f("triangleNormalDrag", triangleNormalDrag);
			
					updateShader.setUniform1f("noisePositionFrequency", noisePositionFrequency);
					updateShader.setUniform1f("noiseMagnitude", noiseMagnitude);
					updateShader.setUniform1f("noiseTimeFrequency", noiseTimeFrequency);
					updateShader.setUniform1f("noisePersistence", noisePersistence);
			
					updateShader.setUniform3fv("wind", wind.get().getPtr() );
			
					posAndAngles.source()->draw(0, 0);
				
				updateShader.end();

			posAndAngles.dest()->end();

			posAndAngles.swap(); // posAndAngles.source() has the new data now
			
		}

		// ------------------------------------------------
		void draw()
		{
			if (!posAndAngles.isAllocated())
			{
				return;
			}

			drawShader.begin();

				drawShader.setUniformTexture("posTex", posAndAngles.source()->getTexture(0), 0);
				drawShader.setUniformTexture("angTex", posAndAngles.source()->getTexture(1), 1);

				drawShader.setUniformTexture("vertex0Tex", v0, 2);
				drawShader.setUniformTexture("vertex1Tex", v1, 3);
				drawShader.setUniformTexture("vertex2Tex", v2, 4);

				drawShader.setUniformTexture("randomTex", random, 5);

				drawShader.setUniform1f("meshAge", meshAge);
				drawShader.setUniform1f("meshMaxAge", meshMaxAge);

				pointsMesh.draw();

			drawShader.end();
		}

		ofMesh pointsMesh;

		ofShader drawShader;
		ofShader updateShader;

		FboPingPong posAndAngles;

		ofTexture v0;
		ofTexture v1;
		ofTexture v2;

		ofTexture random;

		float meshAge;
		float meshMaxAge;
		float timeReceivedMesh;

		ofParameter<float> maxRotation;
		ofParameter<float> triangleNormalVel;
		ofParameter<float> triangleNormalDrag;

		ofParameter<float> noisePositionFrequency;
		ofParameter<float> noiseMagnitude;
		ofParameter<float> noiseTimeFrequency;
		ofParameter<float> noisePersistence;
	
		ofParameter<ofVec3f> wind;

		vector<ofVec3f> trianglePos;
		vector<ofVec3f> triangleV0;
		vector<ofVec3f> triangleV1;
		vector<ofVec3f> triangleV2;

		vector<ofVec4f> randomValues;

		vector<ofVec4f> startAngles;
};