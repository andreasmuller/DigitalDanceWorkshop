//
//  Lathe.h
//  ofAppNew
//
//  Created by Andreas Müller on 13/03/2016.
//
//

#pragma once

#include "ofMain.h"

// ------------------------------------------------------------------------------------------
//
class Lathe
{
	public:
	
		// ------------------------------------------
		static void lathe( ofMesh& _mesh,
						   vector<ofVec2f>& _lengthPoints,
						   vector<ofVec2f>& _radiusPoints,
						   vector<ofMatrix4x4>& _skeletonTransforms,
						   vector<ofVec3f>& _faceNormalsBuffer )
		{
			if( _lengthPoints.size() == 0 || _radiusPoints.size() == 0 || _skeletonTransforms.size() == 0 )
			{
				ofLogError() << "Lathe::lathe empty control point buffer.";
				return;
			}
			
			_mesh.clear();
			_mesh.setMode( OF_PRIMITIVE_TRIANGLES );
			
			for( int transformIndex = 0; transformIndex < _skeletonTransforms.size(); transformIndex++ )
			{
				float frac = ofMap( transformIndex, 0, _skeletonTransforms.size()-1, 0, 1 );
				ofMatrix4x4 parentTransform = _skeletonTransforms[transformIndex];
				ofVec2f segmentLengthPoint = getLinearInterpolated( _lengthPoints, frac );
				
				float segmentHeight = segmentLengthPoint.y;
				
				for( int i = 0; i < _radiusPoints.size(); i++ )
				{
					ofVec2f p = _radiusPoints[i];
					ofVec3f rotatedPoint = (ofVec3f( p.x, p.y, 0) * segmentHeight) * parentTransform;
					
					_mesh.addVertex( rotatedPoint );
				}
			}
			
			for( int transformIndex = 0; transformIndex < _skeletonTransforms.size()-1; transformIndex++ )
			{
				for( int i = 0; i < _radiusPoints.size()-1; i++ )
				{
					int index = (transformIndex * _radiusPoints.size()) + i;
					
					int topLeft = index;
					int topRight = topLeft + 1;
					int botLeft =  topLeft + _radiusPoints.size();
					int botRight = botLeft + 1 ;
					
					_mesh.addTriangle( topLeft, topRight, botRight );
					_mesh.addTriangle( topLeft, botRight, botLeft );
				}
			}
			
			//cout << _mesh.getNumVertices() << "	" << _mesh.getIndex(_mesh.getNumIndices()-2) << endl;
			
			computeVertexNormalsFromTriangleIndices( _mesh, _faceNormalsBuffer );
		}

	
		// ------------------------------------------
		static void computeVertexNormalsFromTriangleIndices( ofMesh& _triangleMesh, vector<ofVec3f>& _faceNormals )
		{
			if( _triangleMesh.getMode() != OF_PRIMITIVE_TRIANGLES )
			{
				ofLogError() << "Lathe::computeFaceNormalsFromTriangleIndices only works on OF_PRIMITIVE_TRIANGLES meshes.";
				return;
			}
	
			_faceNormals.clear();
			
			_triangleMesh.getNormals().resize( _triangleMesh.getNumVertices() );
			for( int i = 0; i < _triangleMesh.getNormals().size(); i++ )
			{
				_triangleMesh.getNormals()[i] = ofVec3f(0);
			}
			
			
			int numTriangles = _triangleMesh.getNumIndices() / 3;
			
			for( int i = 0; i < numTriangles; i++ )
			{
				int i0 = _triangleMesh.getIndex( (i * 3) + 0 );
				int i1 = _triangleMesh.getIndex( (i * 3) + 1 );
				int i2 = _triangleMesh.getIndex( (i * 3) + 2 );
				
				ofVec3f p0 = _triangleMesh.getVertex( i0 );
				ofVec3f p1 = _triangleMesh.getVertex( i1 );
				ofVec3f p2 = _triangleMesh.getVertex( i2 );
				
				ofVec3f normal = -MathUtils::getTriangleNormal( p0, p1, p2 );
				
				_faceNormals.push_back( normal );
				
				_triangleMesh.getNormals()[i0] += normal;
				_triangleMesh.getNormals()[i1] += normal;
				_triangleMesh.getNormals()[i2] += normal;
			}
			
			for( int i = 0; i < _triangleMesh.getNormals().size(); i++ )
			{
				_triangleMesh.getNormals()[i].normalize();
			}
		}
	
