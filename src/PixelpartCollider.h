#ifndef PIXELPART_COLLIDER_H
#define PIXELPART_COLLIDER_H

#include "PixelpartEffectResource.h"
#include "PixelpartCurve.h"
#include "Collider.h"
#include "ParticleEngine.h"
#include <Godot.hpp>
#include <Reference.hpp>

namespace godot {
class PixelpartCollider : public Reference {
	GODOT_CLASS(PixelpartCollider, Reference)

public:
	static void _register_methods();

	PixelpartCollider();

	void _init();
	void init(Ref<PixelpartEffectResource> resource, pixelpart::Collider* collider, pixelpart::ParticleEngine* engine);

	String get_name() const;

	void set_lifetime_start(float time);
	void set_lifetime_duration(float time);
	void set_repeat(bool value);
	float get_lifetime_start() const;
	float get_lifetime_duration() const;
	bool get_repeat() const;
	bool is_active() const;
	float get_local_time() const;

	void add_point(Vector2 point);
	void set_point(int index, Vector2 point);
	void remove_point(int index);
	Vector2 get_point(int index) const;
	int get_num_points() const;

	Ref<PixelpartCurve> get_bounce() const;
	Ref<PixelpartCurve> get_friction() const;

private:
	Ref<PixelpartEffectResource> effectResource;
	pixelpart::Collider* nativeCollider = nullptr;
	pixelpart::ParticleEngine* nativeParticleEngine = nullptr;
};
}

#endif