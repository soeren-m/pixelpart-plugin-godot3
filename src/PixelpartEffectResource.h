#ifndef PIXELPART_EFFECTRESOURCE_H
#define PIXELPART_EFFECTRESOURCE_H

#include "Project.h"
#include <Godot.hpp>
#include <Resource.hpp>

namespace godot {
class PixelpartEffectResource : public Resource {
	GODOT_CLASS(PixelpartEffectResource, Resource)

public:
	static void _register_methods();

	PixelpartEffectResource();
	~PixelpartEffectResource();

	void _init();

	void import(String filepath);
	void load();
	void release();

	void set_data(PoolByteArray bytes);
	PoolByteArray get_data() const;

	void set_scale(float s);
	float get_scale() const;

	const pixelpart::Project& get_project() const;
	const pixelpart::ResourceDatabase& get_project_resources() const;

private:
	pixelpart::Project project;
	pixelpart::ResourceDatabase projectResources;
	bool loaded = false;

	PoolByteArray data;
	float scale = 100.0f;
};
}

#endif