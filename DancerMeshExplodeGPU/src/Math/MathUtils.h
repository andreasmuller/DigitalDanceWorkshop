#pragma once

#include "ofMain.h"
#include <tuple>

class MathUtils
{
	
	public:
	
		// ------------------------------------------------------------
		// Step functions
		// ------------------------------------------------------------
	
		// ------------------------------------------------------------
		static float step(float a, float x)
		{
			return (float) (x >= a);
		}
	
		// ------------------------------------------------------------
		static float linearStep( float _edge0, float _edge1, float _t )
		{
			// Scale, and clamp x to 0..1 range
			return ofClamp( (_t - _edge0)/(_edge1 - _edge0), 0.0f, 1.0f);
		}

		// ------------------------------------------------------------
		static float linearStepOut( float _high1, float _low1, float _t)
		{
			return  (1.0f - linearStep(_high1, _low1, _t));
		}

		// ------------------------------------------------------------
		static float linearStepInOut( float _low0, float _high0, float _high1, float _low1, float _t )
		{
			return linearStep( _low0, _high0, _t ) * (1.0f - linearStep( _high1, _low1, _t ));
		}

		// ------------------------------------------------------------
		static float smoothStep(float edge0, float edge1, float x)
		{
			// Scale, and clamp x to 0..1 range
			x = ofClamp( (x - edge0)/(edge1 - edge0), 0, 1);
			// Evaluate polynomial
			return x*x*x*(x*(x*6 - 15) + 10);
		}

		// ------------------------------------------------------------
		static float smoothStepOut( float _high1, float _low1, float _t)
		{
			return (1.0f - smoothStep(_high1, _low1, _t));
		}

		// ------------------------------------------------------------
		static float smoothStepInOut( float _low0, float _high0, float _high1, float _low1, float _t )
		{
			return smoothStep( _low0, _high0, _t ) * (1.0f - smoothStep( _high1, _low1, _t ));
		}

		// ------------------------------------------------------------
		// From https://en.wikipedia.org/wiki/Smoothstep
		float smootherstep(float edge0, float edge1, float x)
		{
			// Scale, and clamp x to 0..1 range
			x = ofClamp((x - edge0)/(edge1 - edge0), 0.0, 1.0);
			// Evaluate polynomial
			return x*x*x*(x*(x*6 - 15) + 10);
		}

		// ------------------------------------------------------------
		// Shaping functions
		// ------------------------------------------------------------

		// ------------------------------------------------------------
		static float pulseSquare( float _frequency, float _width, float _t )
		{
			return 1 - step( _width, fmodf( _t, _frequency ) );
		}
		
		// ------------------------------------------------------------
		static float pulseTriangle( float _frequency, float _width, float _t )
		{
			float triangleT = fmodf( _t, _frequency ) / _width * 2.0;
			return (1.0 - fabs(fmodf(triangleT,2.0) - 1.0)) * pulseSquare( _frequency, _width, _t );
		}
		
		// ------------------------------------------------------------
		static float pulseLineDownUp( float _frequency, float _width, float _t )
		{
			float tmpVal = fmodf( _t, _frequency ) / _width;
			return tmpVal * (1 - step( 1.0, tmpVal ));
		}
		
		// ------------------------------------------------------------
		static float pulseLineUpDown( float _frequency, float _width, float _t )
		{
			float tmpVal = 1 - (fmodf( _t, _frequency ) / _width);
			return ofClamp( tmpVal * (1 - step( 1.0, tmpVal )), 0, 1);
		}
		
		// ------------------------------------------------------------
		static float pulseSawTooth( float _frequency, float _width, float _t )
		{
			float tmpVal = 1 - (fmodf( _t, _frequency ) / _width);
			return ofClamp( tmpVal * (1 - step( 1.0, tmpVal )), 0, 1);
		}
		
		// ------------------------------------------------------------
		static float pulseSine( float _frequency, float _width, float _t )
		{
			float tmpVal = ofClamp( (fmodf( _t, _frequency ) / _width), 0, 1);
			return sinf(tmpVal * PI);
		}
		
		// -----------------------------------------------------------
		static float pulseSmoothStep( float _frequency, float _x0, float _x1, float _x2, float _x3, float _t )
		{
			float tmpT = fmodf( _t, _frequency );
			return smoothStepInOut( _x0, _x1, _x2, _x3, tmpT );
		}
		
		// -----------------------------------------------------------
		static float pulseLinearStep( float _frequency, float _x0, float _x1, float _x2, float _x3, float _t )
		{
			float tmpT = fmodf( _t, _frequency );
			return linearStepInOut( _x0, _x1, _x2, _x3, tmpT ) ;
		}
	


		// ------------------------------------------------------------
		// Misc
		// ------------------------------------------------------------
		
		//-----------------------------------------
		// From http://www.flong.com/texts/code/shapers_exp/
		static float exponentialEasing(float x, float a)
		{
			float epsilon = 0.00001;
			float min_param_a = 0.0 + epsilon;
			float max_param_a = 1.0 - epsilon;
			a = max(min_param_a, min(max_param_a, a));
  
			if (a < 0.5)
			{
				// emphasis
				a = 2.0*(a);
				float y = pow(x, a);
				return y;
			} 
			else 
			{
				// de-emphasis
				a = 2.0*(a-0.5);
				float y = pow(x, 1.0/(1-a));
				return y;
			}
		}

