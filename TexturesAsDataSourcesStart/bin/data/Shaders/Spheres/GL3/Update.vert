#version 330

#ifdef GL_ES
	// define default precision for float, vec, mat.
	precision highp float;
#endif

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

in vec4 position;
in vec4 color;
in vec4 normal;
in vec2 texcoord;

out vec4 colorVarying;
out vec2 texCoordVarying;

// -----------------------------------------------------------
void main()
{
	colorVarying = color;
	texCoordVarying = texcoord;
	
	gl_Position = projectionMatrix * modelViewMatrix * position;
}