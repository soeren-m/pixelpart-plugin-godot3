#include "PixelpartEffect.h"
#include "PixelpartEffect2D.h"
#include "PixelpartShaders.h"

extern "C" void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *o) {
	godot::Godot::gdnative_init(o);
}

extern "C" void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options *o) {
	godot::Godot::gdnative_terminate(o);
}

extern "C" void GDN_EXPORT godot_nativescript_init(void *handle) {
	godot::Godot::nativescript_init(handle);

	godot::register_class<godot::PixelpartCurve>();
	godot::register_class<godot::PixelpartCurve3>();
	godot::register_class<godot::PixelpartGradient>();
	godot::register_class<godot::PixelpartShaders>();
	godot::register_class<godot::PixelpartEffectResource>();
	godot::register_class<godot::PixelpartParticleEmitter>();
	godot::register_class<godot::PixelpartParticleType>();
	godot::register_class<godot::PixelpartForceField>();
	godot::register_class<godot::PixelpartCollider>();
	godot::register_class<godot::PixelpartEffect>();
	godot::register_class<godot::PixelpartEffect2D>();
}