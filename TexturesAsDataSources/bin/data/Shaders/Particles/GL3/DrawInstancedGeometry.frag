#version 330

#ifdef GL_ES
	// define default precision for float, vec, mat.
	precision highp float;
#endif

// Lights
#define MAX_LIGHTS 3

uniform highp int	numActiveLights; // highp int is range -65535, +65535

uniform vec4  lightSceneAmbient;
uniform vec4  lightDiffuse[MAX_LIGHTS];
uniform vec4  lightSpecular[MAX_LIGHTS];
uniform float lightRadius[MAX_LIGHTS];
uniform vec3  lightPositionWorld[MAX_LIGHTS];
uniform vec3  lightPositionCamera[MAX_LIGHTS];

// Material
uniform vec4  materialEmissive;
uniform vec4  materialAmbient;
uniform vec4  materialSpecular;
uniform float materialShininess;

uniform float frontFraceNormalSign;

// Varying ins
in vec3 v_normal;
in vec4 v_color;	
in vec2 v_texcoord;
in vec3 v_viewDir;	
in vec4 v_vertEyeSpace;	
in vec3 v_lightDir[MAX_LIGHTS];

out vec4 fragOut;

//-------------------------------------------------------------------------------------------------------------------------------------
// returns intensity of diffuse reflection
vec3 diffuseLighting(in vec3 _N, in vec3 _L, in vec3 _materialDiffuse, in vec3 _lightDiffuse )
{
	// calculation as for Lambertian reflection
	float diffuseTerm = clamp(dot(_N, _L), 0.0, 1.0) ;
	return _materialDiffuse * _lightDiffuse * diffuseTerm;
}

//-------------------------------------------------------------------------------------------------------------------------------------
// returns intensity of specular reflection
vec3 specularLighting(in vec3 _N, in vec3 _L, in vec3 _V, in float _materialShininess, in vec3 _materialSpecular, in vec3 _lightSpecular )
{
	float specularTerm = 0.0;
	
	// calculate specular reflection only if
	// the surface is oriented to the light source
	if(dot(_N, _L) > 0.0)
	{
		// half vector
		vec3 H = normalize(_L + _V);
		specularTerm = pow(dot(_N, H), _materialShininess);
	}
	return _materialSpecular * _lightSpecular * specularTerm;
}

//-------------------------------------------------------------------------------------------------------------------------------------
//
vec4 computeLighting()
{
	vec3 finalColor = lightSceneAmbient.xyz + materialEmissive.xyz;
	
	vec4 vertexDiffuse = v_color;

	vec3 n = normalize(v_normal);
	vec3 v = normalize(v_viewDir);
	
	if( gl_FrontFacing ) 
	{
		n = n *  frontFraceNormalSign;
	}
	else
	{
		n = n * -frontFraceNormalSign;
	}

	for ( int i = 0; i < numActiveLights; i++ )
	{
		vec3 l = v_lightDir[i];
		
		float atten = max(0.0, 1.0 - dot(l, l));
		atten = smoothstep( 0.0, 1.0, atten );

		l = normalize(l);
		
		vec3 diffuse  = diffuseLighting(  n, l, vertexDiffuse.xyz, lightDiffuse[i].xyz );
		vec3 specular = specularLighting( n, l, v, materialShininess, materialSpecular.xyz, lightSpecular[i].xyz );
		
		diffuse *= atten;
		specular *= atten;
		
		finalColor += (diffuse + specular);
	}
	
	return vec4(finalColor.xyz, vertexDiffuse.a);
}

// --------------------------------------------
void main ()
{
	vec4 color = computeLighting();
	fragOut = color;
	
	//fragOut = v_color;	
	//fragOut = vec4( (v_normal.xyz + vec3(1.0)) * 0.5, 1.0 );

	//fragOut = vec4( vec3(lightRadius[0]), 1.0 );
	
	//vec4 tmpCol = lightDiffuse[0];
	//tmpCol.w = 1.0;
	//fragOut = tmpCol;	
}