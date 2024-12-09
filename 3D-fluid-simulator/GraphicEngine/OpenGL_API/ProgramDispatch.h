#pragma once

#include <memory>

#include "Program.h"

class ProgramDispatch
{
 private:
  Program _shaderPipelineSimple;
  Program _shaderPipelineWithTesselation;

 private:
  ProgramDispatch()
      : _shaderPipelineSimple{{GL_VERTEX_SHADER, "/ElementTessOff.vert"},
                              {GL_VERTEX_SHADER, "/CalculateNDC.glsl"},
                              {GL_FRAGMENT_SHADER, "/Element.frag"}},
        _shaderPipelineWithTesselation{
            {GL_VERTEX_SHADER, "/ElementTessOn.vert"},
            {GL_TESS_CONTROL_SHADER, "/Element.tesc"},
            {GL_TESS_EVALUATION_SHADER, "/Element.tese"},
            {GL_TESS_EVALUATION_SHADER, "/CalculateNDC.glsl"},
            {GL_FRAGMENT_SHADER, "/Element.frag"}}
  {
  }

 public:
  static std::unique_ptr<ProgramDispatch>& GetInstance()
  {
    static std::unique_ptr<ProgramDispatch> instance{nullptr};

    if (instance == nullptr)
    {
      instance = std::unique_ptr<ProgramDispatch>{new ProgramDispatch{}};
    }
    return instance;
  }

  static void CleanUp()
  {
    GetInstance().reset(nullptr);
  }

  Program& GetSimplePipeline()
  {
    return GetInstance()->_shaderPipelineSimple;
  }

  Program& GetTesselationPipeline()
  {
    return GetInstance()->_shaderPipelineWithTesselation;
  }
};
