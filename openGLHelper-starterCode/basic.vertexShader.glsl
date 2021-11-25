#version 150

in vec3 position;
in vec3 normal;

in vec4 color;
out vec4 col;

out vec3 viewPosition;
out vec3 viewNormal;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 normalMatrix;

void main()
{
  // compute the transformed and projected vertex position (into gl_Position) 
  // compute the vertex color (into col)
  col = color;


  vec4 viewPosition4 = modelViewMatrix * vec4(position, 1.0f);
  // gl_Position = projectionMatrix * viewPosition4;

  viewPosition = viewPosition4.xyz;
  viewNormal = normalize((normalMatrix*vec4(normal, 0.0f)).xyz);
  
  gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0f);

}

