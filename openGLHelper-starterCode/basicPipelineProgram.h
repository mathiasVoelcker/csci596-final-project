#include "pipelineProgram.h"

class BasicPipelineProgram : public PipelineProgram
{
public:
  int Init(const char * shaderBasePath); // Init the program; "shaderBasePath" is the path to the folder containing the shaders.
  void SetModelViewMatrix(const float * m); // m is column-major
  void SetProjectionMatrix(const float * m); // m is column-major
  void SetNormalMatrix(const float * m);
  void SetViewLightDirection(const float * v);
  void SetPhong(
    const float * la,
    const float * ld,
    const float * ls,
    const float * ka,
    const float * kd,
    const float * ks,
    const float alpha);

protected:
  virtual int SetShaderVariableHandles();

  GLint h_projectionMatrix; // handle to the projectionMatrix variable in the shader
  GLint h_modelViewMatrix; // handle to the modelViewMatrix variable in the shader
  GLint h_normalMatrix;
  GLint h_viewLightDirection;
  // Note: we use the following naming convention: h_name is a handle to the shader variable "name".
  GLint h_la;
  GLint h_ld;
  GLint h_ls;
  GLint h_ka;
  GLint h_kd;
  GLint h_ks;
  GLint h_alpha;
};

