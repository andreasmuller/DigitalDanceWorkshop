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
uniform float timeStep;

uniform sampler2D particlePosAndAgeTexture;
uniform sampler2D particleVelTexture; 
uniform sampler2D ageColorPalette;
uniform sampler2D spawnPositionTexture;
uniform sampler2D spawnVelocityTexture;
uniform sampler2D randomTexture;

uniform float particleMaxAge;

uniform vec4 particleStartColor;
uniform vec4 particleEndColor;

uniform float particleStartScale;
uniform float particleEndScale;

uniform float particleSizeRandomness;

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
	vec4 particlePosAndAgeData = texture( particlePosAndAgeTexture, texCoord );
	vec4 particleVelData = texture( particleVelTexture, texCoord );		
	vec4 spawnPosition = texture( spawnPositionTexture, texCoord );	
	vec4 spawnVelocity = texture( spawnVelocityTexture, texCoord );	
	vec4 randomVals    = texture( randomTexture, texCoord );

	vec3 particlePos = particlePosAndAgeData.xyz;
	float particleAge = particlePosAndAgeData.w; 

	//vec3 particlePosOld = particleDataOld.xyz;	

	// If we are on the first frame, copy over the current pos into the old pos so we don't add garbage velocities
	//particlePosOld = mix( particlePos - spawnVel, particlePosOld, step( timeStep * 1.5, particleAge ) );
	// if we are in the first frame add the negative normal to the pos for the oldPos

	vec3 particleVel = particleVelData.xyz; //particlePos - particlePosOld;

	float ageFrac = particleAge / particleMaxAge;
	vec4 vertexPos = position;
	vec3 vertexNormal = normal.xyz;

	// Pass the particle color along to the fragment shader
	v_color = mix(particleStartColor, particleEndColor, ageFrac );
	v_color *= texture( ageColorPalette, vec2(ageFrac, 0));

	// we'll be facing a strange direction in the first frame as we can't update the oldPos or carry a valid vel, so hide it for now
	//vertexPos.xyz *= smoothStepInOut( 0.0, 0.05, 0.95, 1.0, ageFrac );
	float fadeTimeFrames = timeStep * 2.0;
	vertexPos.xyz *= smoothStepInOut( timeStep, fadeTimeFrames, particleMaxAge-fadeTimeFrames, particleMaxAge, particleAge );	
	//vertexPos.xyz *= smoothStepInOut( fadeTimeFrames * 0.01, fadeTimeFrames, particleMaxAge-fadeTimeFrames, particleMaxAge, particleAge );
	vertexPos.xyz *= mix( particleStartScale, particleEndScale, ageFrac ); 

	// Scale a bit here, based on our randomTexture, to make feathers less uniform
	float uniformScale = map( randomVals.x, 0.0, 1.0, particleSizeRandomness, 1.0 );
	vec3 randomScale = vec3( uniformScale, uniformScale, uniformScale );

	vertexPos.xyz *= randomScale; // Add a little bit of randomness to the width?
	vertexNormal.xyz *= randomScale; // do this?

	// Rotate the vertex of our mesh (in model space) to face the velocity direction
	mat4 lookAt = makeLookAt( vec3(0,0,0), particleVel, vec3(0,1,0) );
	vertexPos = lookAt * vertexPos;
	
	// We add the rotated model space vertex pos to the particle pos to get the final position in space
	vec3 newVertexPos = particlePos + vertexPos.xyz;
	
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