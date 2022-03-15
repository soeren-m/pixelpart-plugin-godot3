#ifndef PIXELPART_PATH_H
#define PIXELPART_PATH_H

#include "ParticleEngine.h"
#include <Godot.hpp>
#include <Reference.hpp>

namespace godot {
class PixelpartPath : public Reference {
	GODOT_CLASS(PixelpartPath, Reference)

public:
	enum class ObjectType {
		none,
		particle_emitter,
		force_field,
		collider,
		sprite
	};

	static void _register_methods();

	PixelpartPath();

	void _init();
	void init(pixelpart::Curve<pixelpart::vec2d>* path, pixelpart::ParticleEngine* engine, ObjectType type);

	Vector2 get(float t) const;
	Vector2 get_point(int index) const;

	void set(Vector2 value);
	void add_point(float t, Vector2 value);
	void set_point(int index, Vector2 value);
	void move_point(int index, Vector2 delta);
	void shift_point(int index, float delta);
	void remove_point(int index);
	void clear();
	int get_num_points() const;

	void move(Vector2 delta);
	void shift(float delta);

	void set_interpolation(int method);
	int get_interpolation() const;

	void enable_adaptive_cache();
	void enable_fixed_cache(int size);
	int get_cache_size() const;

private:
	void update_simulation();

	pixelpart::Curve<pixelpart::vec2d>* nativePath = nullptr;
	pixelpart::ParticleEngine* nativeParticleEngine = nullptr;
	ObjectType objectType = ObjectType::none;
};
}

#endif