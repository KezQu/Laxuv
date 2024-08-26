#include "ProgramDispatch.h"

ProgramDispatch::ProgramDispatch()
	:_shaderPipelineSimple{ 
		{ GL_VERTEX_SHADER, "/ElementTessOff.vert" },
		{ GL_VERTEX_SHADER, "/CalculateNDC.glsl" },
		{ GL_FRAGMENT_SHADER, "/Element.frag" }},
	_shaderPipelineWithTesselation{
		{ GL_VERTEX_SHADER, "/ElementTessOn.vert" },
		{ GL_TESS_CONTROL_SHADER, "/Element.tesc" },
		{ GL_TESS_EVALUATION_SHADER, "/Element.tese" },
		{ GL_TESS_EVALUATION_SHADER, "/CalculateNDC.glsl" },
		{ GL_FRAGMENT_SHADER, "/Element.frag" }}
{
	//if (_primitiveType == GL_LINES) {
	//	_renderer.AddShader(GL_GEOMETRY_SHADER, "/Lines.geom");
	//}
	//else if (_primitiveType == GL_POINTS) {
	//	_renderer.AddShader(GL_GEOMETRY_SHADER, "/Points.geom");
	//}
	//else {
	//	_renderer.AddShader(GL_GEOMETRY_SHADER, "/Triangles.geom");
	//}
	//_renderer.AddShader(GL_GEOMETRY_SHADER, "/CalculateNDC.glsl");
	//_renderer.AddShader(GL_GEOMETRY_SHADER, "/CalculateNormal.glsl");
}

ProgramDispatch& ProgramDispatch::GetInstance()
{
	static ProgramDispatch instance;
	return instance;
}

void ProgramDispatch::CleanUp()
{
	_shaderPipelineSimple.~Program();
	_shaderPipelineWithTesselation.~Program();
}

Program& ProgramDispatch::GetSimplePipeline()
{
	return GetInstance()._shaderPipelineSimple;
}

Program& ProgramDispatch::GetTesselationPipeline()
{
	return GetInstance()._shaderPipelineWithTesselation;
}
