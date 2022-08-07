#include "PixelpartShaders.h"
#include "PixelpartUtil.h"
#include "ShaderGraph.h"
#include <VisualServer.hpp>

namespace godot {
PixelpartShaders* PixelpartShaders::instance = nullptr;
PixelpartShaders* PixelpartShaders::get_instance() {
	return instance;
}

void PixelpartShaders::_register_methods() {

}

PixelpartShaders::PixelpartShaders() {
	instance = this;

	nlohmann::ordered_json modelJson = nlohmann::json::parse(
		#include "PixelpartShaderGraph.json"
	);
	pixelpart::ShaderGraph::graphType = modelJson;
}
PixelpartShaders::~PixelpartShaders() {
	VisualServer* vs = VisualServer::get_singleton();
	for(auto& entry : shaders) {
		vs->free_rid(entry.second);
	}

	instance = nullptr;
}

void PixelpartShaders::_init() {

}

RID PixelpartShaders::get_shader(const std::string& shaderSource, const std::string& shaderType, const std::string& renderMode) {
	std::string finalShaderSource =
		"shader_type " + shaderType +
		";\nrender_mode " + renderMode +
		";\n" + shaderSource;

	if(shaderType == "canvas_item") {
		finalShaderSource = replace(finalShaderSource, "", "{SHADER_MAIN_START}");
		finalShaderSource = replace(finalShaderSource, "COLOR = __SHADER_COLOR__;", "{SHADER_MAIN_END}");
		finalShaderSource = replace(finalShaderSource, "VELOCITY = (floor(COLOR.rg) - vec2(10000.0)) / vec2(10000.0);\nFORCE = (floor(COLOR.ba) - vec2(10000.0)) / vec2(10000.0);\nLIFE = floor(UV.x) / 10000.0;\nOBJECT_ID = floor(UV.y);\nUV = fract(UV) * vec2(2.0);\nCOLOR = fract(COLOR) * vec4(2.0);", "{SHADER_VERTEX_MAIN}");
	}
	else if(shaderType == "spatial") {
		finalShaderSource = replace(finalShaderSource, "", "{SHADER_MAIN_START}");
		finalShaderSource = replace(finalShaderSource, "if(!OUTPUT_IS_SRGB) { __SHADER_COLOR__.rgb = mix(pow((__SHADER_COLOR__.rgb + vec3(0.055)) * (1.0 / (1.0 + 0.055)), vec3(2.4)), __SHADER_COLOR__.rgb * (1.0 / 12.92), lessThan(__SHADER_COLOR__.rgb, vec3(0.04045))); }\nALBEDO = __SHADER_COLOR__.rgb;\n\nALPHA = __SHADER_COLOR__.a;", "{SHADER_MAIN_END}");
		finalShaderSource = replace(finalShaderSource, "VELOCITY = NORMAL.xy;\nFORCE = TANGENT.xy;\nLIFE = UV2.x;\nOBJECT_ID = UV2.y;", "{SHADER_VERTEX_MAIN}");
	}
	else {
		return RID();
	}

	if(shaders.count(finalShaderSource) != 0) {
		return shaders.at(finalShaderSource);
	}

	VisualServer* vs = VisualServer::get_singleton();
	RID shader = vs->shader_create();
	vs->shader_set_code(shader, String(finalShaderSource.c_str()));

	shaders[finalShaderSource] = shader;

	return shader;
}
}