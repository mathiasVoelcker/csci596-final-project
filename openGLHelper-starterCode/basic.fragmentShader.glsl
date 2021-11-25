#version 150

in vec3 viewPosition;
in vec3 viewNormal;
in vec4 col;
out vec4 c;

uniform vec4 la;
uniform vec4 ld;
uniform vec4 ls;
uniform vec3 viewLightDirection;
uniform vec4 ka;
uniform vec4 kd;
uniform vec4 ks;
uniform float alpha;

void main()
{
  // compute the final pixel color
  vec3 eyedir = normalize(vec3(0, 0, 0) - viewPosition);

  vec3 reflectDir = -reflect(viewLightDirection, viewNormal);

  float d = max(dot(viewLightDirection, viewNormal), 0.0f);
  float s = max(dot(reflectDir, eyedir), 0.0f);
  c = ka * la + d * kd * ld + pow(s, alpha) * ks * ls;
  c = col * c;
  
}

