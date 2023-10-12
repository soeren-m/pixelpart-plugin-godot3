#ifndef PIXELPART_SHADERS_H
#define PIXELPART_SHADERS_H

#include "BlendMode.h"
#include "ParticleNormalMode.h"
#include <Godot.hpp>
#include <Node.hpp>
#include <Shader.hpp>
#include <CanvasItemMaterial.hpp>
#include <SpatialMaterial.hpp>
#include <unordered_map>
#include <string>

namespace godot {
class PixelpartShaders : public Node {
	GODOT_CLASS(PixelpartShaders, Node)

public:
	static PixelpartShaders* get_instance();

	static void _register_methods();

	PixelpartShaders();
	~PixelpartShaders();

	void _init();

	Ref<Shader> get_canvas_shader(const std::string& shaderSource,
		pixelpart::BlendMode blendMode,
		CanvasItemMaterial::LightMode lightMode);
	Ref<Shader> get_spatial_shader(const std::string& shaderSource,
		pixelpart::BlendMode blendMode,
		bool unshaded, bool vertexLighting,
		SpatialMaterial::DiffuseMode diffuseMode,
		SpatialMaterial::SpecularMode specularMode,
		ParticleNormalMode normalMode);

	Ref<Shader> get_shader(const std::string& shaderSource,
		const std::string& shaderType,
		const std::string& renderMode,
		const std::string& vertexShaderSource,
		const std::string& additionalFragmentShaderSource);

private:
	std::string replace(std::string str, const std::string& from, const std::string& to);

	static PixelpartShaders* instance;

	std::unordered_map<std::string, Ref<Shader>> shaders;
};
}

#endif