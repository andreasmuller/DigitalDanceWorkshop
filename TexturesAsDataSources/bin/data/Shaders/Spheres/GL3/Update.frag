#version 330

precision highp float;

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform mat4 normalMatrix;

uniform sampler2D posAndAgeTexture;
uniform sampler2D velTexture;
uniform sampler2D spawnPositionsTexture;

uniform float time = 0.0;
uniform float timeStep = 0.01666666666667;
uniform float maxAge = 1.0;

uniform vec3 wind = vec3(0.0,0.0,0.0);
uniform vec3 gravity = vec3(0.0,-0.00001,0.0);

in vec4 colorVarying;
in vec2 texCoordVarying;

layout(location = 0) out vec4 fragOutPosAndAge;
layout(location = 1) out vec4 fragOutVel;

// -----------------------------------------------------------
void main (void)
{
	vec2 texCoord = texCoordVarying;

	vec4 posAndAge = texture( posAndAgeTexture, texCoord );
	vec3 pos = posAndAge.xyz;
	float age = posAndAge.w;

	vec3 vel = texture( velTexture, texCoord ).xyz;

	// Don't move this texture read below inside the if( age > maxAge ) statement to optimize,
	// never read a texture inside an if() statement, GPUs hate to do different things per pixel
	vec3 spawnPos = texture( spawnPositionsTexture, texCoord ).xyz; 

	age += (1.0 / 60.0) * 0.1; //timeStep;

//age = 0.0;

	if( age > maxAge )
	{
		age -= maxAge;
		pos = spawnPos;

		vel = vec3(0.0);
	}

	float ageFrac = clamp( age / maxAge, 0.0, 1.0);	

	vec3 frameVel = vec3(0.0); 
	 //vel; //vec3(0.0);
	//frameVel += wind;
	//frameVel += gravity;

	vel = frameVel;

	vec3 newPos = pos + vel;

// TEMP
newPos = spawnPos + vec3(ageFrac * 0.4,0.0,0.0);

/*
	if( newPos.y < 0.0 )
	{
		newPos.y = 0.0;
		vel.y *= -1.0;
		vel *= 0.99;
	}
*/
	fragOutPosAndAge = vec4( newPos, age );
	fragOutVel 		 = vec4( vel,    1.0 );
}