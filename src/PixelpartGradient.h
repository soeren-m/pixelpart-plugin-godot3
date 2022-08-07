#ifndef PIXELPART_GRADIENT_H
#define PIXELPART_GRADIENT_H

#include "ParticleEngine.h"
#include <Godot.hpp>
#include <Reference.hpp>

namespace godot {
class PixelpartGradient : public Reference {
	GODOT_CLASS(PixelpartGradient, Reference)

public:
	enum class ObjectType {
		none,
		particle_emitter,
		sprite,
		force_field,
		collider
	};

	static void _register_methods();

	PixelpartGradient();

	void _init();
	void init(pixelpart::Curve<pixelpart::vec4d>* gradient, pixelpart::ParticleEngine* engine, ObjectType type);

	Color get(float t) const;
	Color get_point(int index) const;

	void set(Color value);
	void add_point(float t, Color value);
	void set_point(int index, Color value);
	void move_point(int index, Color delta);
	void shift_point(int index, float delta);
	void remove_point(int index);
	void clear();
	int get_num_points() const;

	void move(Color delta);
	void shift(float delta);

	void set_interpolation(int method);
	int get_interpolation() const;

	void enable_adaptive_cache();
	void enable_fixed_cache(int size);
	int get_cache_size() const;

private:
	void update_simulation();

	pixelpart::Curve<pixelpart::vec4d>* nativeGradient = nullptr;
	pixelpart::ParticleEngine* nativeParticleEngine = nullptr;
	ObjectType objectType = ObjectType::none;
};
}

#endif