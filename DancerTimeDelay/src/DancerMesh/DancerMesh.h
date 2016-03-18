
#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include <random>

#include "../Math/MathUtils.h"
#include "../Utils/ofTrueTypeFontExt.h"

#include "StickyPoint.h"


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
		void					updateRandomPointMesh( int _numPoints, ofMesh& _sourceTriangleMesh, ofMesh& _randomPointMesh, piecewise_constant_distribution<>& _dist, ofImage& _emissionMask, int _randomSeed );
		void					initWeightedDistribution( piecewise_constant_distribution<>& _dist, ofMesh& _emissionMesh );

		void					findTriangleParamsForStickyPoints( vector<StickyPoint>& _stickyPoints, ofMesh& _triangleMesh );
		void					updateStickyPoints( vector<StickyPoint>& _stickyPoints, ofMesh& _triangleMesh );

		void					setBaseTransform(ofMatrix4x4 _meshBaseTransform)	{ meshBaseTransform = _meshBaseTransform; }
		ofMatrix4x4				getBaseTransform()									{ return meshBaseTransform; }

		// Properties ---------------------------------------------
		string					name;
		bool					loaded;

		ofVideoPlayer			meshTextureVideoPlayer;
		ofImage					meshTexture;
		ofMesh					triangleMesh;
	
		vector<StickyPoint>		midPointStickyPoints;

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

		ofMatrix4x4				meshBaseTransform;
};