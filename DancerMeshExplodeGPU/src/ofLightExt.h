//
//  ofLightExt.h
//  GeometryShaderTubes
//
//  Created by Andreas Müller on 06/12/2014.
//
//


#pragma once

#include "ofLight.h"

#define LIGHT_EXT_SUPPORT_GUI 1

#ifdef LIGHT_EXT_SUPPORT_GUI
	#include "ofxGui.h"
#endif

// ------------------------------------------------------------------------
//
class ofLightExt : public ofLight
{
public:

	// --------------------------------------------
	ofLightExt()
	{
		radius = 10.0f;
	}

#ifdef LIGHT_EXT_SUPPORT_GUI
	// --------------------------------------------
	void addToPanel( ofxPanel* _panel, string _lightName, float _maxRadius = 20, bool _addPosition = true, ofVec3f _posMin = ofVec3f(-30), ofVec3f _posMax = ofVec3f(30) )
	{
		lightParamGroup.setName( _lightName );
		
		if( _addPosition ) lightParamGroup.add( position.set( "Position", ofVec3f(0.001,10,0.001), _posMin, _posMax ) ); // If we look straight down, we just get black, check up on this
		if( _addPosition ) lightParamGroup.add( lookAtPos.set( "Look At", ofVec3f(0,0,0), _posMin, _posMax ) );
		
		lightParamGroup.add( diffuse.set( "Diffuse",  ofColor::white, ofColor(0,0,0,0), ofColor(255)) );
		lightParamGroup.add( specular.set( "Specular", ofColor::white, ofColor(0,0,0,0), ofColor(255)) );
		lightParamGroup.add( radius.set( "Radius", _maxRadius * 0.5, 0, _maxRadius) );
		
		_panel->add( lightParamGroup );
		_panel->minimizeAll();
		
		enable(); // If we added it to a UI the least we can do is enable it by default
	}
#endif //LIGHT_EXT_SUPPORT_GUI
	
	// --------------------------------------------
	static void setParams( ofShader* _shader, vector<ofLightExt>& _lights, ofMatrix4x4 _modelView, bool _bindShader = false )
	{
		setLightPropertiesFromParams( _lights ); // overwrite setDiffuse, etc to update ofParameters instead?
		
		vector<ofVec3f> lightPositionWorld;
		vector<ofVec3f> lightPositionCamera;
		vector<ofFloatColor> lightDiffuse;
		vector<ofFloatColor> lightSpecular;
		vector<float> lightRadius;
		
		for( unsigned int i = 0; i < _lights.size(); i++ )
		{
			ofLightExt* light = &_lights.at(i);
			if( light->getIsEnabled() && light->getRadius() > 0 )
			{
				lightPositionWorld.push_back( light->getGlobalPosition() );
				lightPositionCamera.push_back( light->getGlobalPosition() * _modelView );
				
				lightDiffuse.push_back( light->getDiffuseColor() );
				lightSpecular.push_back( light->getSpecularColor() );
				lightRadius.push_back( light->getRadius() );
				
				light->enable(); // call this again to set latest parameters, for GL2
			}
		}

		setLightParams( _shader,
						 ofGetGlobalAmbientColor(),
						lightPositionWorld, 
						lightPositionCamera, 
						lightDiffuse, 
						lightSpecular, 
						lightRadius,
					    _bindShader);
	}

	// --------------------------------------------
	static void setParams( ofShader* _shader, vector<ofLightExt*>& _lights, ofMatrix4x4 _modelView, bool _bindShader = false )
	{
		setLightPropertiesFromParams( _lights ); // overwrite setDiffuse, etc to update ofParameters instead?

		vector<ofVec3f> lightPositionWorld;
		vector<ofVec3f> lightPositionCamera;
		vector<ofFloatColor> lightDiffuse;
		vector<ofFloatColor> lightSpecular;
		vector<float> lightRadius;
		
		for( unsigned int i = 0; i < _lights.size(); i++ )
		{
			ofLightExt* light = _lights.at(i);
			if( light->getIsEnabled() && light->getRadius() > 0  )
			{
				lightPositionWorld.push_back( light->getGlobalPosition() );
				lightPositionCamera.push_back( light->getGlobalPosition() * _modelView );
				
				lightDiffuse.push_back( light->getDiffuseColor() );
				lightSpecular.push_back( light->getSpecularColor() );
				lightRadius.push_back( light->getRadius() );
				
				light->enable(); // call this again to set latest parameters, for GL2
			}
		}

		setLightParams( _shader,
						 ofGetGlobalAmbientColor(),
						lightPositionWorld, 
						lightPositionCamera, 
						lightDiffuse, 
						lightSpecular, 
						lightRadius,
					    _bindShader );
	}

