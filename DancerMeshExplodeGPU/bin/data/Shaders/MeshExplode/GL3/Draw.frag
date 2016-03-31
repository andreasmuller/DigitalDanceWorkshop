#version 330

precision highp float;

// This could be anything, no need to stick to max 8
#define MAX_LIGHTS 3

in VertexAttrib {
	vec3 normal;
	vec2 texcoord;
	vec4 color;		
	vec3 viewDir;
	vec3 lightDir[MAX_LIGHTS];
} vertex;

out vec4 fragColor;

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform mat4 normalMatrix;

uniform int	numActiveLights;

uniform vec4  lightSceneAmbient;
uniform vec4  lightDiffuse[MAX_LIGHTS];
uniform vec4  lightSpecular[MAX_LIGHTS];
uniform float lightRadius[MAX_LIGHTS];
uniform vec3  lightPositionWorld[MAX_LIGHTS];
uniform vec3  lightPositionCamera[MAX_LIGHTS];

//uniform vec4  materialDiffuse; // We'll be using vertex.color
//uniform vec4  materialAmbient;
uniform vec4  materialEmissive;
uniform vec4  materialSpecular;
uniform float materialShininess;

uniform float frontFraceNormalSign = -1.0;

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
	vec3 finalColor = lightSceneAmbient.xyz + materialEmissive.xyz;// + materialAmbient.xyz;
	
	vec3 n = normalize(vertex.normal);
	vec3 v = normalize(vertex.viewDir);
	
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
		vec3 l = vertex.lightDir[i];
		
		float atten = max(0.0, 1.0 - dot(l, l));
		l = normalize(l);
		
		vec3 diffuse  = diffuseLighting(  n, l, vertex.color.xyz, lightDiffuse[i].xyz );
		vec3 specular = specularLighting( n, l, v, materialShininess, materialSpecular.xyz, lightSpecular[i].xyz );
		
		diffuse *= atten;
		specular *= atten;
		
		finalColor += (diffuse + specular);
	}
	

	if( !gl_FrontFacing ) 
	{
		finalColor.xyz = mix( finalColor.xyz, vec3(1.0, 0.0, 0.0), 0.5 );
	}

	return vec4(finalColor.xyz, vertex.color.a);
	
}

//-------------------------------------------------------------------------------------------------------------------------------------
//
void main (void)
{
	vec4 materialAndLight = computeLighting();
	
	fragColor = materialAndLight;
	//fragColor = vertex.color;
	//fragColor = vec4(1.0,0.0,1.0,1.0);
	//fragColor = vec4( (vertex.normal.xyz + vec3(1,1,1)) * 0.5, 1 );
}