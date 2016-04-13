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

uniform float time;

uniform sampler2D spheresPosAndAgeTexture;
uniform sampler2D spheresVelTexture; 

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
uniform vec4  materialDiffuse   = vec4(1.0,1.0,1.0,1.0);
uniform vec4  materialEmissive  = vec4(0.0,0.0,0.0,0.0);
uniform vec4  materialAmbient   = vec4(0.0,0.0,0.0,0.0);
uniform vec4  materialSpecular  = vec4(1.0,1.0,1.0,1.0);
uniform float materialShininess = 60.0;

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
	vec4 particlePosAndAgeData = texture( spheresPosAndAgeTexture, texCoord );
	vec4 particleVelData 	   = texture( spheresVelTexture, texCoord );		

	vec3 particlePos = particlePosAndAgeData.xyz;
	float particleAge = particlePosAndAgeData.w; 

	vec3 particleVel = particleVelData.xyz; 

	vec4 vertexPos = position;
	vec3 vertexNormal = normal.xyz;

	// Pass the particle color along to the fragment shader
	v_color = materialDiffuse; 
		
	// We add the rotated model space vertex pos to the particle pos to get the final position in space
	vec3 newVertexPos = particlePos + vertexPos.xyz;
	
	gl_Position = modelViewProjectionMatrix * vec4(newVertexPos, 1.0);
	
	// Light stuff	
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