	// --------------------------------------------
	static void setLightParams(	ofShader* _shader,
							ofFloatColor _lightSceneAmbient,
							vector<ofVec3f>& _lightPositionWorld, 
							vector<ofVec3f>& _lightPositionCamera, 
							vector<ofFloatColor>& _lightDiffuse, 
							vector<ofFloatColor>& _lightSpecular, 
							vector<float>& _lightRadius,
							bool _bindShader)
	{
		int numActiveLights = _lightPositionWorld.size();
		
		if( _bindShader ) _shader->begin();

			_shader->setUniform1i("numActiveLights", numActiveLights );
			_shader->setUniform4fv("lightSceneAmbient", (float*)&_lightSceneAmbient.v );
		
			if( numActiveLights > 0 )
			{
				_shader->setUniform3fv("lightPositionWorld", 	_lightPositionWorld.at(0).getPtr(), 	numActiveLights );
				_shader->setUniform3fv("lightPositionCamera", 	_lightPositionCamera.at(0).getPtr(), 	numActiveLights );
				_shader->setUniform4fv("lightDiffuse", 			(float*)&_lightDiffuse.at(0).v, 		numActiveLights );
				_shader->setUniform4fv("lightSpecular", 		(float*)&_lightSpecular.at(0).v, 		numActiveLights );
				_shader->setUniform1fv("lightRadius", 			(float*)&_lightRadius.at(0), 			numActiveLights );
			}
		
		if( _bindShader ) _shader->end();
	}

	// --------------------------------------------
	static void setLightPropertiesFromParams( vector<ofLightExt*>& _lights )
	{
		for (unsigned int i = 0; i < _lights.size(); i++)
		{ 
			ofLightExt* light = _lights.at(i);
			if( light->getIsEnabled() )
			{
				light->setGlobalPosition( light->position );
				light->lookAt( light->lookAtPos.get() );
				light->setDiffuseColor(   light->diffuse.get() );
				light->setSpecularColor(  light->specular.get() );
			}
		}
	}

	// --------------------------------------------
	static void setLightPropertiesFromParams( vector<ofLightExt>& _lights )
	{
		for (unsigned int i = 0; i < _lights.size(); i++)
		{
			ofLightExt* light = &_lights.at(i);
			if(light->getIsEnabled() )
			{
				light->setGlobalPosition( light->position );
				light->lookAt( light->lookAtPos.get() );
				light->setDiffuseColor(   light->diffuse.get() );
				light->setSpecularColor(  light->specular.get() );
			}
		}
	}
	
	// --------------------------------------------
	void draw( float _size = 0.2 )
	{
		ofSetColor( getDiffuseColor() );
		ofDrawSphere( getGlobalPosition(), _size );
	}

	// --------------------------------------------
	void setPosition(const ofVec3f& p)
	{
		position = p;
		ofLight::setPosition(p);
	}

	// --------------------------------------------
	void setRadius( float _radius )
	{
		radius.set( _radius );
	}

	// --------------------------------------------
	float getRadius()
	{
		return radius.get();
	}

	//----------------------------------------
	void setDiffuseColor(const ofFloatColor& c) 
	{
		ofLight::setDiffuseColor( c );
		diffuse.set( c );
	}

	//----------------------------------------
	void setSpecularColor(const ofFloatColor& c) 
	{
		ofLight::setSpecularColor( c );
		specular.set( c );
	}

	//----------------------------------------
	ofFloatColor getDiffuseColor() const 
	{
		return diffuse.get();
	}

	//----------------------------------------
	ofFloatColor getSpecularColor() const 
	{
		return specular.get();
	}

	ofParameter<ofColor>	diffuse;
	ofParameter<ofColor>	specular;

	ofParameter<ofVec3f>	position;
	ofParameter<ofVec3f>	lookAtPos;

	ofParameter<float>		radius;
	
	ofParameterGroup		lightParamGroup;
};