#ifndef PIXELPART_SHADERS_H
#define PIXELPART_SHADERS_H

#include "BlendMode.h"
#include <Godot.hpp>
#include <Node.hpp>

namespace godot {
class PixelpartShaders : public Node {
	GODOT_CLASS(PixelpartShaders, Node)

public:
	static PixelpartShaders* get_instance();

	static void _register_methods();

	PixelpartShaders();
	~PixelpartShaders();

	void _init();

	RID get_shader_canvasitem(pixelpart::BlendMode blendMode) const;
	RID get_shader_spatial(pixelpart::BlendMode blendMode) const;

private:
	static PixelpartShaders* instance;

	static const char* shaderSourceCanvasItem;
	static const char* shaderSourceSpatial;

	RID shaderCanvasItem[3];
	RID shaderSpatial[3];
};
}

#endif