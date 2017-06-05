#version 430

in vec4  position;
in vec4  color;

//out vec4 colorVarying;
uniform mat4 modelViewProjectionMatrix;
uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;

void main()
{
    //gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

    vec4 eyeCoord = modelViewMatrix * position;
    gl_Position = projectionMatrix * eyeCoord; 
    //colorVarying = color;
}