		// ------------------------------------------
		static void getTransforms( vector<ofMatrix4x4>& _transforms, vector<ofVec3f>& _controlPoints, int _targetAmount, ofVec3f _firstUp = ofVec3f(0,1,0) )
		{
			_transforms.clear();
			
			vector<ofVec3f> points;
			getCardinalInterpolated( _controlPoints, _targetAmount + 1, points );
			
			ofVec3f xAxis = ofVec3f(1,0,0);
			ofVec3f yAxis = ofVec3f(0,1,0);
			ofVec3f zAxis = ofVec3f(0,0,1);
			
			for( int i = 0; i < _targetAmount; i++ )
			{
				ofVec3f p0 = points.at( i );
				ofVec3f p1 = points.at( i + 1 );
				
				if( i == 0 )
				{
					zAxis = (p1 - p0).getNormalized(); // grab our forward
					yAxis = _firstUp;
					xAxis = yAxis.getCrossed( zAxis ); // calculate side
				}
				else
				{
					zAxis = (p1 - p0).getNormalized(); // forward
					xAxis = yAxis.getCrossed( zAxis ); // guess side based on previous segment's Y
					yAxis = zAxis.getCrossed( xAxis ); // recalculate Y now that we have forward and sideways
				}
				
				xAxis = xAxis.getNormalized();
				zAxis = zAxis.getNormalized();
				
				_transforms.push_back( ofMatrix4x4::newLookAtMatrix( p0, p0 + zAxis, yAxis) );
			}
		}

		// ------------------------------------------
		static vector<ofMatrix4x4> getTransforms( vector<ofVec3f>& _controlPoints, int _targetAmount, ofVec3f _firstUp = ofVec3f(0,1,0) )
		{
			vector<ofMatrix4x4> transforms;
			getTransforms( transforms, _controlPoints, _targetAmount, _firstUp );
			return transforms;
		}
	
		// ------------------------------------------
		template< typename T>
		static T getLinearInterpolated( vector<T>& _points, float _frac )
		{
			float index = ofMap( _frac, 0, 1, 0, _points.size()-1.00001, true );
			int index0 = floor(index);
			int index1 = ceil(index);
			float tmpFrac = index - index0;
			T p0 = _points[index0];
			T p1 = _points[index1];
			return p0 + ((p1-p0) * tmpFrac);
		}
	
		// ------------------------------------------
		template< typename T>
		static T getCardinalInterpolated( vector<T>& _points, float _frac )
		{
			float index = ofMap( _frac, 0, 1, 1, _points.size()-2.00001, true );
			int index1 = floor(index);
			int index2 = ceil(index);
			
			int index0 = index1-1;
			int index3 = index2+1;
			
			float tmpFrac = index - (index1);
			T p0 = _points[index0];
			T p1 = _points[index1];
			T p2 = _points[index2];
			T p3 = _points[index3];
			
			return ofInterpolateCatmullRom( p0, p1, p2, p3, tmpFrac );
		}
	
	// ------------------------------------------
	template< typename T>
	static void getCardinalInterpolated( vector<T>& _points, int _res, vector<T>& _target )
	{
		_target.clear();
		
		for( int i = 0; i < _res; i++ )
		{
			float frac = ofMap( i, 0, _res-1, 0, 1 );
			float index = ofMap( frac, 0, 1, 1, _points.size()-2.00001, true );
			int index1 = floor(index);
			int index2 = ceil(index);
			
			int index0 = index1-1;
			int index3 = index2+1;
			
			float tmpFrac = index - (index1);
			T p0 = _points[index0];
			T p1 = _points[index1];
			T p2 = _points[index2];
			T p3 = _points[index3];
			
			_target.push_back( ofInterpolateCatmullRom( p0, p1, p2, p3, tmpFrac ) );
		}
	}
	
	// ------------------------------------------
	static vector<ofVec2f> getCirclePoints(int _res, ofVec2f _radius = ofVec2f(1.0) )
	{
		vector<ofVec2f> tmp;
		for( int i = 0; i < _res; i++ )
		{
			float ang = ofMap( i, 0, _res-1, 0, PI * 2 );
			tmp.push_back( ofVec2f( cosf(ang), sinf(ang) ) * 0.5 * _radius);
		}
		return tmp;
	}
	
	// ------------------------------------------
	static vector<ofVec2f> getCirclePointsRidged( int _res, int _numRidges, ofVec2f _radius = ofVec2f(1.0) )
	{
		vector<ofVec2f> tmp;
		for( int i = 0; i < _res; i++ )
		{
			float ridgeAng = ofWrap( ofMap( i, 0, _res-1, 0, PI * _numRidges ), 0, PI );
			float ridge = ofMap( sinf( ridgeAng ), -1, 1, 0, 1 );
			float ang = ofMap( i, 0, _res-1, 0, PI * 2 );
			tmp.push_back( ofVec2f( cosf(ang), sinf(ang) ) * ridge * 0.5 * _radius);
		}
		return tmp;
	}

