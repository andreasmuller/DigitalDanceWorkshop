
#pragma once

#include "ofMain.h"

// ----------------------------------------------------
class StickyPoint
{
	public:

		StickyPoint( ofVec2f _uv = ofVec2f(0,0) )
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
		ofVec3f currentPos;
		ofVec3f currentNormal;
};
