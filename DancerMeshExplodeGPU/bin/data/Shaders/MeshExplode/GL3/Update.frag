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

uniform float time;

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

uniform float meshAge = 0.0;
uniform float meshMaxAge = 1.0;

uniform vec3 wind = vec3(0.0,0.0,0.0);

uniform float maxRotation = 0.1;

uniform float triangleNormalVel = 0.0;
uniform float triangleNormalDrag = 1.0;

uniform float noisePositionFrequency = 0.1; 
uniform float noiseMagnitude = 0.075;
uniform float noiseTimeFrequency = 1.0;
uniform float noisePersistence = 0.2;

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

	//vec3 pos = gl_in[0].gl_Position.xyz;
	vec3 pos = texture( posTex, texCoord ).xyz;

	vec4 angles = texture( angTex, texCoord );
	vec4 random = texture( randomTex, texCoord );	

	vec3 v0 = texture( vertex0Tex, texCoord ).xyz;
	vec3 v1 = texture( vertex1Tex, texCoord ).xyz;
	vec3 v2 = texture( vertex2Tex, texCoord ).xyz;	

	float triangleScale = linearStepOut( meshMaxAge * 0.8, meshMaxAge, meshAge );

	float restEnd = 0.02;		// vary these to make the mesh hold it's shape longer
	float rampupLength = 0.05;	// unit is age normalized
	float forceMagnitude = linearStep( meshMaxAge * restEnd, meshMaxAge * (restEnd+rampupLength), meshAge );

	angles.x += map( random.y, 0, 1, -maxRotation, maxRotation ) * forceMagnitude;  
	angles.y += map( random.z, 0, 1, -maxRotation, maxRotation ) * forceMagnitude;	

	TriangleData triangleData = getTriangleData( pos, v0, v1, v2, angles );

	vec3 frameVel = vec3(0.0);
	frameVel += wind;
	frameVel += triangleData.normal * triangleNormalVel;
	frameVel += curlNoise( triangleData.pos * noisePositionFrequency, time * noiseTimeFrequency, OCTAVES, noisePersistence ) * noiseMagnitude;

	// Fake some drag by slowing the frameVel the more we are facing the triangle normal  
	float amountTriangleNormalFacesTravelDir = dot( triangleData.normal, normalize(frameVel) ); // 1 pointing the same way, -1 pointing opposite
	amountTriangleNormalFacesTravelDir = abs(amountTriangleNormalFacesTravelDir); // both sides have drag
	frameVel *= map( amountTriangleNormalFacesTravelDir, 0.0, 1.0,   1.0, triangleNormalDrag );

	frameVel *= forceMagnitude; // we ramp the forces up as the mesh ages

	vec3 newPos = pos + frameVel;

	newPos.y = clamp( newPos.y, 0.0, 9999999.0 ); // don't go below the 'floor'

	fragOutPos = vec4( newPos, 1.0 );
	fragOutAng = angles;
}