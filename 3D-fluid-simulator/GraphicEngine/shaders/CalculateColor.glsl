#version 460 core

const uint MaxNeighbours =  512;
const uint NONE = 0;

const uint CUSTOM = 1;
const uint VELOCITY = 2;
const uint DENSITY_ERROR = 3;
const uint DIVERGENCE_ERROR = 4;
const uint PRESSURE = 5;

uniform float mass;

uniform uint colorType;
uniform vec4 color;

struct ParticleProperties {
	vec4 velocityDFSPHfactor;
	vec4 position;
	vec4 VolumeDensityPressureDro_Dt;
  	vec4 color;
	uint neighbours[MaxNeighbours];
};

vec4 CalculateColor(float property_value){
	vec4 chosen_color = color;
	const float boundCheck = 0.25f;
	
	if (property_value <= -1.f){
		chosen_color.xyz = vec3(1);
	}
	else if(property_value < 0.f){
		chosen_color.xyz = vec3(0, 0, 1 - property_value);
	}
	else if(property_value < boundCheck){
		chosen_color.xyz = vec3(0, property_value * 4 , 1);
	}
	else if(property_value < 2 * boundCheck){
		chosen_color.xyz = vec3(0, 1, 1 - (property_value - boundCheck) * 4);
	}
	else if(property_value < 3 * boundCheck){
		chosen_color.xyz = vec3((property_value - 2 * boundCheck) * 4, 1, 0);
	}
	else if(property_value <= 4 * boundCheck){
		chosen_color.xyz = vec3(1, 1 - (property_value - 3 * boundCheck) * 4, 0);
	}
	else if(property_value <= 5 * boundCheck){
		chosen_color.xyz = vec3(1 - (property_value - 4 * boundCheck) * 4, 0, 0);
	}
	else{
		chosen_color.xyz = vec3(0);
	}
	return chosen_color;
}

vec4 ChooseColor(ParticleProperties properties){
	const float max_speed = 50.f;
	const float density0 = mass;
	vec4 chosen_color = vec4(0);
	switch(colorType){
		case CUSTOM:
			chosen_color = color;
			break;
		case VELOCITY:
			chosen_color = CalculateColor(length(properties.velocityDFSPHfactor.xyz) / max_speed);
			break;
		case DENSITY_ERROR:
			chosen_color = CalculateColor(abs(properties.VolumeDensityPressureDro_Dt.y - density0) / density0);
			break;
		case DIVERGENCE_ERROR:
			chosen_color = CalculateColor(abs(properties.VolumeDensityPressureDro_Dt.w) * 1e+2);
			break;
		case PRESSURE:
			chosen_color = CalculateColor(2 * log(abs(properties.VolumeDensityPressureDro_Dt.z) + 1) / (mass * 1e+3));
			break;
	}
	return chosen_color;
}
