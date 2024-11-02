#pragma once

#include <Program.h>

#include <memory>

class ProgramDispatch
{
 private:
  Program _shaderPipelineSimple;
  Program _shaderPipelineWithTesselation;

 private:
  ProgramDispatch();

 public:
  static std::unique_ptr<ProgramDispatch>& GetInstance();
  static void CleanUp();
  Program& GetSimplePipeline();
  Program& GetTesselationPipeline();
};
