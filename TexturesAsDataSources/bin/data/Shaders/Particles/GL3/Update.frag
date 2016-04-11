#version 330

#ifdef GL_ES
	// define default precision for float, vec, mat.
	precision highp float;
#endif

#pragma include "../../Common/ShaderHelpers.glslinc"
#pragma include "../../Common/SimplexNoiseDerivatives4D.glslinc"
//#pragma include "../../Common/Noise4D.glslinc"

uniform sampler2D particlePosAndAgeTexture;
uniform sampler2D particleOldPosAndAgeTexture;
uniform sampler2D particleVelTexture;
uniform sampler2D spawnPositionTexture;
uniform sampler2D spawnVelocityTexture;

uniform float time;
uniform float timeStep;

uniform float particleMaxAge;

uniform vec3 wind = vec3( 0.0 );

in vec4 colorVarying;
in vec2 texCoordVarying;

layout(location = 0) out vec4 fragOutPosAndAge;
layout(location = 1) out vec4 fragOutVel;

// -----------------------------------------------------------
void main (void)
{
	vec2 texCoord = texCoordVarying;
	
	vec4 posAndAge = texture( particlePosAndAgeTexture, texCoord );
	vec3 vel = texture( particleVelTexture, texCoord ).xyz;	

	vec4 spawnPosition = texture( spawnPositionTexture, texCoord );	
	vec4 spawnVelocity = texture( spawnVelocityTexture, texCoord );		

	vec3 pos = posAndAge.xyz;
	float age = posAndAge.w;

	vec3 newVel = vel;

	age += timeStep;
	
	if( age > particleMaxAge )
	{
		age -= particleMaxAge;
		pos = spawnPosition.xyz; 
		newVel = spawnVelocity.xyz * 0.3;
	}

	vec3 totalVelocity = wind;

	newVel += totalVelocity;
	vec3 newPos = pos + newVel;

	if( newPos.y < 0.0 )
	{
		newPos.y = 0.0;
		newVel.y *= -1.0;
		newVel *= 0.99;
	}

	fragOutPosAndAge = vec4( newPos, age );
	fragOutVel = vec4( newVel, 1.0 );
}