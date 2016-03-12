
#pragma once

// ------------------------------------------------------------------------------------------------------------
//
class ofVec2i
{
	public:

		ofVec2i( int _x, int _y )
		{
			x = _x;
			y = _y;
		}

		ofVec2i( int _num = 0 )
		{
			x = _num;
			y = _num;
		}

		int x,y;
};

// ------------------------------------------------------------------------------------------------------------
//
class ofVec3i
{
	public:

		ofVec3i( int _x, int _y, int _z )
		{
			x = _x;
			y = _y;
			z = _z;
		}

		ofVec3i( int _num = 0 )
		{
			x = _num;
			y = _num;
			z = _num;
		}

		int x,y,z;
};

// ------------------------------------------------------------------------------------------------------------
//
class ofVec4i
{
	public:

		ofVec4i( int _x, int _y, int _z ,int _w )
		{
			x = _x;
			y = _y;
			z = _z;
			w = _w;
		}

		ofVec4i( int _num = 0 )
		{
			x = _num;
			y = _num;
			z = _num;
			w = _num;
		}

		int x,y,z,w;
};

// ------------------------------------------------------------------------------------------------------------
//
class ofVec4ui
{
	public:

		ofVec4ui( int _x, int _y, int _z ,int _w )
		{
			x = _x;
			y = _y;
			z = _z;
			w = _w;
		}

		ofVec4ui( int _num = 0 )
		{
			x = _num;
			y = _num;
			z = _num;
			w = _num;
		}

		unsigned int x,y,z,w;
};