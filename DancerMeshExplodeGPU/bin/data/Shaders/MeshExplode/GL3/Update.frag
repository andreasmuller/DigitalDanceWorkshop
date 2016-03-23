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

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform mat4 normalMatrix;

uniform sampler2D posTex;
uniform sampler2D angTex;
uniform sampler2D vertex0Tex;
uniform sampler2D vertex1Tex;
uniform sampler2D vertex2Tex;

uniform sampler2D randomTex;

#define OCTAVES 3

in vec4 colorVarying;
in vec2 texCoordVarying;

layout(location = 0) out vec4 fragOutPos;
layout(location = 1) out vec4 fragOutAng;

// Includes our TriangleData and the getTriangleData function
#pragma include "Triangle.glslinc"


// -----------------------------------------------------------
void main (void)
{
	vec2 texCoord = texCoordVarying;

	vec2 texCoord = vertex[0].texcoord;

	//vec3 pos = gl_in[0].gl_Position.xyz;
	vec3 pos = texture( posTex, texCoord ).xyz;
	vec4 angles = texture( angTex, texCoord );
	vec4 random = texture( randomTex, texCoord );	

	vec3 v0 = texture( vertex0Tex, texCoord ).xyz;
	vec3 v1 = texture( vertex1Tex, texCoord ).xyz;
	vec3 v2 = texture( vertex2Tex, texCoord ).xyz;	

	TriangleData triangleData = getTriangleData( pos, v0, v1, v2, angles );


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

	//triangleData.normal *
	vec3 newPos = pos + vec3(0,0,0.001);

	fragOutPosAndAge = vec4( newPos, 1 );
	fragOutVel = vec4( 0.0f,0.0,0.0,0.0 );
	//fragOutVel = vec4( 1.0, 1.0, 1.0, 1.0 );	
}