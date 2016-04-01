#version 330

#pragma include "../../Common/ShaderHelpers.glslinc"

layout(points) in;
layout(triangle_strip, max_vertices=6) out;

precision highp float;

#define MAX_LIGHTS 3

in VertexAttrib {
	vec3 normal;
	vec2 texcoord;
} vertex[];


out VertexAttrib {
	vec3 normal;
	vec2 texcoord;
	vec4 color;	
	vec3 viewDir;
	vec3 lightDir[MAX_LIGHTS];
} vertexOut;


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

uniform vec4 startColor;
uniform vec4 endColor;	

uniform int	numActiveLights;
uniform float lightRadius[MAX_LIGHTS];
uniform vec3  lightPositionWorld[MAX_LIGHTS];
uniform vec3  lightPositionCamera[MAX_LIGHTS];

// Includes our TriangleData and the getTriangleData function
#pragma include "Triangle.glslinc"

vec4 currentColor = vec4(1.0,1.0,1.0,1.0);	

// ----------------------------------------------
void setVertexOutLightParamsForWorldSpaceVertex( vec3 _v, vec3 _n )
{
	vec4 v = vec4( _v, 1 );

	vec4 vertexCameraPos = modelViewMatrix * v;
	vertexOut.viewDir = -vertexCameraPos.xyz;
	vertexOut.color = currentColor;

	for ( int i = 0; i < numActiveLights; i++ )
	{
		vertexOut.lightDir[i] = vec3(lightPositionCamera[i] - vertexCameraPos.xyz) / lightRadius[i];
	}	

	vertexOut.normal = (normalMatrix * vec4(_n,0.0)).xyz;
	gl_Position = modelViewProjectionMatrix * v;
}

// ----------------------------------------------
void main()
{
	vec2 texCoord = vertex[0].texcoord;

	//vec3 pos = gl_in[0].gl_Position.xyz;
	vec3 pos = texture( posTex, texCoord ).xyz;
	vec4 angles = texture( angTex, texCoord );
	vec4 random = texture( randomTex, texCoord );	

	float triangleScale = smoothStepOut( meshMaxAge * 0.95, meshMaxAge, meshAge ); // make triangle smaller towards the end of it's life

	vec3 v0Model = texture( vertex0Tex, texCoord ).xyz * triangleScale;
	vec3 v1Model = texture( vertex1Tex, texCoord ).xyz * triangleScale;
	vec3 v2Model = texture( vertex2Tex, texCoord ).xyz * triangleScale; 	

	TriangleData triangleData = getTriangleData( pos, v0Model, v1Model, v2Model, angles );

	currentColor = mix( startColor, endColor, map( meshAge, 0.0, meshMaxAge, 0.0, 1.0) );

	// v0
	setVertexOutLightParamsForWorldSpaceVertex( triangleData.v0World, triangleData.normal );
	EmitVertex();

	// v1
	setVertexOutLightParamsForWorldSpaceVertex( triangleData.v1World, triangleData.normal );	
	EmitVertex();

	// v2
	setVertexOutLightParamsForWorldSpaceVertex( triangleData.v2World, triangleData.normal );	
	EmitVertex();		

	EndPrimitive();

}