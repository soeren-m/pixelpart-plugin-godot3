#include "PixelpartParticleMaterial2D.h"

namespace godot {
void PixelpartParticleMaterial2D::_register_methods() {
	register_property<PixelpartParticleMaterial2D, int>("light_mode",
		&PixelpartParticleMaterial2D::set_light_mode, &PixelpartParticleMaterial2D::get_light_mode,
		CanvasItemMaterial::LIGHT_MODE_NORMAL,
		GODOT_METHOD_RPC_MODE_DISABLED,
		GODOT_PROPERTY_USAGE_DEFAULT,
		GODOT_PROPERTY_HINT_ENUM, "Normal,Unshaded,Light-Only");

	register_method("_init", &PixelpartParticleMaterial2D::_init);
}

PixelpartParticleMaterial2D::PixelpartParticleMaterial2D() {

}

void PixelpartParticleMaterial2D::_init() {
	lightMode = CanvasItemMaterial::LIGHT_MODE_NORMAL;

	shaderDirty = true;
}

void PixelpartParticleMaterial2D::set_light_mode(int mode) {
	shaderDirty |= mode != static_cast<int>(lightMode);
	lightMode = static_cast<CanvasItemMaterial::LightMode>(mode);
}
int PixelpartParticleMaterial2D::get_light_mode() const {
	return static_cast<int>(lightMode);
}

void PixelpartParticleMaterial2D::set_shader_dirty(bool state) {
	shaderDirty = state;
}
bool PixelpartParticleMaterial2D::is_shader_dirty() const {
	return shaderDirty;
}
}