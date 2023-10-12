#include "PixelpartShaders.h"
#include "PixelpartUtil.h"
#include "PixelpartShaderGraph.h"
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
		PixelpartShaderGraph_json,
		PixelpartShaderGraph_json + PixelpartShaderGraph_json_size);
	pixelpart::ShaderGraph::graphType = modelJson;
}
PixelpartShaders::~PixelpartShaders() {
	instance = nullptr;
}

void PixelpartShaders::_init() {

}

Ref<Shader> PixelpartShaders::get_canvas_shader(const std::string& shaderSource,
	pixelpart::BlendMode blendMode,
	CanvasItemMaterial::LightMode lightMode) {
	std::string renderMode;

	switch(blendMode) {
		case pixelpart::BlendMode::additive:
			renderMode = "blend_add";
			break;
		case pixelpart::BlendMode::subtractive:
			renderMode = "blend_sub";
			break;
		default:
			renderMode = "blend_mix";
			break;
	}

	switch(lightMode) {
		case CanvasItemMaterial::LIGHT_MODE_UNSHADED:
			renderMode += ",unshaded";
			break;
		case CanvasItemMaterial::LIGHT_MODE_LIGHT_ONLY:
			renderMode += ",light_only";
			break;
		default:
			break;
	}

	std::string vertexShaderSource = R"!(
		const float __PACK_FACTOR__ = 10000.0;
		VELOCITY = vec3((floor(COLOR.rg) - vec2(__PACK_FACTOR__)) / vec2(__PACK_FACTOR__), 0.0);
		FORCE = vec3((floor(COLOR.ba) - vec2(__PACK_FACTOR__)) / vec2(__PACK_FACTOR__), 0.0);
		LIFE = floor(UV.x) / __PACK_FACTOR__;
		OBJECT_ID = floor(UV.y) - 1.0;
		UV = fract(UV) * vec2(10.0);
		COLOR = fract(COLOR) * vec4(10.0);
	)!";

	std::string additionalFragmentShaderSource = R"!(COLOR = __SHADER_COLOR__;)!";

	return get_shader(shaderSource, "canvas_item", renderMode, vertexShaderSource, additionalFragmentShaderSource);
}
Ref<Shader> PixelpartShaders::get_spatial_shader(const std::string& shaderSource,
	pixelpart::BlendMode blendMode,
	bool unshaded, bool vertexLighting,
	SpatialMaterial::DiffuseMode diffuseMode,
	SpatialMaterial::SpecularMode specularMode,
	ParticleNormalMode normalMode) {
	std::string renderMode = "cull_disabled";

	switch(blendMode) {
		case pixelpart::BlendMode::additive:
			renderMode += ",blend_add";
			break;
		case pixelpart::BlendMode::subtractive:
			renderMode += ",blend_sub";
			break;
		default:
			renderMode += ",blend_mix";
			break;
	}

	if(unshaded) {
		renderMode += ",unshaded";
	}
	else if(vertexLighting) {
		renderMode += ",vertex_lighting";
	}

	switch(diffuseMode) {
		case SpatialMaterial::DIFFUSE_BURLEY:
			renderMode += ",diffuse_burley";
			break;
		case SpatialMaterial::DIFFUSE_LAMBERT:
			renderMode += ",diffuse_lambert";
			break;
		case SpatialMaterial::DIFFUSE_LAMBERT_WRAP:
			renderMode += ",diffuse_lambert_wrap";
			break;
		case SpatialMaterial::DIFFUSE_OREN_NAYAR:
			renderMode += ",diffuse_oren_nayar";
			break;
		case SpatialMaterial::DIFFUSE_TOON:
			renderMode += ",diffuse_toon";
			break;
		default:
			break;
	}

	switch(specularMode) {
		case SpatialMaterial::SPECULAR_SCHLICK_GGX:
			renderMode += ",specular_schlick_ggx";
			break;
		case SpatialMaterial::SPECULAR_BLINN:
			renderMode += ",specular_blinn";
			break;
		case SpatialMaterial::SPECULAR_PHONG:
			renderMode += ",specular_phong";
			break;
		case SpatialMaterial::SPECULAR_TOON:
			renderMode += ",specular_toon";
			break;
		case SpatialMaterial::SPECULAR_DISABLED:
			renderMode += ",specular_disabled";
			break;
		default:
			break;
	}

	std::string vertexShaderSource = R"!(
		const float __PACK_FACTOR__ = 10000.0;
		VELOCITY = TANGENT.xyz;
		FORCE = (floor(COLOR.rgb) - vec3(__PACK_FACTOR__)) / vec3(__PACK_FACTOR__);
		LIFE = UV2.x;
		OBJECT_ID = UV2.y;
		COLOR = fract(COLOR) * vec4(10.0);)!";

	switch(normalMode) {
		case PARTICLE_NORMAL_MODE_STATIC:
			vertexShaderSource += "\nNORMAL = normalize(u_StaticNormal);";
			break;
		default:
			vertexShaderSource += "\nNORMAL = normalize(NORMAL);";
			break;
	}

	std::string additionalFragmentShaderSource = R"!(
		if(!OUTPUT_IS_SRGB) {
			__SHADER_COLOR__.rgb = mix(pow((__SHADER_COLOR__.rgb + vec3(0.055)) * (1.0 / (1.0 + 0.055)), vec3(2.4)), __SHADER_COLOR__.rgb * (1.0 / 12.92), lessThan(__SHADER_COLOR__.rgb, vec3(0.04045)));
		}
		ALBEDO = __SHADER_COLOR__.rgb;
		ALPHA = __SHADER_COLOR__.a;
		METALLIC = u_Metallic;
		SPECULAR = u_Specular;
		ROUGHNESS = u_Roughness;)!";

	return get_shader(shaderSource, "spatial", renderMode, vertexShaderSource, additionalFragmentShaderSource);
}

Ref<Shader> PixelpartShaders::get_shader(const std::string& shaderSource,
	const std::string& shaderType,
	const std::string& renderMode,
	const std::string& vertexShaderSource,
	const std::string& additionalFragmentShaderSource) {
	std::string finalShaderSource =
		"shader_type " + shaderType +
		";\nrender_mode " + renderMode +
		";\n" + shaderSource;

	finalShaderSource = replace(finalShaderSource, "{SHADER_MAIN_START}", "");
	finalShaderSource = replace(finalShaderSource, "{SHADER_MAIN_END}", additionalFragmentShaderSource);
	finalShaderSource = replace(finalShaderSource, "{SHADER_VERTEX_MAIN}", vertexShaderSource);

	if(shaders.count(finalShaderSource) != 0) {
		return shaders.at(finalShaderSource);
	}

	Ref<Shader> shader;
	shader.instance();
	shader->set_code(String(finalShaderSource.c_str()));

	shaders[finalShaderSource] = shader;

	return shader;
}

std::string PixelpartShaders::replace(std::string str, const std::string& from, const std::string& to) {
	if(from.empty()) {
		return str;
	}

	std::size_t pos = 0;
	while((pos = str.find(from, pos)) != std::string::npos) {
		str.replace(pos, from.length(), to);
		pos += to.length();
	}

	return str;
}
}