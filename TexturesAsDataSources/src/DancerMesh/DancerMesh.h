
#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include <random>

#include "../Math/MathUtils.h"
#include "../Utils/ofTrueTypeFontExt.h"

#include "MeshPoint.h"


// ----------------------------------------------------
class DancerMesh
{
	public:

		DancerMesh();

		// Methods ----------------------------------------------- 
		virtual bool			load( string _inputFilePath, string _texture = "white" ) = 0;

		virtual void			addToPanel( ofxPanel* _panel ) = 0;

		void					initTexture( string _texture );

		virtual void			update( float _time ) = 0;

		virtual float			getAnimationLength() { return -1; }

		virtual void			debugDraw() = 0;

		virtual void			debugDrawInfo( ofTrueTypeFontExt& _font, ofVec2f _pos ) = 0;

		virtual	void			updateTriangleMesh( ofMesh& _mesh ) = 0;
	
		void					updateRandomPoints(int _numPoints, vector<MeshPoint>& _newRandomPoints, int _seed = -1 );
		void					updateRandomPoints(int _numPoints, vector<MeshPoint>& _newRandomPoints, ofImage& _emissionMask, int _seed = -1 );
		void					updateRandomPoints( int _numPoints, vector<MeshPoint>& _newRandomPoints, ofMesh& _sourceTriangleMesh,
													   piecewise_constant_distribution<>& _dist, ofImage& _emissionMask, int _randomSeed = -1);

		void					initWeightedDistribution( piecewise_constant_distribution<>& _dist, ofMesh& _emissionMesh );

		static vector<ofVec2f>	getRandomUVPointsWithinMaskedArea(int _numPoints, ofImage& _mask, ofColor _searchColor, float _maxColorDist);
		static vector<ofVec2f>	getRandomUVPointsWithinMaskedArea(int _numPoints, string _maskPath, ofColor _searchColor, float _maxColorDist);

		void					findTriangleParamsForStickyPoints(vector<MeshPoint>& _stickyPoints);
		void					findTriangleParamsForStickyPoints( vector<MeshPoint>& _stickyPoints, ofMesh& _triangleMesh );
		void					updateStickyPoints(vector<MeshPoint>& _stickyPoints);
		void					updateStickyPoints( vector<MeshPoint>& _stickyPoints, ofMesh& _triangleMesh, ofMesh& _prevTriangleMesh );

		void					setBaseTransform(ofMatrix4x4 _meshBaseTransform)	{ meshBaseTransform = _meshBaseTransform; }
		ofMatrix4x4				getBaseTransform()									{ return meshBaseTransform; }
	
		static void				drawNormals( vector<MeshPoint>& _meshPoints, float _normalScale  = 1.0f );
		static void				drawVelocities( vector<MeshPoint>& _meshPoints, float _velScale  = 1.0f );
	
		// Properties ---------------------------------------------
		string					name;
		bool					loaded;

		ofVideoPlayer			meshTextureVideoPlayer;
		ofImage					meshTexture;

		ofMesh					triangleMesh;
		ofMesh					prevTriangleMesh;
		vector<ofVec3f>			currVertexVelocities;
	
		//vector<ofVec3f>			currVertexVelocities;
		//vector<StickyPoint>		midPointStickyPoints;

		float					normalDebugScale;

		ofVec3f					modelMiddle;

		string					eyeMeshName;
		vector<string>			meshSkipList;

		ofNode					headNode;
		ofNode					lookingAtTargetNode;
		ofNode					interpolatedLookAtNode;
		float					distToTarget;
		float					distWithNodeScale;

		float					angleBetweenCurrentHeadOrientationAndLookAt;
		float					lookAtLerpAmount;

		bool					supportsLookAt;
		ofParameter<bool>		lookAt;
		ofParameter<float>		minAngleBeforeTurningTowards;
		ofParameter<float>		maxAngleBeforeTurningTowards;
		ofParameter<ofVec3f>	modelLookAtTargetPos;
		ofParameter<float>		maxLookAtFrac;

	protected:

		piecewise_constant_distribution<> triangleAreaWeightedDistribution;

		ofMatrix4x4				meshBaseTransform;
		ofImage					emptyMask;
};