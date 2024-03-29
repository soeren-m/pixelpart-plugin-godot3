#include "PixelpartForceField.h"
#include "PixelpartUtil.h"

namespace godot {
void PixelpartForceField::_register_methods() {
	register_property<PixelpartForceField, float>("lifetime_start", &PixelpartForceField::set_lifetime_start, &PixelpartForceField::get_lifetime_start, 0.0f);
	register_property<PixelpartForceField, float>("lifetime_duration", &PixelpartForceField::set_lifetime_duration, &PixelpartForceField::get_lifetime_duration, 0.0f);
	register_property<PixelpartForceField, bool>("repeat", &PixelpartForceField::set_repeat, &PixelpartForceField::get_repeat, false);
	register_property<PixelpartForceField, int>("type", &PixelpartForceField::set_type, &PixelpartForceField::get_type, 0);
	register_property<PixelpartForceField, float>("direction_variance", &PixelpartForceField::set_direction_variance, &PixelpartForceField::get_direction_variance, 0.0f);
	register_property<PixelpartForceField, float>("strength_variance", &PixelpartForceField::set_strength_variance, &PixelpartForceField::get_strength_variance, 0.0f);
	register_method("_init", &PixelpartForceField::_init);
	register_method("get_id", &PixelpartForceField::get_id);
	register_method("get_name", &PixelpartForceField::get_name);
	register_method("is_active", &PixelpartForceField::is_active);
	register_method("get_local_time", &PixelpartForceField::get_local_time);
	register_method("get_position", &PixelpartForceField::get_position);
	register_method("get_size", &PixelpartForceField::get_size);
	register_method("get_orientation", &PixelpartForceField::get_orientation);
	register_method("get_direction", &PixelpartForceField::get_direction);
	register_method("get_strength", &PixelpartForceField::get_strength);
	register_method("set_grid_size", &PixelpartForceField::set_grid_size);
	register_method("get_grid_width", &PixelpartForceField::get_grid_width);
	register_method("get_grid_height", &PixelpartForceField::get_grid_height);
	register_method("get_grid_depth", &PixelpartForceField::get_grid_depth);
}

PixelpartForceField::PixelpartForceField() {

}

void PixelpartForceField::_init() {

}
void PixelpartForceField::init(Ref<PixelpartEffectResource> resource, pixelpart::ForceField* forceField, pixelpart::ParticleEngine* engine) {
	effectResource = resource;
	nativeForceField = forceField;
	nativeParticleEngine = engine;
}

int PixelpartForceField::get_id() const {
	if(nativeForceField) {
		return static_cast<int>(nativeForceField->id);
	}

	return -1;
}
String PixelpartForceField::get_name() const {
	if(nativeForceField) {
		return String(nativeForceField->name.c_str());
	}

	return String();
}

void PixelpartForceField::set_lifetime_start(float time) {
	if(nativeForceField && nativeParticleEngine) {
		nativeForceField->lifetimeStart = time;
		nativeParticleEngine->updateForceSolver();
	}
}
void PixelpartForceField::set_lifetime_duration(float time) {
	if(nativeForceField && nativeParticleEngine) {
		nativeForceField->lifetimeDuration = time;
		nativeParticleEngine->updateForceSolver();
	}
}
void PixelpartForceField::set_repeat(bool value) {
	if(nativeForceField && nativeParticleEngine) {
		nativeForceField->repeat = value;
		nativeParticleEngine->updateForceSolver();
	}
}
float PixelpartForceField::get_lifetime_start() const {
	if(nativeForceField) {
		return static_cast<float>(nativeForceField->lifetimeStart);
	}

	return 0.0f;
}
float PixelpartForceField::get_lifetime_duration() const {
	if(nativeForceField) {
		return static_cast<float>(nativeForceField->lifetimeDuration);
	}

	return 0.0f;
}
bool PixelpartForceField::get_repeat() const {
	if(nativeForceField) {
		return nativeForceField->repeat;
	}

	return false;
}
bool PixelpartForceField::is_active() const {
	if(nativeForceField && nativeParticleEngine) {
		return
			nativeParticleEngine->getTime() >= nativeForceField->lifetimeStart &&
			(nativeParticleEngine->getTime() <= nativeForceField->lifetimeStart + nativeForceField->lifetimeDuration || nativeForceField->repeat);
	}

	return false;
}
float PixelpartForceField::get_local_time() const {
	if(nativeParticleEngine) {
		return static_cast<float>(std::fmod(
			nativeParticleEngine->getTime() - nativeForceField->lifetimeStart, nativeForceField->lifetimeDuration) / nativeForceField->lifetimeDuration);
	}

	return 0.0f;
}

Ref<PixelpartCurve3> PixelpartForceField::get_position() const {
	if(nativeForceField && nativeParticleEngine) {
		Ref<PixelpartCurve3> curve;
		curve.instance();
		curve->init(&nativeForceField->position, nativeParticleEngine, PixelpartCurve3::ObjectType::force_field);

		return curve;
	}

	return Ref<PixelpartCurve3>();
}

void PixelpartForceField::set_type(int type) {
	if(nativeForceField && nativeParticleEngine) {
		nativeForceField->type = static_cast<pixelpart::ForceField::Type>(type);
		nativeParticleEngine->updateForceSolver();
	}
}
int PixelpartForceField::get_type() const {
	if(nativeForceField) {
		return static_cast<int>(nativeForceField->type);
	}

	return 0;
}

Ref<PixelpartCurve3> PixelpartForceField::get_size() const {
	if(nativeForceField && nativeParticleEngine) {
		Ref<PixelpartCurve3> curve;
		curve.instance();
		curve->init(&nativeForceField->size, nativeParticleEngine, PixelpartCurve3::ObjectType::force_field);

		return curve;
	}

	return Ref<PixelpartCurve3>();
}
Ref<PixelpartCurve3> PixelpartForceField::get_orientation() const {
	if(nativeForceField && nativeParticleEngine) {
		Ref<PixelpartCurve3> curve;
		curve.instance();
		curve->init(&nativeForceField->orientation, nativeParticleEngine, PixelpartCurve3::ObjectType::force_field);

		return curve;
	}

	return Ref<PixelpartCurve3>();
}
Ref<PixelpartCurve3> PixelpartForceField::get_direction() const {
	if(nativeForceField && nativeParticleEngine) {
		Ref<PixelpartCurve3> curve;
		curve.instance();
		curve->init(&nativeForceField->direction, nativeParticleEngine, PixelpartCurve3::ObjectType::force_field);

		return curve;
	}

	return Ref<PixelpartCurve3>();
}
Ref<PixelpartCurve> PixelpartForceField::get_strength() const {
	if(nativeForceField && nativeParticleEngine) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeForceField->strength, nativeParticleEngine, PixelpartCurve::ObjectType::force_field);

