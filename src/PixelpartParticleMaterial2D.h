#ifndef PIXELPART_PARTICLE_MATERIAL_2D_H
#define PIXELPART_PARTICLE_MATERIAL_2D_H

#include "PixelpartShaders.h"
#include "ParticleType.h"
#include <Resource.hpp>

namespace godot {
class PixelpartParticleMaterial2D : public Resource {
	GODOT_CLASS(PixelpartParticleMaterial2D, Resource)

public:
	static void _register_methods();

	PixelpartParticleMaterial2D();

	void _init();

	void set_light_mode(int mode);
	int get_light_mode() const;

	void set_shader_dirty(bool state);
	bool is_shader_dirty() const;

private:
	CanvasItemMaterial::LightMode lightMode = CanvasItemMaterial::LIGHT_MODE_NORMAL;

	bool shaderDirty = true;
};
}

#endif