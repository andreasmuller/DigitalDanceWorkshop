
#pragma once

#include "ofMain.h"

// ----------------------------------------------------
class MeshPoint
{
	public:

	
		// --------------------------------------------
		MeshPoint( ofVec2f _uv = ofVec2f(0,0) )
		{
			uv = _uv;
			triangleID = -1;
			barycentric1 = 0;
			barycentric2 = 0;
		}

		// --------------------------------------------
		static vector<ofVec3f> getPositions( vector<MeshPoint>& _meshPoints )
		{
			vector<ofVec3f> tmp;
			for( int i = 0; i < _meshPoints.size(); i++ )
			{
				tmp.push_back( _meshPoints.at(i).pos );
			}
			return tmp;
		}

		ofVec2f uv;
		int triangleID;
		float barycentric1;
		float barycentric2;
		ofVec3f pos;
		ofVec3f normal;
		ofVec3f vel;
};