		return curve;
	}

	return Ref<PixelpartCurve>();
}

void PixelpartForceField::set_direction_variance(float variance) {
	if(nativeForceField && nativeParticleEngine) {
		nativeForceField->directionVariance = static_cast<pixelpart::floatd>(variance);
		nativeParticleEngine->updateForceSolver();
	}
}
void PixelpartForceField::set_strength_variance(float variance) {
	if(nativeForceField && nativeParticleEngine) {
		nativeForceField->strengthVariance = static_cast<pixelpart::floatd>(variance);
		nativeParticleEngine->updateForceSolver();
	}
}
float PixelpartForceField::get_direction_variance() const {
	if(nativeForceField) {
		return static_cast<float>(nativeForceField->directionVariance);
	}

	return 0.0f;
}
float PixelpartForceField::get_strength_variance() const {
	if(nativeForceField) {
		return static_cast<float>(nativeForceField->strengthVariance);
	}

	return 0.0f;
}

void PixelpartForceField::set_grid_size(int width, int height, int depth) {
	if(nativeForceField && nativeParticleEngine) {
		nativeForceField->gridSize[0] = static_cast<uint32_t>(std::max(width, 1));
		nativeForceField->gridSize[1] = static_cast<uint32_t>(std::max(height, 1));
		nativeForceField->gridSize[2] = static_cast<uint32_t>(std::max(depth, 1));
		nativeForceField->randomize(rng);
		nativeParticleEngine->updateForceSolver();
	}
}
int PixelpartForceField::get_grid_width() const {
	if(nativeForceField) {
		return static_cast<int>(nativeForceField->gridSize[0]);
	}

	return 1;
}
int PixelpartForceField::get_grid_height() const {
	if(nativeForceField) {
		return static_cast<int>(nativeForceField->gridSize[1]);
	}

	return 1;
}
int PixelpartForceField::get_grid_depth() const {
	if(nativeForceField) {
		return static_cast<int>(nativeForceField->gridSize[2]);
	}

	return 1;
}
}