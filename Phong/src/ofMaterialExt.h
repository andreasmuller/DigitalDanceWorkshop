//
//  ofMaterialExt.h
//  GeometryShaderTubes
//
//  Created by Andreas Müller on 06/12/2014.
//
//

#pragma once 

#include "ofMaterial.h"

#define MATERIAL_EXT_SUPPORT_GUI 1

#ifdef MATERIAL_EXT_SUPPORT_GUI
	#include "ofxGui.h"
#endif

// ------------------------------------------------------------------------
//
class ofMaterialExt : public ofMaterial
{
	public:
	
		// --------------------------------------------
		ofMaterialExt()
		{
			setDiffuseColor( ofColor::white );
			setAmbientColor( ofColor::black );
			setSpecularColor( ofColor::white );
			setEmissiveColor( ofColor::black );
			setShininess( 70 );
		}

		// --------------------------------------------
		void setParams( ofShader* _shader, bool _bindShader = false )
		{
			setDiffuseColor( diffuse.get() );
			setAmbientColor( ambient.get() );
			setSpecularColor( specular.get() );
			setEmissiveColor( emissive.get() );
			setShininess( shininess.get() );

			if( _bindShader ) _shader->begin();

				_shader->setUniform4fv("materialEmissive", getEmissiveColor().v );
				_shader->setUniform4fv("materialAmbient", getAmbientColor().v );
				_shader->setUniform4fv("materialDiffuse", getDiffuseColor().v );
				_shader->setUniform4fv("materialSpecular", getSpecularColor().v );
				_shader->setUniform1f("materialShininess", getShininess() );

			if( _bindShader ) _shader->end();
		}

#ifdef MATERIAL_EXT_SUPPORT_GUI
		// --------------------------------------------
		void addToPanel( ofxPanel* _panel, string _materialName )
		{
			_panel->add( diffuse.set( _materialName   + " Diffuse",  ofColor::white, ofColor(0,0,0,0), ofColor(255)) );
			//_panel->add( ambient.set( _materialName   + " Ambient",  ofColor::black, ofColor(0,0,0,0), ofColor(255)) );
			_panel->add( specular.set( _materialName  + " Specular", ofColor::white, ofColor(0,0,0,0), ofColor(255)) );
			_panel->add( emissive.set( _materialName  + " Emissive", ofColor::black, ofColor(0,0,0,0), ofColor(255)) );
			_panel->add( shininess.set( _materialName + " Shininess", 10, 0, 127) );
		}
#endif // MATERIAL_EXT_SUPPORT_GUI

		// --------------------------------------------
		void setDiffuseColor( ofFloatColor _color )
		{
			diffuse.set( _color );
			ofMaterial::setDiffuseColor( _color );
		}

		// --------------------------------------------
		void setAmbientColor( ofFloatColor _color )
		{
			ambient.set( _color );
			ofMaterial::setAmbientColor( _color );
		}

		// --------------------------------------------
		void setSpecularColor( ofFloatColor _color )
		{
			specular.set( _color );
			ofMaterial::setSpecularColor( _color );
		}

		// --------------------------------------------
		void setEmissiveColor( ofFloatColor _color )
		{
			emissive.set( _color );
			ofMaterial::setEmissiveColor( _color );
		}

		// --------------------------------------------
		void setShininess( float _shininess )
		{
			//cout << _shininess << " " << ofGetFrameNum() << endl;
			shininess.set( _shininess );
			ofMaterial::setShininess( _shininess );
		}

		ofParameter<ofColor>	emissive;
		ofParameter<ofColor>	ambient;
		ofParameter<ofColor>	diffuse;
		ofParameter<ofColor>	specular;
		ofParameter<float>		shininess;
};