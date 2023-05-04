#include "PixelpartParticleEmitter.h"
#include "PixelpartUtil.h"

namespace godot {
void PixelpartParticleEmitter::_register_methods() {
	register_property<PixelpartParticleEmitter, float>("lifetime_start", &PixelpartParticleEmitter::set_lifetime_start, &PixelpartParticleEmitter::get_lifetime_start, 0.0f);
	register_property<PixelpartParticleEmitter, float>("lifetime_duration", &PixelpartParticleEmitter::set_lifetime_duration, &PixelpartParticleEmitter::get_lifetime_duration, 0.0f);
	register_property<PixelpartParticleEmitter, bool>("repeat", &PixelpartParticleEmitter::set_repeat, &PixelpartParticleEmitter::get_repeat, false);
	register_property<PixelpartParticleEmitter, int>("shape", &PixelpartParticleEmitter::set_shape, &PixelpartParticleEmitter::get_shape, 0);
	register_property<PixelpartParticleEmitter, int>("distribution", &PixelpartParticleEmitter::set_distribution, &PixelpartParticleEmitter::get_distribution, 0);
	register_property<PixelpartParticleEmitter, int>("emission_mode", &PixelpartParticleEmitter::set_emission_mode, &PixelpartParticleEmitter::get_emission_mode, 0);
	register_property<PixelpartParticleEmitter, int>("direction_mode", &PixelpartParticleEmitter::set_direction_mode, &PixelpartParticleEmitter::get_direction_mode, 0);
	register_method("_init", &PixelpartParticleEmitter::_init);
	register_method("get_id", &PixelpartParticleEmitter::get_id);
	register_method("get_parent_id", &PixelpartParticleEmitter::get_parent_id);
	register_method("get_name", &PixelpartParticleEmitter::get_name);
	register_method("is_active", &PixelpartParticleEmitter::is_active);
	register_method("get_local_time", &PixelpartParticleEmitter::get_local_time);
	register_method("get_position", &PixelpartParticleEmitter::get_position);
	register_method("get_path", &PixelpartParticleEmitter::get_path);
	register_method("get_size", &PixelpartParticleEmitter::get_size);
	register_method("get_orientation", &PixelpartParticleEmitter::get_orientation);
	register_method("get_direction", &PixelpartParticleEmitter::get_direction);
	register_method("get_spread", &PixelpartParticleEmitter::get_spread);
}

PixelpartParticleEmitter::PixelpartParticleEmitter() {

}

void PixelpartParticleEmitter::_init() {

}
void PixelpartParticleEmitter::init(Ref<PixelpartEffectResource> resource, pixelpart::ParticleEmitter* particleEmitter, pixelpart::ParticleEngine* engine) {
	effectResource = resource;
	nativeParticleEmitter = particleEmitter;
	nativeParticleEngine = engine;
}

int PixelpartParticleEmitter::get_id() const {
	if(nativeParticleEmitter) {
		return static_cast<int>(nativeParticleEmitter->id);
	}

	return -1;
}
int PixelpartParticleEmitter::get_parent_id() const {
	if(nativeParticleEmitter) {
		return static_cast<int>(nativeParticleEmitter->parentId);
	}

	return -1;
}
String PixelpartParticleEmitter::get_name() const {
	if(nativeParticleEmitter) {
		return String(nativeParticleEmitter->name.c_str());
	}

	return String();
}

void PixelpartParticleEmitter::set_lifetime_start(float time) {
	if(nativeParticleEmitter) {
		nativeParticleEmitter->lifetimeStart = time;
	}
}
void PixelpartParticleEmitter::set_lifetime_duration(float time) {
	if(nativeParticleEmitter) {
		nativeParticleEmitter->lifetimeDuration = time;
	}
}
void PixelpartParticleEmitter::set_repeat(bool value) {
	if(nativeParticleEmitter) {
		nativeParticleEmitter->repeat = value;
	}
}
float PixelpartParticleEmitter::get_lifetime_start() const {
	if(nativeParticleEmitter) {
		return static_cast<float>(nativeParticleEmitter->lifetimeStart);
	}

	return 0.0f;
}
float PixelpartParticleEmitter::get_lifetime_duration() const {
	if(nativeParticleEmitter) {
		return static_cast<float>(nativeParticleEmitter->lifetimeDuration);
	}

	return 0.0f;
}
bool PixelpartParticleEmitter::get_repeat() const {
	if(nativeParticleEmitter) {
		return nativeParticleEmitter->repeat;
	}

	return false;
}
bool PixelpartParticleEmitter::is_active() const {
	if(nativeParticleEmitter && nativeParticleEngine) {
		return
			nativeParticleEngine->getTime() >= nativeParticleEmitter->lifetimeStart &&
			(nativeParticleEngine->getTime() <= nativeParticleEmitter->lifetimeStart + nativeParticleEmitter->lifetimeDuration || nativeParticleEmitter->repeat);
	}

	return false;
}
float PixelpartParticleEmitter::get_local_time() const {
	if(nativeParticleEngine) {
		return static_cast<float>(std::fmod(
			nativeParticleEngine->getTime() - nativeParticleEmitter->lifetimeStart, nativeParticleEmitter->lifetimeDuration) / nativeParticleEmitter->lifetimeDuration);
	}

	return 0.0f;
}

Ref<PixelpartCurve3> PixelpartParticleEmitter::get_position() const {
	if(nativeParticleEmitter && nativeParticleEngine) {
		Ref<PixelpartCurve3> path;
		path.instance();
		path->init(&nativeParticleEmitter->position, nativeParticleEngine, PixelpartCurve3::ObjectType::particle_emitter);

		return path;
	}

	return Ref<PixelpartCurve3>();
}

void PixelpartParticleEmitter::set_shape(int type) {
	if(nativeParticleEmitter) {
		nativeParticleEmitter->shape = static_cast<pixelpart::ParticleEmitter::Shape>(type);
	}
}
int PixelpartParticleEmitter::get_shape() const {
	if(nativeParticleEmitter) {
		return static_cast<int>(nativeParticleEmitter->shape);
	}

	return static_cast<int>(pixelpart::ParticleEmitter::Shape::point);
}

Ref<PixelpartCurve3> PixelpartParticleEmitter::get_path() const {
	if(nativeParticleEmitter && nativeParticleEngine) {
		Ref<PixelpartCurve3> curve;
		curve.instance();
		curve->init(&nativeParticleEmitter->path, nativeParticleEngine, PixelpartCurve3::ObjectType::particle_emitter);

		return curve;
	}

	return Ref<PixelpartCurve3>();
}
Ref<PixelpartCurve3> PixelpartParticleEmitter::get_size() const {
	if(nativeParticleEmitter && nativeParticleEngine) {
		Ref<PixelpartCurve3> curve;
		curve.instance();
		curve->init(&nativeParticleEmitter->size, nativeParticleEngine, PixelpartCurve3::ObjectType::particle_emitter);

		return curve;
	}

	return Ref<PixelpartCurve3>();
}
Ref<PixelpartCurve3> PixelpartParticleEmitter::get_orientation() const {
	if(nativeParticleEmitter && nativeParticleEngine) {
		Ref<PixelpartCurve3> curve;
		curve.instance();
		curve->init(&nativeParticleEmitter->orientation, nativeParticleEngine, PixelpartCurve3::ObjectType::particle_emitter);

		return curve;
	}

	return Ref<PixelpartCurve3>();
}

void PixelpartParticleEmitter::set_distribution(int mode) {
	if(nativeParticleEmitter) {
		nativeParticleEmitter->distribution = static_cast<pixelpart::ParticleEmitter::Distribution>(mode);
	}
}
void PixelpartParticleEmitter::set_emission_mode(int mode) {
	if(nativeParticleEmitter) {
		nativeParticleEmitter->emissionMode = static_cast<pixelpart::ParticleEmitter::EmissionMode>(mode);
	}
}
void PixelpartParticleEmitter::set_direction_mode(int mode) {
	if(nativeParticleEmitter) {
		nativeParticleEmitter->directionMode = static_cast<pixelpart::ParticleEmitter::DirectionMode>(mode);
	}
}
int PixelpartParticleEmitter::get_distribution() const {
	if(nativeParticleEmitter) {
		return static_cast<int>(nativeParticleEmitter->distribution);
	}

	return static_cast<int>(pixelpart::ParticleEmitter::Distribution::uniform);
}
int PixelpartParticleEmitter::get_emission_mode() const {
	if(nativeParticleEmitter) {
		return static_cast<int>(nativeParticleEmitter->emissionMode);
	}

	return static_cast<int>(pixelpart::ParticleEmitter::EmissionMode::continuous);
}
int PixelpartParticleEmitter::get_direction_mode() const {
	if(nativeParticleEmitter) {
		return static_cast<int>(nativeParticleEmitter->directionMode);
	}

	return static_cast<int>(pixelpart::ParticleEmitter::DirectionMode::fixed);
}

Ref<PixelpartCurve3> PixelpartParticleEmitter::get_direction() const {
	if(nativeParticleEmitter && nativeParticleEngine) {
		Ref<PixelpartCurve3> curve;
		curve.instance();
		curve->init(&nativeParticleEmitter->direction, nativeParticleEngine, PixelpartCurve3::ObjectType::particle_emitter);

		return curve;
	}

	return Ref<PixelpartCurve3>();
}
Ref<PixelpartCurve> PixelpartParticleEmitter::get_spread() const {
	if(nativeParticleEmitter && nativeParticleEngine) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeParticleEmitter->spread, nativeParticleEngine, PixelpartCurve::ObjectType::particle_emitter);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
}