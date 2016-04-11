#version 330

#ifdef GL_ES
	// define default precision for float, vec, mat.
	precision highp float;
#endif

#pragma include "../../Common/ShaderHelpers.glslinc"

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform mat4 normalMatrix;

uniform vec2 resolution;

uniform float time = 0.0;
uniform float timeStep = 0.01666666666667;
uniform float maxAge = 1.0;

uniform sampler2D posAndAgeTexture;
uniform sampler2D oldPosAndAgeTexture;

uniform sampler2D spawnPosTexture;

// Lights
#define MAX_LIGHTS 3

uniform highp int numActiveLights; // highp int is range -65535, +65535

uniform vec4  lightSceneAmbient;
uniform vec4  lightDiffuse[MAX_LIGHTS];
uniform vec4  lightSpecular[MAX_LIGHTS];
uniform float lightRadius[MAX_LIGHTS];
uniform vec3  lightPositionWorld[MAX_LIGHTS];
uniform vec3  lightPositionCamera[MAX_LIGHTS];

// Material
uniform vec4  materialDiffuse;
uniform vec4  materialEmissive;
uniform vec4  materialAmbient;
uniform vec4  materialSpecular;
uniform float materialShininess;

in vec4 position;
in vec4 color;
in vec4 normal;
in vec2 texcoord;

// Varying outs
out vec3 v_normal;
out vec4 v_color;	
out vec2 v_texcoord;
out vec3 v_viewDir;	
out vec4 v_vertEyeSpace;	
out vec3 v_lightDir[MAX_LIGHTS];

// ----------------------------
void main ()
{
	// Figure out the texture coordinate our data is on from the instance ID
	float instanceIDf = float(gl_InstanceID);
	vec2 texCoord;
	texCoord.x = mod(instanceIDf, resolution.x) / resolution.x;
	texCoord.y = floor(instanceIDf / resolution.x) / resolution.y;
	
	// Grab our data
	vec4 posAndAgeData 		= texture( posAndAgeTexture,    texCoord );
	vec4 oldPosAndAgeData 	= texture( oldPosAndAgeTexture, texCoord );

	vec3 pos = posAndAgeData.xyz;
	float age = posAndAgeData.w; 
	vec3 oldPos = oldPosAndAgeData.xyz;	

//pos = texture( spawnPosTexture, texCoord ).xyz;

	vec3 vel = pos - oldPos;

	float ageFrac = age / maxAge;
	vec4 vertexPos = position;
	vec3 vertexNormal = normal.xyz;
	float scale = 1.0;

	vertexPos *= scale;

	// Pass the particle color along to the fragment shader
	v_color = vec4(1.0,1.0,1.0,1.0); //color;

//	float fadeTimeFrames = timeStep * 2.0;
//	vertexPos.xyz *= smoothStepInOut( timeStep, fadeTimeFrames, maxAge-fadeTimeFrames, maxAge, age );	

	// Rotate the vertex of our mesh (in model space) to face the velocity direction
	mat4 lookAt = makeLookAt( vec3(0,0,0), vel, vec3(0,1,0) );
	vertexPos = lookAt * vertexPos;
	
	// We add the rotated model space vertex pos to the particle pos to get the final position in space
	vec3 newVertexPos = pos + vertexPos.xyz;
	
	gl_Position = modelViewProjectionMatrix * vec4(newVertexPos, 1.0);
	
	// Light stuff	
	// Rotate the normal just as we did the vertex, then apply the camera transform
	vertexNormal = (lookAt * vec4(vertexNormal, 0)).xyz;
	v_normal = normalize(normalMatrix * vec4(vertexNormal,1.0)).xyz;
	
	// We do lighting clculations in view (camera) space
	vec4 viewSpaceVertex = modelViewMatrix * vec4(newVertexPos, 1.0);
	v_viewDir = -viewSpaceVertex.xyz;

	// TODO: flatten this? Might make it faster.
	for ( int i = 0; i < numActiveLights; ++i )
	{
		v_lightDir[i] = vec3(lightPositionCamera[i].xyz - viewSpaceVertex.xyz) / lightRadius[i];
	}	
}