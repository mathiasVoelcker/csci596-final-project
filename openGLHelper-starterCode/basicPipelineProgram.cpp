#include "basicPipelineProgram.h"
#include "openGLHeader.h"
#include <iostream>
#include <cstring>

using namespace std;

int BasicPipelineProgram::Init(const char * shaderBasePath) 
{
  if (BuildShadersFromFiles(shaderBasePath, "basic.vertexShader.glsl", "basic.fragmentShader.glsl") != 0)
  {
    cout << "Failed to build the pipeline program." << endl;
    return 1;
  }

  cout << "Successfully built the pipeline program." << endl;
  return 0;
}

void BasicPipelineProgram::SetModelViewMatrix(const float * m) 
{
  // Pass "m" to the pipeline program, as the modelview matrix.
  glUniformMatrix4fv(h_modelViewMatrix, 1, GL_FALSE, m);
}

void BasicPipelineProgram::SetProjectionMatrix(const float * m) 
{
  // Pass "m" to the pipeline program, as the projection matrix.
  glUniformMatrix4fv(h_projectionMatrix, 1, GL_FALSE, m);
}

void BasicPipelineProgram::SetNormalMatrix(const float * n)
{
  glUniformMatrix4fv(h_normalMatrix, 1, GL_FALSE, n);
}

void BasicPipelineProgram::SetViewLightDirection(const float * v)
{
  glUniform3fv(h_viewLightDirection, 1, v);
}

void BasicPipelineProgram::SetPhong(const float * la,
    const float * ld,
    const float * ls,
    const float * ka,
    const float * kd,
    const float * ks,
    const float alpha)
{
  glUniform4fv(h_la, 1, la);
  glUniform4fv(h_ld, 1, ld);
  glUniform4fv(h_ls, 1, ls);
  glUniform4fv(h_ka, 1, ka);
  glUniform4fv(h_kd, 1, kd);
  glUniform4fv(h_ks, 1, ks);
  glUniform1f(h_alpha, alpha);
}

int BasicPipelineProgram::SetShaderVariableHandles() 
{
  // Set h_modelViewMatrix and h_projectionMatrix.
  SET_SHADER_VARIABLE_HANDLE(modelViewMatrix);
  SET_SHADER_VARIABLE_HANDLE(projectionMatrix);
  SET_SHADER_VARIABLE_HANDLE(viewLightDirection);
  SET_SHADER_VARIABLE_HANDLE(normalMatrix);
  SET_SHADER_VARIABLE_HANDLE(la);
  SET_SHADER_VARIABLE_HANDLE(ld);
  SET_SHADER_VARIABLE_HANDLE(ls);
  SET_SHADER_VARIABLE_HANDLE(ka);
  SET_SHADER_VARIABLE_HANDLE(kd);
  SET_SHADER_VARIABLE_HANDLE(ks);
  SET_SHADER_VARIABLE_HANDLE(alpha);
  // SET_SHADER_VARIABLE_HANDLE(mode);
  return 0;
}

