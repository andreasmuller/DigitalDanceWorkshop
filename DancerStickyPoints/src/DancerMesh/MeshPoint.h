
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

		ofVec2f uv;
		int triangleID;
		float barycentric1;
		float barycentric2;
		ofVec3f pos;
		ofVec3f normal;
		ofVec3f vel;
};


// ----------------------------------------------------
// We'll save a history of points a fair amount,
// so here's a small helper class
//
class PositionAndNormalHistory
{
	public:
	
		// --------------------------------------------
		PositionAndNormalHistory( int _maxLength = 10 )
		{
			setMaxLength( _maxLength );
		}

		// --------------------------------------------
		void setMaxLength( int _maxLength )
		{
			maxLength = _maxLength;
			limitHistoryLength();
		}

		// --------------------------------------------
		int getMaxLength()
		{
			return maxLength;
		}
	
		// --------------------------------------------
		void add( ofVec3f _p, ofVec3f _n = ofVec3f(0,1,0), float _minDistance = 0.0f )
		{
			ofVec3f lastPointPos(9999); // If we dont have any points, set the one we compare to far away so the new point gets added
			if (positions.size() > 0) lastPointPos = positions.back();
			
			if(_p.distance(lastPointPos) > _minDistance )
			{
				positions.push_front( _p );
				normals.push_front( _n );
			}
			
			limitHistoryLength();
		}
	
		// --------------------------------------------
		deque<ofVec3f>& getPositions()
		{
			return positions;
		}

		// --------------------------------------------
		deque<ofVec3f>& getNormals()
		{
			return normals;
		}
	
		// --------------------------------------------
		// Ideally you can make everything accept deque<ofVec3f>
		// but otherwise here are some convenienve functions
		vector<ofVec3f> getPositionsAsVector()
		{
			vector<ofVec3f> tmp;
			for( int i = 0; i < positions.size(); i++ ) { tmp.push_back(positions.at(i));}
			return tmp;
		}
	
		// --------------------------------------------
		vector<ofVec3f> getNormalsAsVector()
		{
			vector<ofVec3f> tmp;
			for( int i = 0; i < normals.size(); i++ ) { tmp.push_back(normals.at(i));}
			return tmp;
		}

	protected:

		void limitHistoryLength()
		{
			// Limit to a certain amount
			while (positions.size() > maxLength) { positions.pop_back();  }
			while (normals.size()   > maxLength) { normals.pop_back();  }
		}
	
		int maxLength;
		
		deque<ofVec3f> positions;
		deque<ofVec3f> normals;
};