		static int roundUp(int numToRound, int multiple) 
		{
			return ((numToRound + multiple - 1) / multiple) * multiple;
		}

		// ------------------------------------------------------------
		static float horizontalToVerticalFov(float _horizontalfov, float _aspect ) // aspect = w/h
		{
			_horizontalfov = ofDegToRad(_horizontalfov);
			float yfov = 2.0f * atanf(tanf(_horizontalfov * 0.5f) / _aspect);
			return ofRadToDeg(yfov);
		}
	
		// ------------------------------------------------------------
		static float verticalToHorizontalFov(float _verticalFov, float _aspect)
		{
			_verticalFov = ofDegToRad(_verticalFov);
			float xfov = 2.0f * atanf(tanf(_verticalFov * 0.5f) * _aspect);
			return ofRadToDeg(xfov);
		}

		// ------------------------------------------------------------	
		static unsigned int permuteQPR(unsigned int x)
		{
			static const unsigned int prime = 4294967291u;
			if (x >= prime)
				return x;  // The 5 integers out of range are mapped to themselves.
			unsigned int residue = ((unsigned long long) x * x) % prime;
			return (x <= prime / 2) ? residue : prime - residue;
		}
	
		// ------------------------------------------------------------
		static ofVec3f closestPointOnRay(  const ofVec3f _rayStart, const ofVec3f _rayDir, const ofVec3f _point )
		{
			float tmpT = 0.0;
			return closestPointOnRay( _rayStart, _rayDir, _point, tmpT );
		}
	
		// ------------------------------------------------------------
		static ofVec3f closestPointOnRay(  const ofVec3f _rayStart, const ofVec3f _rayDir, const ofVec3f _point,  float& _t )
		{
			ofVec3f startToPoint = _point - _rayStart;
			_t = _rayDir.dot(startToPoint);
			return _rayStart + (_t * _rayDir);
		}

		// ------------------------------------------------------------
		static ofVec3f closestPointOnLine(  const ofVec3f _a, const ofVec3f _b, const ofVec3f _point )
		{
			float tmpT = 0.0;
			return closestPointOnLine( _a, _b, _point, tmpT );
		}
	
		// ------------------------------------------------------------
		static ofVec3f closestPointOnLine(  const ofVec3f _a, const ofVec3f _b, const ofVec3f _point,  float& _t )
		{
			ofVec3f ap = _point - _a;			// vector from a to point
			ofVec3f ab = _b - _a;				// vector from a to b
			float d = ab.length();				// = Distance( a, b);  // length Line
			ab /= d;							// normalized direction vector from a to b
			_t = ab.dot(ap);				// ab is unit vector, t is distance from a to point projected on line ab
			
			if( _t <= 0 ) { return _a; } // point projected on line ab is out of line, closest point on line is a
			if( _t >= d ) { return _b; } // point projected on line ab is out of line, closest point on line is b
			
			return _a + (_t * ab);
		}
	
		// ------------------------------------------------------------
		static bool isInsideEllipse( ofVec2f _p, ofRectangle _ellipseRectangle )
		{
			if( _ellipseRectangle.inside( _p ) )
			{
				ofVec2f center = _ellipseRectangle.getPosition() + (ofVec2f(_ellipseRectangle.getWidth(), _ellipseRectangle.getHeight()) * 0.5f);
				
				float _xRadius = _ellipseRectangle.width * 0.5f;
				float _yRadius = _ellipseRectangle.height * 0.5f;
				
				if (_xRadius <= 0.0 || _yRadius <= 0.0)
					return false;
				
				ofVec2f normalized = ofVec2f(_p.x - center.x,
											 _p.y - center.y);
				
				return ((normalized.x * normalized.x) / (_xRadius * _xRadius)) + ((normalized.y * normalized.y) / (_yRadius * _yRadius))
						<= 1.0;
			}
			
			return false;
		}

		// ------------------------------------------------------------
		static ofVec3f randomPointOnSphere()
		{
			float lambda = ofRandom(1.0f);
			float u = ofRandom(-1.0f, 1.0f);
			float phi = ofRandom( 2.0 * PI );
			
			ofVec3f p;
			p.x = pow(lambda, 1/3) * sqrt(1.0 - u * u) * cos(phi);
			p.y = pow(lambda, 1/3) * sqrt(1.0 - u * u) * sin(phi);
			p.z = pow(lambda, 1/3) * u;
			
			return p;
		}

		// ------------------------------------------------------------
		static float getTriangleArea( ofVec3f _p0, ofVec3f _p1, ofVec3f _p2 )
		{
			ofVec3f triangle1Side1 = _p0 - _p1;
			ofVec3f triangle1Side2 = _p0 - _p2;
			ofVec3f triangle1Side3 = _p1 - _p2;
			float P1 = (triangle1Side1.length() + triangle1Side2.length() + triangle1Side3.length() ) / 2.0f;
			return (float)sqrt(P1 * (P1-triangle1Side1.length()) * (P1-triangle1Side2.length()) * (P1-triangle1Side3.length()) );
		}

