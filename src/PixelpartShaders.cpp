#include "PixelpartShaders.h"
#include <VisualServer.hpp>

namespace godot {
const char* PixelpartShaders::shaderSourceCanvasItem =
R""(
uniform int u_ColorMode = 0;
uniform float u_AlphaThreshold = 0.0;

vec4 blend(vec4 color1, vec4 color2, int mode) {
	vec4 outColor = vec4(1, 1, 1, 1);

	if(mode == 0) {
		outColor = color1 * color2;
	}
	else if(mode == 1) {
		outColor.rgb = clamp(color1.rgb + color2.rgb, vec3(0.0), vec3(1.0));
		outColor.a = color1.a * color2.a;
	}
	else if(mode == 2) {
		outColor.rgb = clamp(color1.rgb - color2.rgb, vec3(0.0), vec3(1.0));
		outColor.a = color1.a * color2.a;
	}
	else if(mode == 3) {
		outColor.rgb = abs(color1.rgb - color2.rgb);
		outColor.a = color1.a * color2.a;
	}
	else if(mode == 4) {
		outColor.rgb = vec3(1.0) - (vec3(1.0) - color1.rgb) * (vec3(1.0) - color2.rgb);
		outColor.a = color1.a * color2.a;
	}
	else if(mode == 5) {
		outColor.r = (color1.r < 0.5) ? (2.0 * color1.r * color2.r) : (1.0 - 2.0 * (1.0 - color1.r) * (1.0 - color2.r));
		outColor.g = (color1.g < 0.5) ? (2.0 * color1.g * color2.g) : (1.0 - 2.0 * (1.0 - color1.g) * (1.0 - color2.g));
		outColor.b = (color1.b < 0.5) ? (2.0 * color1.b * color2.b) : (1.0 - 2.0 * (1.0 - color1.b) * (1.0 - color2.b));
		outColor.a = color1.a * color2.a;
	}
	else if(mode == 6) {
		outColor.rgb = max(color1.rgb, color2.rgb);
		outColor.a = color1.a * color2.a;
	}
	else if(mode == 7) {
		outColor.rgb = min(color1.rgb, color2.rgb);
		outColor.a = color1.a * color2.a;
	}
	else if(mode == 8) {
		outColor.rgb = color2.rgb;
		outColor.a = color1.a;
	}

	return outColor;
}

void fragment() {
	vec4 finalColor = blend(texture(TEXTURE, UV), COLOR, u_ColorMode);
	if(finalColor.a <= u_AlphaThreshold) {
		discard;
	}

	COLOR = finalColor;
}
)"";

const char* PixelpartShaders::shaderSourceSpatial =
R""(
uniform sampler2D u_Texture0 : hint_albedo;
uniform int u_ColorMode = 0;
uniform float u_AlphaThreshold = 0.0;

vec4 blend(vec4 color1, vec4 color2, int mode) {
	vec4 outColor = vec4(1, 1, 1, 1);

	if(mode == 0) {
		outColor = color1 * color2;
	}
	else if(mode == 1) {
		outColor.rgb = clamp(color1.rgb + color2.rgb, vec3(0.0), vec3(1.0));
		outColor.a = color1.a * color2.a;
	}
	else if(mode == 2) {
		outColor.rgb = clamp(color1.rgb - color2.rgb, vec3(0.0), vec3(1.0));
		outColor.a = color1.a * color2.a;
	}
	else if(mode == 3) {
		outColor.rgb = abs(color1.rgb - color2.rgb);
		outColor.a = color1.a * color2.a;
	}
	else if(mode == 4) {
		outColor.rgb = vec3(1.0) - (vec3(1.0) - color1.rgb) * (vec3(1.0) - color2.rgb);
		outColor.a = color1.a * color2.a;
	}
	else if(mode == 5) {
		outColor.r = (color1.r < 0.5) ? (2.0 * color1.r * color2.r) : (1.0 - 2.0 * (1.0 - color1.r) * (1.0 - color2.r));
		outColor.g = (color1.g < 0.5) ? (2.0 * color1.g * color2.g) : (1.0 - 2.0 * (1.0 - color1.g) * (1.0 - color2.g));
		outColor.b = (color1.b < 0.5) ? (2.0 * color1.b * color2.b) : (1.0 - 2.0 * (1.0 - color1.b) * (1.0 - color2.b));
		outColor.a = color1.a * color2.a;
	}
	else if(mode == 6) {
		outColor.rgb = max(color1.rgb, color2.rgb);
		outColor.a = color1.a * color2.a;
	}
	else if(mode == 7) {
		outColor.rgb = min(color1.rgb, color2.rgb);
		outColor.a = color1.a * color2.a;
	}
	else if(mode == 8) {
		outColor.rgb = color2.rgb;
		outColor.a = color1.a;
	}

	return outColor;
}

void fragment() {
	vec4 finalColor = blend(texture(u_Texture0, UV), COLOR, u_ColorMode);
	if(finalColor.a <= u_AlphaThreshold) {
		discard;
	}

	ALBEDO = finalColor.rgb;
	ALPHA = finalColor.a;
}
)"";

PixelpartShaders* PixelpartShaders::instance = nullptr;
PixelpartShaders* PixelpartShaders::get_instance() {
	return instance;
}

void PixelpartShaders::_register_methods() {

}

PixelpartShaders::PixelpartShaders() {
	instance = this;

	VisualServer* vs = VisualServer::get_singleton();

	shaderCanvasItem[0] = vs->shader_create();
	shaderCanvasItem[1] = vs->shader_create();
	shaderCanvasItem[2] = vs->shader_create();
	vs->shader_set_code(shaderCanvasItem[0], String("shader_type canvas_item;\nrender_mode blend_mix;\n") + String(shaderSourceCanvasItem));
	vs->shader_set_code(shaderCanvasItem[1], String("shader_type canvas_item;\nrender_mode blend_add;\n") + String(shaderSourceCanvasItem));
	vs->shader_set_code(shaderCanvasItem[2], String("shader_type canvas_item;\nrender_mode blend_sub;\n") + String(shaderSourceCanvasItem));

	shaderSpatial[0] = vs->shader_create();
	shaderSpatial[1] = vs->shader_create();
	shaderSpatial[2] = vs->shader_create();
	vs->shader_set_code(shaderSpatial[0], String("shader_type spatial;\nrender_mode cull_disabled,blend_mix;\n") + String(shaderSourceSpatial));
	vs->shader_set_code(shaderSpatial[1], String("shader_type spatial;\nrender_mode cull_disabled,blend_add;\n") + String(shaderSourceSpatial));
	vs->shader_set_code(shaderSpatial[2], String("shader_type spatial;\nrender_mode cull_disabled,blend_sub;\n") + String(shaderSourceSpatial));
}
PixelpartShaders::~PixelpartShaders() {
	instance = nullptr;
}

void PixelpartShaders::_init() {

}

RID PixelpartShaders::get_shader_canvasitem(pixelpart::BlendMode blendMode) const {
	if(static_cast<uint32_t>(blendMode) >= 3) {
		return RID();
	}

	return shaderCanvasItem[static_cast<uint32_t>(blendMode)];
}
RID PixelpartShaders::get_shader_spatial(pixelpart::BlendMode blendMode) const {
	if(static_cast<uint32_t>(blendMode) >= 3) {
		return RID();
	}

	return shaderSpatial[static_cast<uint32_t>(blendMode)];
}
}