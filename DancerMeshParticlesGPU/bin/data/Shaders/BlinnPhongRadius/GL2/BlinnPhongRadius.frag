#version 120

#define MAX_LIGHTS 4

uniform int	numActiveLights;

uniform vec4  lightSceneAmbient;
uniform vec4  lightDiffuse[MAX_LIGHTS];
uniform vec4  lightSpecular[MAX_LIGHTS];
uniform float lightRadius[MAX_LIGHTS];
uniform vec3  lightPositionWorld[MAX_LIGHTS];
uniform vec3  lightPositionCamera[MAX_LIGHTS];

uniform vec4  materialDiffuse;
uniform vec4  materialSpecular;
uniform float materialShininess;

varying vec3 normal;
varying vec3 viewDir;

varying vec3 lightDir[MAX_LIGHTS];

varying vec3 debugVec3;


//-------------------------------------------------------------------------------------------------------------------------------------
// returns intensity of diffuse reflection
vec3 diffuseLighting(in vec3 _N, in vec3 _L, in vec3 _materialDiffuse, in vec3 _lightDiffuse )
{
	// calculation as for Lambertian reflection
	float diffuseTerm = clamp(dot(_N, _L), 0, 1) ;
	return _materialDiffuse * _lightDiffuse * diffuseTerm;
}

//-------------------------------------------------------------------------------------------------------------------------------------
// returns intensity of specular reflection
vec3 specularLighting(in vec3 _N, in vec3 _L, in vec3 _V, in float _materialShininess, in vec3 _materialSpecular, in vec3 _lightSpecular )
{
	float specularTerm = 0;
	
	// calculate specular reflection only if
	// the surface is oriented to the light source
	if(dot(_N, _L) > 0)
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
	vec3 finalColor = lightSceneAmbient.xyz;
	
	vec3 n = normalize(normal);
	vec3 v = normalize(viewDir);
	
	for ( int i = 0; i < numActiveLights; i++ )
	{
		vec3 l = lightDir[i];
		
		float atten = max(0.0, 1.0 - dot(l, l));
		l = normalize(l);
		
		vec3 diffuse  = diffuseLighting(  n, l, materialDiffuse.xyz, lightDiffuse[i].xyz );
		vec3 specular = specularLighting( n, l, v, materialShininess, materialSpecular.xyz, lightSpecular[i].xyz );
		
		diffuse *= atten;
		specular *= atten;
		
		finalColor += (diffuse + specular);
	}
	
	return vec4(finalColor.xyz, materialDiffuse.a);
	
}

//-------------------------------------------------------------------------------------------------------------------------------------
//
void main (void)
{
	vec4 materialAndLight = computeLighting();
	gl_FragColor = materialAndLight;
}




