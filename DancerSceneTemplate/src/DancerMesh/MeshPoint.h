
#pragma once

#include "ofMain.h"

// ----------------------------------------------------
class MeshPoint
{
	public:

		MeshPoint( ofVec2f _uv = ofVec2f(0,0) )
		{
			uv = _uv;
			triangleID = -1;
			barycentric1 = 0;
			barycentric2 = 0;
		}

		ofVec2f uv;
		int triangleID;
		float barycentric1;
		float barycentric2;
		ofVec3f pos;
		ofVec3f normal;
		ofVec3f vel;
};
