#pragma once

#include "ofMain.h"
#include "Utils/FboPingPong.h"

// -------------------------------------------------------------------------------------------------------------
//
class MeshShaderData
{
	public:

		// ------------------------------------------------
		void newMesh( ofMesh& _triangleMesh )
		{
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

				pointsMesh.addVertex(midPoint); // this vertex position could really be anything as we will later read it from our data FBO

				trianglePos.push_back(midPoint);

				triangleV0.push_back(localV0);
				triangleV1.push_back(localV1);
				triangleV2.push_back(localV2);
			}

			// Upload the data to textures
			int textureSize = ceil(sqrtf(trianglePos.size()));

			// Add some padding 
			while (trianglePos.size() < (textureSize*textureSize)) { trianglePos.push_back(ofVec3f(0)); }
			while (triangleV0.size()  < (textureSize*textureSize)) { triangleV0.push_back(ofVec3f(0)); }
			while (triangleV1.size()  < (textureSize*textureSize)) { triangleV1.push_back(ofVec3f(0)); }
			while (triangleV2.size()  < (textureSize*textureSize)) { triangleV2.push_back(ofVec3f(0)); }

			// Allocate data textures and FBO if needed
			bool allocate = !pos.isAllocated();
			if( pos.isAllocated() ) {  if ((int)pos.source()->getWidth() != textureSize) { allocate = true; } }

			if ( allocate )
			{
				pos.allocateAsData(textureSize, textureSize, GL_RGBA32F, 1); // We could allocate more buffers here if we wanted to store vel or other data

				v0.allocate(textureSize, textureSize, GL_RGBA32F, false);
				v1.allocate(textureSize, textureSize, GL_RGBA32F, false);
				v2.allocate(textureSize, textureSize, GL_RGBA32F, false);
				
				v0.setTextureMinMagFilter( GL_NEAREST, GL_NEAREST );
				v1.setTextureMinMagFilter( GL_NEAREST, GL_NEAREST );
				v2.setTextureMinMagFilter( GL_NEAREST, GL_NEAREST );
			}

			// Upload
			pos.source()->getTexture(0).loadData( &trianglePos.at(0).x, textureSize, textureSize, GL_RGB);

			v0.loadData( &triangleV0.at(0).x, textureSize, textureSize, GL_RGB );
			v1.loadData( &triangleV1.at(0).x, textureSize, textureSize, GL_RGB );
			v2.loadData( &triangleV2.at(0).x, textureSize, textureSize, GL_RGB );

			if ( !shader.isLoaded() )
			{

			}
		}

		// ------------------------------------------------
		void draw()
		{
			pointsMesh.draw();
		}

		ofMesh pointsMesh;

		ofShader shader;

		FboPingPong pos;

		ofTexture v0;
		ofTexture v1;
		ofTexture v2;

		vector<ofVec3f> trianglePos;
		vector<ofVec3f> triangleV0;
		vector<ofVec3f> triangleV1;
		vector<ofVec3f> triangleV2;
};