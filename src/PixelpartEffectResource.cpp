#include "PixelpartEffectResource.h"
#include <Engine.hpp>
#include <File.hpp>

namespace godot {
void PixelpartEffectResource::_register_methods() {
	register_property<PixelpartEffectResource, PoolByteArray>("data", &PixelpartEffectResource::set_data, &PixelpartEffectResource::get_data, { });
	register_property<PixelpartEffectResource, float>("scale", &PixelpartEffectResource::set_scale, &PixelpartEffectResource::get_scale, 100.0f,
		GODOT_METHOD_RPC_MODE_DISABLED,
		GODOT_PROPERTY_USAGE_DEFAULT,
		GODOT_PROPERTY_HINT_RANGE, "0.0,1000.0,1.0");
	register_method("_init", &PixelpartEffectResource::_init);
	register_method("import", &PixelpartEffectResource::import);
	register_method("load", &PixelpartEffectResource::load);
	register_method("release", &PixelpartEffectResource::release);
}

PixelpartEffectResource::PixelpartEffectResource() {

}
PixelpartEffectResource::~PixelpartEffectResource() {
	release();
}

void PixelpartEffectResource::_init() {

}

void PixelpartEffectResource::import(String filepath) {
	Ref<File> file = File::_new();
	if(file->open(filepath, File::READ) == Error::OK) {
		data = file->get_buffer(file->get_len());
	}
	else {
		Godot::print_error(String("Failed to open file: ") + filepath, __FUNCTION__, "", __LINE__);
	}

	file->close();
	loaded = false;
}
void PixelpartEffectResource::load() {
	if(loaded) {
		return;
	}

	try {
		project = pixelpart::deserialize(std::string((char*)data.read().ptr(), data.size()), projectResources);
	}
	catch(std::exception& e) {
		Godot::print_error(String("Failed to parse effect: ") + get_path(), __FUNCTION__, "", __LINE__);
	}

	loaded = true;
}
void PixelpartEffectResource::release() {
	project = pixelpart::Project();
	projectResources = pixelpart::ResourceDatabase();
}

void PixelpartEffectResource::set_data(PoolByteArray bytes) {
	data = bytes;
}
PoolByteArray PixelpartEffectResource::get_data() const {
	return data;
}

void PixelpartEffectResource::set_scale(float s) {
	scale = s;
}
float PixelpartEffectResource::get_scale() const {
	return scale;
}

const pixelpart::Project& PixelpartEffectResource::get_project() const {
	return project;
}
const pixelpart::ResourceDatabase& PixelpartEffectResource::get_project_resources() const {
	return projectResources;
}
}