#pragma once

#include "ofMain.h"

class PlacementUtils
{
	public:

		static ofRectangle getDrawPos( int _index, ofRectangle _itemDimensions, ofRectangle _bounds, ofVec2f _padding = ofVec2f(0) )
		{
			ofRectangle fullRect = _itemDimensions;
			fullRect.width  += _padding.x * 2;
			fullRect.height += _padding.y * 2;

			int maxNumX = floor( _bounds.width / fullRect.width );

			int indexX = _index % maxNumX;
			int indexY = floor(_index / maxNumX);

			ofRectangle drawRect = ofRectangle( _bounds.x + (indexX * fullRect.width), 
												_bounds.y + (indexY * fullRect.height), 
												_itemDimensions.width, 
												_itemDimensions.height );
			drawRect.x += _padding.x;
			drawRect.y += _padding.y;

			return drawRect;
		}
};

/*
// ------------------------------------------------------------------------------
//
class BoxPlacement
{
	// -------------------------------------------------
	void init( ofRectangle _bounds )
	{
		bounds = _bounds;
		lastDrawPos = bounds.getPosition();
	}

	// -------------------------------------------------
	ofVec2f getDrawPos( ofRectangle _itemDimensions, ofVec2f _padding = ofVec2f(0) )
	{
		ofRectangle fullRect = _itemDimensions;
		fullRect.width  += _padding.x * 2;
		fullRect.height += _padding.y * 2;

		if( lastDrawPos.x + fullRect.width > bounds.x + bounds.width )
		{
			
		}

		return;
	}

	ofRectangle bounds;
	ofVec2f lastDrawPos;
};
*/