	// ------------------------------------------
	static vector<ofVec2f> getSmoothStepInOutHeightPoints(int _res, float _length, float _height, float _low0 = 0.0f, float _high0 = 0.25f, float _high1 = 0.75f, float _low1 = 1.0f )
	{
		vector<ofVec2f> tmp;
		for (int i = 0; i < _res; i++)
		{
			float frac = ofMap( i, 0, _res-1, 0, 1 );
			float height = MathUtils::smoothStepInOut(_low0, _high0, _high1, _low1, frac) * _height;
			tmp.push_back( ofVec2f( _length * frac, height ) );
		}

		return tmp;
	}

	// ------------------------------------------
	static vector<ofVec2f> getCircularInOutHeightPoints(int _res, float _length, float _height, float _low0 = 0.0f, float _high0 = 0.25f, float _high1 = 0.75f, float _low1 = 1.0f )
	{
		vector<ofVec2f> tmp;
		for (int i = 0; i < _res; i++)
		{
			float frac = ofMap( i, 0, _res-1, 0, 1 );
			float height = MathUtils::circularStepInOut(_low0, _high0, _high1, _low1, frac) * _height;
			tmp.push_back( ofVec2f( _length * frac, height ) );
		}
		
		return tmp;
	}
	
	// ------------------------------------------
	template< typename T>
	static void scalePoints( vector<T>& _points, T _scale )
	{
		for( int i = 0; i < _points.size(); i++ )
		{
			_points[i] *= _scale;
		}
	}

	// ------------------------------------------
	template< typename T>
	static void scalePoints(vector<T>& _points, vector<T>& _scalePoints )
	{
		if ( _points.size() != _scalePoints.size())
		{
			return;
		}

		for (int i = 0; i < _points.size(); i++)
		{
			_points[i] *= _scalePoints[i];
		}
	}

	// ------------------------------------------
	template< typename T>
	static void offsetPoints( vector<T>& _points, T _offset )
	{
		for( int i = 0; i < _points.size(); i++ )
		{
			_points[i] += _offset;
		}
	}

	// ------------------------------------------
	template< typename T>
	static void offsetPoints(vector<T>& _points, vector<T>& _offsetPoints)
	{
		if (_points.size() != _offsetPoints.size())
		{
			return;
		}

		for (int i = 0; i < _points.size(); i++)
		{
			_points[i] += _offsetPoints[i];
		}
	}

};

// ------------------------------------------------------------------------------------------
//
class LatheMesh
{
	public:

		// ------------------------------------------
		LatheMesh()
		{
			circumferencePoints = Lathe::getCirclePoints(20);
			heightPoints = Lathe::getSmoothStepInOutHeightPoints( 200, 5, 1, 	0, 0.1, 0.9, 1.0 );
		}

		// ------------------------------------------
		void updateMesh( deque<ofVec3f>& _spineControlPoints, int _lengthRes, ofVec3f _firstUp = ofVec3f(0,1,0) )
		{
			updateMesh(heightPoints, circumferencePoints, _spineControlPoints, _lengthRes, _firstUp );
		}

		// ------------------------------------------
		void updateMesh(vector<ofVec2f>& _heightPoints, vector<ofVec2f>& _radiusPoints, deque<ofVec3f>& _spineControlPoints, int _lengthRes, ofVec3f _firstUp = ofVec3f(0,1,0) )
		{
			vector<ofVec3f> tmpSpineControlPoints;
			for (int i = 0; i < _spineControlPoints.size(); i++) { tmpSpineControlPoints.push_back(_spineControlPoints.at(i) ); }
			updateMesh(_heightPoints, _radiusPoints, tmpSpineControlPoints, _lengthRes, _firstUp );
		}

		// ------------------------------------------
		void updateMesh( vector<ofVec2f>& _heightPoints,
						 vector<ofVec2f>& _radiusPoints,
						 vector<ofVec3f>& _spineControlPoints,
						 int _lengthRes,
						 ofVec3f _firstUp = ofVec3f(0,1,0) )
		{
			Lathe::getTransforms( spineTransforms, _spineControlPoints, _lengthRes, _firstUp );
			Lathe::lathe( mesh, _heightPoints, _radiusPoints, spineTransforms, faceNormals );
		}
	
		ofMesh mesh;
	
		vector<ofVec2f> circumferencePoints;
		vector<ofVec2f> heightPoints;

		vector<ofMatrix4x4> spineTransforms;
		vector<ofVec3f> faceNormals;
};