		// ------------------------------------------------------------
		static ofVec3f getTriangleNormal( ofVec3f _p0, ofVec3f _p1, ofVec3f _p2 )
		{
			ofVec3f v1 = _p1 - _p0;
			ofVec3f v2 = _p2 - _p0;
			return v1.getCrossed( v2 ).getNormalized();
		}

		// ------------------------------------------------------------
		// Compute barycentric coordinates (u, v, w) for
		// point p with respect to triangle (a, b, c)
		//
		static ofVec3f barycentric(ofVec3f _p, ofVec3f _a, ofVec3f _b, ofVec3f _c )
		{
			ofVec3f v0 = _b - _a;
			ofVec3f v1 = _c - _a;
			ofVec3f v2 = _p - _a;

			float d00 = v0.dot(v0);
			float d01 = v0.dot(v1);
			float d11 = v1.dot(v1);
			float d20 = v2.dot(v0);
			float d21 = v2.dot(v1);
			float denom = d00 * d11 - d01 * d01;

			float v = (d11 * d20 - d01 * d21) / denom;
			float w = (d00 * d21 - d01 * d20) / denom;
			float u = 1.0f - v - w;	

			return ofVec3f(u,v,w);
		}

		// ------------------------------------------------------------
		static tuple<bool,ofVec2f> isInsideTriangle( ofVec3f _p, ofVec3f _a, ofVec3f _b, ofVec3f _c )
		{
			// Compute vectors        
			ofVec3f v0 = _c - _a;
			ofVec3f v1 = _b - _a;
			ofVec3f v2 = _p - _a;

			// Compute dot products
			float dot00 = v0.dot(v0);
			float dot01 = v0.dot(v1);
			float dot02 = v0.dot(v2);
			float dot11 = v1.dot(v1);
			float dot12 = v1.dot(v2);

			// Compute barycentric coordinates
			float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
			float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
			float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

			// Check if point is in triangle
			bool isInside = (u >= 0) && (v >= 0) && (u + v < 1);

			return make_tuple( isInside, ofVec2f(u,v) );
		}

		// ------------------------------------------------------------
		static ofVec3f noiseVelocity( ofVec3f _pos, float _time )
		{
			ofVec3f vel;
			vel.x += ofSignedNoise( _pos.x, _pos.y, _pos.z, _time );
			vel.y += ofSignedNoise( _pos.y, _pos.z, _time,	_pos.x );
			vel.z += ofSignedNoise( _pos.z, _time,	_pos.x, _pos.y );
			return vel;
		}
	
		// ------------------------------------------------------------
		template<class Vec>
		static float fbm( Vec _loc, int _octaves, float _lacunarity = 2.0, float _persistence = 0.5)
		{
			return (signedFbm( _loc, _octaves, _lacunarity, _persistence ) + 1.0) * 0.5;
		}
	
		// ------------------------------------------------------------
		template<class Vec>
		static float signedFbm( Vec _loc, int _octaves, float _lacunarity = 2.0, float _persistence = 0.5 )
		{
			float finalNoise = 0.0;
			float amplitude = 1.0;
			float totalAmplitude = 0.0;
			Vec tmpLoc = _loc;
			
			for( int i = 0; i < _octaves; i++)
			{
				amplitude *= _persistence;
				totalAmplitude += amplitude;
				float layerNoise = signedNoise(tmpLoc);
				finalNoise += layerNoise * amplitude;
				tmpLoc *= _lacunarity; // //sum += amp * snoise(pp);
			}
			
			return finalNoise / totalAmplitude;
		}

		// ------------------------------------------------------------
		// Noise shortcuts
		// ------------------------------------------------------------
	
		// ------------------------------------------------------------
		static float noise( float _p ) { return ofNoise( _p ); }
	
		// ------------------------------------------------------------
		static float noise( ofVec2f _p ) { return ofNoise( _p.x, _p.y ); }
		
		// ------------------------------------------------------------
		static float noise( ofVec3f _p ) { return ofNoise( _p.x, _p.y, _p.z ); }
		
		// ------------------------------------------------------------
		static float noise( ofVec4f _p ) { return ofNoise( _p.x, _p.y, _p.z, _p.w ); }
	
		// ------------------------------------------------------------
		static float signedNoise( float _p ) { return ofSignedNoise( _p ); }
		
		// ------------------------------------------------------------
		static float signedNoise( ofVec2f _p ) { return ofSignedNoise( _p.x, _p.y ); }
		
		// ------------------------------------------------------------
		static float signedNoise( ofVec3f _p ) { return ofSignedNoise( _p.x, _p.y, _p.z ); }
		
		// ------------------------------------------------------------
		static	float signedNoise( ofVec4f _p ) { return ofSignedNoise( _p.x, _p.y, _p.z, _p.w ); }
	
	private:
};
