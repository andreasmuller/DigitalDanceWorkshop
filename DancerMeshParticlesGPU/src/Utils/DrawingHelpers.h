//
//  DrawingHelpers.h
//  FirstPersonCamera
//
//  Created by Andreas Müller on 22/09/2014.
//
//

#pragma once

#include "ofMain.h"

class DrawingHelpers
{
	public:
		
		//-----------------------------------------------------------------------------------------
		//
		static void drawFloorGrid( ofTrueTypeFont* _font )
		{
			ofSetColor( ofColor(50) );
			
			ofPushMatrix();
			ofRotate(90, 0, 0, -1);
			ofDrawGridPlane( 10, 1, false );
			ofPopMatrix();
			
			ofDisableDepthTest();
			
			ofSetColor( ofColor(100) );
			
			float tickDistMm = 1000;
			int numTicks = 20;
			ofVec2f start( ((numTicks * -0.5) * tickDistMm), ((numTicks * -0.5) * tickDistMm) );
			ofVec2f end(   ((numTicks *  0.5) * tickDistMm), ((numTicks *  0.5) * tickDistMm) );
			for( int y = 0; y <= numTicks; y++ )
			{
				float tmpY = start.y + (y * tickDistMm);
				
				ofLine( ofVec3f( start.x, 0, tmpY), ofVec3f( end.x, 0, tmpY) );
				
				for( int x = 0; x <= numTicks; x++ )
				{
					float tmpX = start.x + (x * tickDistMm);
					ofLine( ofVec3f( tmpX, 0, start.y), ofVec3f( tmpX, 0, end.y) );
				}
			}
			
			ofSetColor( ofColor(150) );
			
			for( int y = 0; y <= numTicks; y++ )
			{
				float tmpY = start.y + (y * tickDistMm);
				
				ofPushMatrix();
					ofTranslate( ofVec3f(-10,0,tmpY + 10) );
					ofRotateX(-90);
					ofScale(2,2,2);
					//fontLarge.drawString( ofToString(tmpY), 0, 0 );
					if( _font != NULL ) _font->drawString( ofToString(tmpY), 0, 0 );
				ofPopMatrix();
				
				ofPushMatrix();
					ofTranslate( ofVec3f(tmpY + 10,0, -10) );
					ofRotateX(-90);
					ofScale(2,2,2);
					//fontLarge.drawString( ofToString(tmpY), 0, 0 );
					if( _font != NULL ) _font->drawString( ofToString(tmpY), 0, 0 );
				ofPopMatrix();
			}
			
			ofEnableDepthTest();
		}
	
		//-----------------------------------------------------------------------------------------
		//
		static void drawTransform( ofMatrix4x4 _transform, float _size = 1.0f )
		{
			ofPushMatrix();
			
				ofMultMatrix( _transform );
				ofDrawAxis( _size );
			
			ofPopMatrix();
		}
	
		//-----------------------------------------------------------------------------------------
		//
		static void makeCylinderLineMesh( ofMesh& _targetMesh,
										  vector<ofVec3f>& _points,
										  vector<ofVec3f>& _velocities,
										  float _radius,
										  float _velocityScale = 1.0f,
										  bool _colorByDirection = false )
		{
			if( _points.size() != _velocities.size() )
			{
				return;
			}
			
			ofCylinderPrimitive cylinder = ofCylinderPrimitive( 0.5, 1.0f, 8, 2, 1, true, OF_PRIMITIVE_TRIANGLES ); // Make sure it's OF_PRIMITIVE_TRIANGLES
			
			ofMesh tmpMesh;
			
			_targetMesh.clear();
			_targetMesh.setMode( OF_PRIMITIVE_TRIANGLES );
			//_targetMesh.setMode( OF_PRIMITIVE_LINES );
			
			for( int i = 0; i < _points.size(); i++ )
			{
				ofVec3f startPos = _points.at(i);
				ofVec3f endPos   = _points.at(i) + (_velocities.at(i) * _velocityScale);
				float length = (endPos-startPos).length();
				
				ofMatrix4x4 transform = ofMatrix4x4::newLookAtMatrix( startPos, endPos, ofVec3f(0,1,0) );
				transform.glRotate(90, 1, 0, 0);
				transform.glTranslate( 0,length * 0.5,0 );
				transform.glScale(_radius,length,_radius);
				
//				_targetMesh.addVertex( ofVec3f(0,-0.5,0) * transform);
//				_targetMesh.addVertex( ofVec3f(0, 0.5,0) * transform);

//				_targetMesh.addVertex( startPos );
//				_targetMesh.addVertex( endPos );
				
				tmpMesh = cylinder.getMesh();
				for( int i = 0; i < tmpMesh.getNumVertices(); i++ )
				{
					tmpMesh.getVertices()[i] = tmpMesh.getVertices()[i] * transform;
					tmpMesh.getNormals()[i] = ofMatrix4x4::transform3x3( tmpMesh.getNormals()[i], transform );
				}
				
				if( _colorByDirection )
				{
					tmpMesh.clearColors();
					
					ofVec3f tmpDirection = _velocities.at(i).getNormalized();
					tmpDirection = (tmpDirection + ofVec3f(1)) * 0.5;
					ofFloatColor tmpCol( tmpDirection.x, tmpDirection.y, tmpDirection.z );
					for( int i = 0; i < tmpMesh.getNumVertices(); i++ )
					{
						tmpMesh.addColor(tmpCol);
					}
				}
				
				_targetMesh.append( tmpMesh );
			}
		}
};
