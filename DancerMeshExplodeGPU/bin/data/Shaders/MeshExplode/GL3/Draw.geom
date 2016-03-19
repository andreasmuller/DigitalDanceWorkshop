#version 330

#pragma include "../../Common/ShaderHelpers.glslinc"

layout(points) in;
layout(triangle_strip, max_vertices=6) out;

precision highp float;

#define MAX_LIGHTS 4

in VertexAttrib {
	vec3 normal;
	vec2 texcoord;
	vec3 viewDir;
	vec3 lightDir[MAX_LIGHTS];
} vertex[];


out VertexAttrib {
	vec3 normal;
	vec2 texcoord;
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

uniform int	numActiveLights;
uniform float lightRadius[MAX_LIGHTS];
uniform vec3  lightPositionWorld[MAX_LIGHTS];
uniform vec3  lightPositionCamera[MAX_LIGHTS];

// ----------------------------------------------
void setVertexOutLightParamsForModelSpaceVertex( vec3 _v )
{
	vec4 v = vec4( _v, 1 );

	vec4 vertexCameraPos = modelViewMatrix * v;
	vertexOut.viewDir = -vertexCameraPos.xyz;
	
	for ( int i = 0; i < numActiveLights; i++ )
	{
		vertexOut.lightDir[i] = vec3(lightPositionCamera[i] - vertexCameraPos.xyz) / lightRadius[i];
	}	
}

// ----------------------------------------------
void main()
{
	vec2 texCoord = vertex[0].texcoord;

	// TEMP, gotta recompute light stuff
	vertexOut.normal = vertex[0].normal;
	vertexOut.texcoord = vertex[0].texcoord;
	vertexOut.viewDir = vertex[0].viewDir;
	for ( int i = 0; i < MAX_LIGHTS; i++ )
	{
		vertexOut.lightDir[i] = vertex[0].lightDir[i];
	}

	//vec3 pos = gl_in[0].gl_Position.xyz;
	vec3 pos = texture( posTex, texCoord ).xyz;
	vec3 angles = texture( angTex, texCoord ).xyz;
	vec4 random = texture( randomTex, texCoord );	

	vec3 v0 = texture( vertex0Tex, texCoord ).xyz;
	vec3 v1 = texture( vertex1Tex, texCoord ).xyz;
	vec3 v2 = texture( vertex2Tex, texCoord ).xyz;	

	mat4 rotMat = rotationMatrix(vec3(0,0,1), angles.x ) * 
				  rotationMatrix(vec3(0,1,0), angles.y );

	v0 = (rotMat * vec4(v0,1)).xyz;
	v1 = (rotMat * vec4(v1,1)).xyz;
	v2 = (rotMat * vec4(v2,1)).xyz;

	vec3 v0World = pos + v0.xyz;
	vec3 v1World = pos + v1.xyz;
	vec3 v2World = pos + v2.xyz;

	vec3 triangleNormal	= getTriangleNormal( v0World, v1World, v2World );

	// v0
	vertexOut.normal = triangleNormal;
	setVertexOutLightParamsForModelSpaceVertex( v0World );
	gl_Position = modelViewProjectionMatrix * vec4(v0World, 1.0 );
	EmitVertex();

	// v1
	vertexOut.normal = triangleNormal;
	setVertexOutLightParamsForModelSpaceVertex( v1World );	
	gl_Position = modelViewProjectionMatrix * vec4(v1World, 1.0 );
	EmitVertex();

	// v2
	vertexOut.normal = triangleNormal;
	setVertexOutLightParamsForModelSpaceVertex( v2World );	
	gl_Position = modelViewProjectionMatrix * vec4(v2World, 1.0 );
	EmitVertex();		

	EndPrimitive();

}