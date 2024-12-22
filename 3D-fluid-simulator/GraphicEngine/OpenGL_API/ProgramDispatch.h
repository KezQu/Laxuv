#pragma once

#include <memory>

#include "Program.h"

/**
 * @brief Class representing a singleton containing two rendering pipelines with
 * and without tesselation
 *
 */
class ProgramDispatch
{
 private:
  Program _shaderPipelineSimple;
  Program _shaderPipelineWithTesselation;

 private:
  /**
   * @brief Constructs a program dispatch with two basic rendering pipelines
   *
   */
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
  /**
   * @brief Getter to retrieve program dispatch instance
   *
   * @return std::unique_ptr<ProgramDispatch>&
   */
  static std::unique_ptr<ProgramDispatch>& GetInstance()
  {
    static std::unique_ptr<ProgramDispatch> instance{nullptr};

    // Creates new instance at the first call
    if (instance == nullptr)
    {
      instance = std::unique_ptr<ProgramDispatch>{new ProgramDispatch{}};
    }
    return instance;
  }

  /**
   * @brief Helper function responsible for explicit deletion of instance object
   * to prevent deletion of program objects after deletion of OpenGL context
   *
   */
  static void CleanUp()
  {
    GetInstance().reset(nullptr);
  }

  /**
   * @brief Getter responsible for providing access to pipeline without
   * tesselation
   *
   * @return Program&
   */
  Program& GetSimplePipeline()
  {
    return GetInstance()->_shaderPipelineSimple;
  }

  /**
   * @brief Getter responsible for providing access to pipeline with tesselation
   *
   * @return Program&
   */
  Program& GetTesselationPipeline()
  {
    return GetInstance()->_shaderPipelineWithTesselation;
  }
};
