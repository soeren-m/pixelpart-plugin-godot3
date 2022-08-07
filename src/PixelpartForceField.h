#ifndef PIXELPART_FORCEFIELD_H
#define PIXELPART_FORCEFIELD_H

#include "PixelpartEffectResource.h"
#include "PixelpartCurve.h"
#include "PixelpartPath.h"
#include "ForceField.h"
#include "ParticleEngine.h"
#include <Godot.hpp>
#include <Reference.hpp>

namespace godot {
class PixelpartForceField : public Reference {
	GODOT_CLASS(PixelpartForceField, Reference)

public:
	static void _register_methods();

	PixelpartForceField();

	void _init();
	void init(Ref<PixelpartEffectResource> resource, pixelpart::ForceField* forceField, pixelpart::ParticleEngine* engine);

	String get_name() const;
	int get_id() const;

	void set_lifetime_start(float time);
	void set_lifetime_duration(float time);
	void set_repeat(bool value);
	float get_lifetime_start() const;
	float get_lifetime_duration() const;
	bool get_repeat() const;
	bool is_active() const;
	float get_local_time() const;

	void set_type(int type);
	int get_type() const;

	void set_direction_variance(float variance);
	void set_strength_variance(float variance);
	float get_direction_variance() const;
	float get_strength_variance() const;

	void set_grid_size(int width, int height);
	int get_grid_width() const;
	int get_grid_height() const;

	Ref<PixelpartCurve> get_width() const;
	Ref<PixelpartCurve> get_height() const;
	Ref<PixelpartCurve> get_orientation() const;
	Ref<PixelpartPath> get_motion_path() const;
	Ref<PixelpartCurve> get_direction() const;
	Ref<PixelpartCurve> get_strength() const;

private:
	Ref<PixelpartEffectResource> effectResource;
	pixelpart::ForceField* nativeForceField = nullptr;
	pixelpart::ParticleEngine* nativeParticleEngine = nullptr;
	std::mt19937 rng;
};
}

#endif