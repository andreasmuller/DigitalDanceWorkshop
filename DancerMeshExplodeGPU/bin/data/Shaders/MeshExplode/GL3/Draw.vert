#version 330

precision highp float;

// This could be anything, no need to stick to max 8
#define MAX_LIGHTS 3

layout(location = 0) in vec4  position;
layout(location = 1) in vec4  color;
layout(location = 2) in vec3  normal;
layout(location = 3) in vec2  texcoord;

out VertexAttrib {
	vec3 normal;
	vec2 texcoord;
} vertex;

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

uniform vec3  lightPositionWorldDebug;
uniform vec3  lightPositionCameraDebug;

void main()
{
	vertex.normal = (transpose(inverse(modelViewMatrix)) * vec4(normal,0.0)).xyz; // has to be done like this for now as the normalMatrix is not updated when we transform an object to draw from OF
	//vertex.normal = (normalMatrix * vec4(normal,0.0)).xyz;
	
	vertex.texcoord = texcoord;

	gl_Position = position;	
}