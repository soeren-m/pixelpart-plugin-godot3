#ifndef PIXELPART_SPRITE_H
#define PIXELPART_SPRITE_H

#include "PixelpartEffectResource.h"
#include "PixelpartCurve.h"
#include "PixelpartPath.h"
#include "PixelpartGradient.h"
#include "Sprite.h"
#include "ParticleEngine.h"
#include <Godot.hpp>
#include <Reference.hpp>

namespace godot {
class PixelpartSprite : public Reference {
	GODOT_CLASS(PixelpartSprite, Reference)

public:
	static void _register_methods();

	PixelpartSprite();
	~PixelpartSprite();

	void _init();
	void init(Ref<PixelpartEffectResource> resource, pixelpart::Sprite* sprite, pixelpart::ParticleEngine* engine);

	String get_name() const;

	void set_lifetime_start(float time);
	void set_lifetime_duration(float time);
	void set_repeat(bool value);
	float get_lifetime_start() const;
	float get_lifetime_duration() const;
	bool get_repeat() const;
	bool is_active() const;
	float get_local_time() const;

	void set_blend_mode(int mode);
	void set_color_mode(int mode);
	void set_layer(int layer);
	void set_visible(bool visible);
	int get_blend_mode() const;
	int get_color_mode() const;
	int get_layer() const;
	bool is_visible() const;

	Ref<PixelpartCurve> get_width() const;
	Ref<PixelpartCurve> get_height() const;
	Ref<PixelpartCurve> get_orientation() const;
	Ref<PixelpartPath> get_motion_path() const;
	Ref<PixelpartGradient> get_color() const;
	Ref<PixelpartCurve> get_opacity() const;

private:
	Ref<PixelpartEffectResource> effectResource;
	pixelpart::Sprite* nativeSprite = nullptr;
	pixelpart::ParticleEngine* nativeParticleEngine = nullptr;
};
}

#endif