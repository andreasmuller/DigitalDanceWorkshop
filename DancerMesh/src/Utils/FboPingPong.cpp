//
//  FboPingPong.cpp
//  emptyExample
//
//  Created by Andreas Müller on 12/08/2013.
//
//

#include "FboPingPong.h"

// ------------------------------------------------------------------------------------
//
FboPingPong::FboPingPong()
{
	allocated = false;
}

// ------------------------------------------------------------------------------------
//
void FboPingPong::allocate( int _w, int _h, int _internalformat, ofColor _clearColor )
{
	ofFbo::Settings settings = ofFbo::Settings();
	settings.width  = _w;		
	settings.height = _h;
	settings.useDepth = true;
	settings.internalformat = _internalformat;
	
	allocate( settings, _clearColor );
}

// ------------------------------------------------------------------------------------
//
void FboPingPong::allocate( ofFbo::Settings _settings, ofColor _clearColor )
{
	clearColor = _clearColor;
	
	fbo1.allocate( _settings);
	fbo2.allocate( _settings );
	
	sourceBuffer = &fbo1;
	destBuffer = &fbo2;
	
	clearSource();
	clearDest();

	allocated = true;
}

// ------------------------------------------------------------------------------------
//
void FboPingPong::allocateAsData( int _w, int _h, int _internalformat, int _numColorBuffers )
{
	// Allocate buffers
	ofFbo::Settings fboSettings;
	fboSettings.width  = _w;
	fboSettings.height = _h;
	
	// We can create several color buffers for one FBO if we want to store velocity for instance,
	// then draw to them simultaneously from a shader using gl_FragData[0], gl_FragData[1], etc.
	fboSettings.numColorbuffers = _numColorBuffers;
	
	fboSettings.useDepth = false;
	fboSettings.internalformat = _internalformat;	// Gotta store the data as floats, they won't be clamped to 0..1
	fboSettings.textureTarget = GL_TEXTURE_2D;
	fboSettings.wrapModeHorizontal = GL_CLAMP_TO_EDGE;
	fboSettings.wrapModeVertical = GL_CLAMP_TO_EDGE;
	fboSettings.minFilter = GL_NEAREST; // No interpolation, that would mess up data reads later!
	fboSettings.maxFilter = GL_NEAREST;
	
	allocate( fboSettings );

	allocated = true;
}

// ------------------------------------------------------------------------------------
//
void FboPingPong::draw( ofPoint _pos, float _width, bool _drawBack )
{
	float desWidth = _width;
	float desHeight = (source()->getWidth() / source()->getHeight()) * desWidth;
	
	source()->draw( _pos, desWidth, desHeight );
	
	if( _drawBack )
	{
		dest()->draw( _pos + ofVec2f(desWidth,0), desWidth, desHeight );
	}
}

// ------------------------------------------------------------------------------------
//
void FboPingPong::clearBoth()
{
	clearSource();
	clearDest();
}

// ------------------------------------------------------------------------------------
//
void FboPingPong::clearBoth( ofColor _clearColor )
{
	clearSource( _clearColor );
	clearDest( _clearColor );
}

// ------------------------------------------------------------------------------------
//
void FboPingPong::clearSource()
{
	clearSource( clearColor );
}

// ------------------------------------------------------------------------------------
//
void FboPingPong::clearDest()
{
	clearDest( clearColor );
}

// ------------------------------------------------------------------------------------
//
void FboPingPong::clearSource( ofColor _clearColor )
{
	source()->begin();
		ofClear( _clearColor );
	source()->end();
}

// ------------------------------------------------------------------------------------
//
void FboPingPong::clearDest( ofColor _clearColor )
{
	dest()->begin();
		ofClear( _clearColor );
	dest()->end();
}

// ------------------------------------------------------------------------------------
//
void FboPingPong::setClearColor( ofColor _color )
{
	clearColor = _color;
}

// ------------------------------------------------------------------------------------
//
void FboPingPong::swap()
{
	std::swap(sourceBuffer, destBuffer);
}