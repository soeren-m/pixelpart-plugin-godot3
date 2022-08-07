#ifndef PIXELPART_SHADERS_H
#define PIXELPART_SHADERS_H

#include <Godot.hpp>
#include <Node.hpp>
#include <unordered_map>

namespace godot {
class PixelpartShaders : public Node {
	GODOT_CLASS(PixelpartShaders, Node)

public:
	static PixelpartShaders* get_instance();

	static void _register_methods();

	PixelpartShaders();
	~PixelpartShaders();

	void _init();

	RID get_shader(const std::string& shaderSource, const std::string& shaderType, const std::string& renderMode);

private:
	static PixelpartShaders* instance;

	std::unordered_map<std::string, RID> shaders;
};
}

#endif