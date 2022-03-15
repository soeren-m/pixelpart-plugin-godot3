#ifndef PIXELPART_PARTICLEEMITTER_H
#define PIXELPART_PARTICLEEMITTER_H

#include "PixelpartEffectResource.h"
#include "PixelpartCurve.h"
#include "PixelpartPath.h"
#include "PixelpartGradient.h"
#include "ParticleEmitter.h"
#include "ParticleEngine.h"
#include <Godot.hpp>
#include <Reference.hpp>

namespace godot {
class PixelpartEffectBase;

class PixelpartParticleEmitter : public Reference {
	GODOT_CLASS(PixelpartParticleEmitter, Reference)

public:
	static void _register_methods();

	PixelpartParticleEmitter();

	void _init();
	void init(Ref<PixelpartEffectResource> resource, pixelpart::ParticleEmitter* particleEmitter, pixelpart::ParticleEngine* engine);

	String get_name() const;
	int get_id() const;
	int get_parent_id() const;

	void set_lifetime_start(float time);
	void set_lifetime_duration(float time);
	void set_repeat(bool value);
	float get_lifetime_start() const;
	float get_lifetime_duration() const;
	bool get_repeat() const;
	bool is_active() const;
	float get_local_time() const;

	void set_shape(int type);
	void set_distribution(int mode);
	void set_spawn_mode(int mode);
	void set_burst(bool burst);
	int get_shape() const;
	int get_distribution() const;
	int get_spawn_mode() const;
	bool get_burst() const;

	void set_alpha_threshold(float threshold);
	void set_blend_mode(int mode);
	void set_color_mode(int mode);
	void set_layer(int layer);
	void set_visible(bool visible);
	float get_alpha_threshold() const;
	int get_blend_mode() const;
	int get_color_mode() const;
	int get_layer() const;
	bool is_visible() const;

	void set_particle_rotation_mode(int mode);
	int get_particle_rotation_mode() const;

	void set_particle_lifespan_variance(float variance);
	void set_particle_initial_velocity_variance(float variance);
	void set_particle_rotation_variance(float variance);
	void set_particle_angular_velocity_variance(float variance);
	void set_particle_size_variance(float variance);
	void set_particle_opacity_variance(float variance);
	float get_particle_lifespan_variance() const;
	float get_particle_initial_velocity_variance() const;
	float get_particle_rotation_variance() const;
	float get_particle_angular_velocity_variance() const;
	float get_particle_size_variance() const;
	float get_particle_opacity_variance() const;
	
	Ref<PixelpartCurve> get_width() const;
	Ref<PixelpartCurve> get_height() const;
	Ref<PixelpartCurve> get_orientation() const;
	Ref<PixelpartCurve> get_direction() const;
	Ref<PixelpartCurve> get_spread() const;
	Ref<PixelpartCurve> get_num_particles() const;
	Ref<PixelpartCurve> get_particle_lifespan() const;
	Ref<PixelpartPath> get_motion_path() const;
	Ref<PixelpartPath> get_particle_motion_path() const;
	Ref<PixelpartCurve> get_particle_initial_velocity() const;
	Ref<PixelpartCurve> get_particle_acceleration() const;
	Ref<PixelpartCurve> get_particle_radial_acceleration() const;
	Ref<PixelpartCurve> get_particle_damping() const;
	Ref<PixelpartCurve> get_particle_initial_rotation() const;
	Ref<PixelpartCurve> get_particle_rotation() const;
	Ref<PixelpartCurve> get_particle_weight() const;
	Ref<PixelpartCurve> get_particle_bounce() const;
	Ref<PixelpartCurve> get_particle_friction() const;
	Ref<PixelpartCurve> get_particle_initial_size() const;
	Ref<PixelpartCurve> get_particle_size() const;
	Ref<PixelpartCurve> get_particle_width() const;
	Ref<PixelpartCurve> get_particle_height() const;
	Ref<PixelpartGradient> get_particle_color() const;
	Ref<PixelpartCurve> get_particle_initial_opacity() const;
	Ref<PixelpartCurve> get_particle_opacity() const;

	void spawn_particles(int count);

private:
	Ref<PixelpartEffectResource> effectResource;
	pixelpart::ParticleEmitter* nativeParticleEmitter = nullptr;
	pixelpart::ParticleEngine* nativeParticleEngine = nullptr;
};
}

#endif