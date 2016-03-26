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
uniform float particleMaxVel;

uniform float oldVelToUse;

uniform float noisePositionScale = 0.5; // some start values in case we don't set any
uniform float noiseMagnitude = 0.075;
uniform float noiseTimeScale = 0.4;
uniform float noisePersistence = 0.33;
uniform vec3 wind = vec3( 0.0 );

#define OCTAVES 3

in vec4 colorVarying;
in vec2 texCoordVarying;

layout(location = 0) out vec4 fragOutPosAndAge;
layout(location = 1) out vec4 fragOutVel;

// -----------------------------------------------------------
void main (void)
{
	vec2 texCoord = texCoordVarying;
	
	vec4 posAndAge = texture( particlePosAndAgeTexture, texCoord );
	//vec4 posAndAgeOld = texture( particleOldPosAndAgeTexture, texCoord );	
	vec3 oldVel = texture( particleVelTexture, texCoord ).xyz;
	vec4 spawnPosition = texture( spawnPositionTexture, texCoord );	
	vec4 spawnVelocity = texture( spawnVelocityTexture, texCoord );	

	vec3 pos = posAndAge.xyz;
	float age = posAndAge.w;

	//vec3 oldPos = posAndAgeOld.xyz;

	vec3 newVel = vec3(0,0,0);
	//vec3 oldVel = pos - oldPos;	

	age += timeStep;
	
	if( age > particleMaxAge )
	{
		age -= particleMaxAge;
		
		pos = spawnPosition.xyz; 
		//oldPos = pos - spawnVelocity.xyz;

		vec3 limitedSpawnVel = spawnVelocity.xyz;
		float tmpSpawnVelLength = length( limitedSpawnVel );
		if( tmpSpawnVelLength > particleMaxVel )
		{
			limitedSpawnVel = (limitedSpawnVel / tmpSpawnVelLength) * particleMaxVel;
		}

		//oldVel = spawnVelocity.xyz;
		oldVel = limitedSpawnVel;
		newVel = limitedSpawnVel;
	}

//pos = spawnPosition.xyz;
//oldVel = vec3( 0.0, 1.0, 0.0 );
	
	vec3 noisePosition = pos  * noisePositionScale;
	float noiseTime    = time * noiseTimeScale;
	
	vec3 noiseVelocity = curlNoise( noisePosition, noiseTime, OCTAVES, noisePersistence ) * noiseMagnitude;
	//vec3 noiseVelocity = fbmvec3( vec4(noisePosition, noiseTime), OCTAVES, 2.023, noisePersistence ) * noiseMagnitude;	
	vec3 totalVelocity = wind + noiseVelocity;// + (oldVel * oldVelToUse);

	newVel += totalVelocity;
	newVel = mix( newVel, oldVel, oldVelToUse );

	// Limit vel
	float tmpVelLength = length( newVel );
	if( tmpVelLength > particleMaxVel )
	{
		newVel = (newVel / tmpVelLength) * particleMaxVel;
	}

	vec3 newPos = pos + newVel;
		
	//newPos = spawnPosition.xyz;

	fragOutPosAndAge = vec4( newPos, age );
	fragOutVel = vec4( newVel, 1.0 );
	//fragOutVel = vec4( 1.0, 1.0, 1.0, 1.0 );	
}