#version 330

precision highp float;

#pragma include "../../Common/ShaderHelpers.glslinc"
#pragma include "../../Common/SimplexNoiseDerivatives4D.glslinc"
//#pragma include "../../Common/Noise4D.glslinc"

/*
uniform float time;
uniform float timeStep;

uniform float particleMaxAge;
uniform float particleMaxVel;

uniform float oldVelToUse;

uniform float noisePositionScale;// = 1.5; // some start values in case we don't set any
uniform float noiseMagnitude;// = 0.075;
uniform float noiseTimeScale;// = 1.0 / 4000.0;
uniform float noisePersistence;// = 0.2;
uniform vec3 wind;// = vec3( 0.5, 0.0, 0.0 );
*/
#define OCTAVES 3

in vec4 colorVarying;
in vec2 texCoordVarying;

layout(location = 0) out vec4 fragOutPos;
layout(location = 1) out vec4 fragOutAng;

// -----------------------------------------------------------
void main (void)
{
	vec2 texCoord = texCoordVarying;
	
/*
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
	*/

	//newPos = spawnPosition.xyz;

	fragOutPosAndAge = vec4( newPos, age );
	fragOutVel = vec4( newVel, 1.0 );
	//fragOutVel = vec4( 1.0, 1.0, 1.0, 1.0 );	
}