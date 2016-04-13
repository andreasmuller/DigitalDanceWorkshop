#version 120

// This could be anything, no need to stick to max 8
#define MAX_LIGHTS 4

uniform int	numActiveLights;

uniform vec4  lightSceneAmbient;
uniform vec4  lightDiffuse[MAX_LIGHTS];
uniform vec4  lightSpecular[MAX_LIGHTS];
uniform float lightRadius[MAX_LIGHTS];
uniform vec3  lightPositionWorld[MAX_LIGHTS];
uniform vec3  lightPositionCamera[MAX_LIGHTS];

uniform vec3  lightPositionWorldDebug;
uniform vec3  lightPositionCameraDebug;

varying vec3 normal;
varying vec3 viewDir;

varying vec3 lightDir[MAX_LIGHTS];

varying vec3 debugVec3;

void main()
{
	normal = gl_NormalMatrix * gl_Normal;
	vec4 vertexCameraPos = gl_ModelViewMatrix * gl_Vertex;
	viewDir = -vertexCameraPos.xyz;
	
	for ( int i = 0; i < numActiveLights; i++ )
	{
		lightDir[i] = vec3(lightPositionCamera[i] - vertexCameraPos.xyz) / lightRadius[i];
	}
	
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0] = gl_MultiTexCoord0;
}

