#include "PixelpartParticleEmitter.h"
#include "PixelpartUtil.h"

namespace godot {
void PixelpartParticleEmitter::_register_methods() {
	register_property<PixelpartParticleEmitter, float>("lifetime_start", &PixelpartParticleEmitter::set_lifetime_start, &PixelpartParticleEmitter::get_lifetime_start, 0.0f);
	register_property<PixelpartParticleEmitter, float>("lifetime_duration", &PixelpartParticleEmitter::set_lifetime_duration, &PixelpartParticleEmitter::get_lifetime_duration, 0.0f);
	register_property<PixelpartParticleEmitter, bool>("repeat", &PixelpartParticleEmitter::set_repeat, &PixelpartParticleEmitter::get_repeat, false);
	register_property<PixelpartParticleEmitter, int>("shape", &PixelpartParticleEmitter::set_shape, &PixelpartParticleEmitter::get_shape, 0);
	register_property<PixelpartParticleEmitter, int>("distribution", &PixelpartParticleEmitter::set_distribution, &PixelpartParticleEmitter::get_distribution, 0);
	register_property<PixelpartParticleEmitter, int>("spawn_mode", &PixelpartParticleEmitter::set_spawn_mode, &PixelpartParticleEmitter::get_spawn_mode, 0);
	register_property<PixelpartParticleEmitter, bool>("burst", &PixelpartParticleEmitter::set_burst, &PixelpartParticleEmitter::get_burst, false);
	register_property<PixelpartParticleEmitter, float>("alpha_threshold", &PixelpartParticleEmitter::set_alpha_threshold, &PixelpartParticleEmitter::get_alpha_threshold, 0.0f);
	register_property<PixelpartParticleEmitter, int>("blend_mode", &PixelpartParticleEmitter::set_blend_mode, &PixelpartParticleEmitter::get_blend_mode, 0);
	register_property<PixelpartParticleEmitter, int>("color_mode", &PixelpartParticleEmitter::set_color_mode, &PixelpartParticleEmitter::get_color_mode, 0);
	register_property<PixelpartParticleEmitter, int>("layer", &PixelpartParticleEmitter::set_layer, &PixelpartParticleEmitter::get_layer, 0);
	register_property<PixelpartParticleEmitter, bool>("visible", &PixelpartParticleEmitter::set_visible, &PixelpartParticleEmitter::is_visible, true);
	register_property<PixelpartParticleEmitter, int>("particle_rotation_mode", &PixelpartParticleEmitter::set_particle_rotation_mode, &PixelpartParticleEmitter::get_particle_rotation_mode, 0);
	register_property<PixelpartParticleEmitter, float>("particle_lifespan_variance", &PixelpartParticleEmitter::set_particle_lifespan_variance, &PixelpartParticleEmitter::get_particle_lifespan_variance, 0.0f);
	register_property<PixelpartParticleEmitter, float>("particle_initial_velocity_variance", &PixelpartParticleEmitter::set_particle_initial_velocity_variance, &PixelpartParticleEmitter::get_particle_initial_velocity_variance, 0.0f);
	register_property<PixelpartParticleEmitter, float>("particle_rotation_variance", &PixelpartParticleEmitter::set_particle_rotation_variance, &PixelpartParticleEmitter::get_particle_rotation_variance, 0.0f);
	register_property<PixelpartParticleEmitter, float>("particle_angular_velocity_variance", &PixelpartParticleEmitter::set_particle_angular_velocity_variance, &PixelpartParticleEmitter::get_particle_angular_velocity_variance, 0.0f);
	register_property<PixelpartParticleEmitter, float>("particle_size_variance", &PixelpartParticleEmitter::set_particle_size_variance, &PixelpartParticleEmitter::get_particle_size_variance, 0.0f);
	register_property<PixelpartParticleEmitter, float>("particle_opacity_variance", &PixelpartParticleEmitter::set_particle_opacity_variance, &PixelpartParticleEmitter::get_particle_opacity_variance, 0.0f);
	register_method("_init", &PixelpartParticleEmitter::_init);
	register_method("get_name", &PixelpartParticleEmitter::get_name);
	register_method("get_id", &PixelpartParticleEmitter::get_id);
	register_method("get_parent_id", &PixelpartParticleEmitter::get_parent_id);
	register_method("is_active", &PixelpartParticleEmitter::is_active);
	register_method("get_local_time", &PixelpartParticleEmitter::get_local_time);
	register_method("get_width", &PixelpartParticleEmitter::get_width);
	register_method("get_height", &PixelpartParticleEmitter::get_height);
	register_method("get_orientation", &PixelpartParticleEmitter::get_orientation);
	register_method("get_direction", &PixelpartParticleEmitter::get_direction);
	register_method("get_spread", &PixelpartParticleEmitter::get_spread);
	register_method("get_num_particles", &PixelpartParticleEmitter::get_num_particles);
	register_method("get_particle_lifespan", &PixelpartParticleEmitter::get_particle_lifespan);
	register_method("get_motion_path", &PixelpartParticleEmitter::get_motion_path);
	register_method("get_particle_motion_path", &PixelpartParticleEmitter::get_particle_motion_path);
	register_method("get_particle_initial_velocity", &PixelpartParticleEmitter::get_particle_initial_velocity);
	register_method("get_particle_acceleration", &PixelpartParticleEmitter::get_particle_acceleration);
	register_method("get_particle_radial_acceleration", &PixelpartParticleEmitter::get_particle_radial_acceleration);
	register_method("get_particle_damping", &PixelpartParticleEmitter::get_particle_damping);
	register_method("get_particle_initial_rotation", &PixelpartParticleEmitter::get_particle_initial_rotation);
	register_method("get_particle_rotation", &PixelpartParticleEmitter::get_particle_rotation);
	register_method("get_particle_weight", &PixelpartParticleEmitter::get_particle_weight);
	register_method("get_particle_bounce", &PixelpartParticleEmitter::get_particle_bounce);
	register_method("get_particle_friction", &PixelpartParticleEmitter::get_particle_friction);
	register_method("get_particle_initial_size", &PixelpartParticleEmitter::get_particle_initial_size);
	register_method("get_particle_size", &PixelpartParticleEmitter::get_particle_size);
	register_method("get_particle_width", &PixelpartParticleEmitter::get_particle_width);
	register_method("get_particle_height", &PixelpartParticleEmitter::get_particle_height);	
	register_method("get_particle_color", &PixelpartParticleEmitter::get_particle_color);
	register_method("get_particle_initial_opacity", &PixelpartParticleEmitter::get_particle_initial_opacity);
	register_method("get_particle_opacity", &PixelpartParticleEmitter::get_particle_opacity);
	register_method("spawn_particles", &PixelpartParticleEmitter::spawn_particles);
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

String PixelpartParticleEmitter::get_name() const {
	if(nativeParticleEmitter) {
		return String(nativeParticleEmitter->name.c_str());
	}

	return String();
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
	if(nativeParticleEmitter) {
		return
			nativeParticleEngine->getTime() >= nativeParticleEmitter->lifetimeStart &&
			(nativeParticleEngine->getTime() <= nativeParticleEmitter->lifetimeStart + nativeParticleEmitter->lifetimeDuration || nativeParticleEmitter->repeat);
	}

	return false;
}
float PixelpartParticleEmitter::get_local_time() const {
	return static_cast<float>(std::fmod(
		nativeParticleEngine->getTime() - nativeParticleEmitter->lifetimeStart, nativeParticleEmitter->lifetimeDuration) / nativeParticleEmitter->lifetimeDuration);
}

void PixelpartParticleEmitter::set_shape(int type) {
	if(nativeParticleEmitter) {
		nativeParticleEmitter->shape = static_cast<pixelpart::ParticleEmitter::Shape>(type);
	}
}
void PixelpartParticleEmitter::set_distribution(int mode) {	
	if(nativeParticleEmitter) {
		nativeParticleEmitter->distribution = static_cast<pixelpart::ParticleEmitter::Distribution>(mode);
	}
}
void PixelpartParticleEmitter::set_spawn_mode(int mode) {	
	if(nativeParticleEmitter) {
		nativeParticleEmitter->spawnMode = static_cast<pixelpart::ParticleEmitter::SpawnMode>(mode);
	}
}
void PixelpartParticleEmitter::set_burst(bool burst) {
	if(nativeParticleEmitter) {
		nativeParticleEmitter->burst = burst;
	}
}
int PixelpartParticleEmitter::get_shape() const {	
	if(nativeParticleEmitter) {
		return static_cast<int>(nativeParticleEmitter->shape);
	}

	return static_cast<int>(pixelpart::ParticleEmitter::Shape::point);
}
int PixelpartParticleEmitter::get_distribution() const {	
	if(nativeParticleEmitter) {
		return static_cast<int>(nativeParticleEmitter->distribution);
	}

	return static_cast<int>(pixelpart::ParticleEmitter::Distribution::uniform);
}
int PixelpartParticleEmitter::get_spawn_mode() const {	
	if(nativeParticleEmitter) {
		return static_cast<int>(nativeParticleEmitter->spawnMode);
	}

	return static_cast<int>(pixelpart::ParticleEmitter::SpawnMode::fixed);
}
bool PixelpartParticleEmitter::get_burst() const {
	if(nativeParticleEmitter) {
		return nativeParticleEmitter->burst;
	}

	return false;
}

void PixelpartParticleEmitter::set_alpha_threshold(float threshold) {
	if(nativeParticleEmitter) {
		nativeParticleEmitter->alphaThreshold = threshold;
	}
}
void PixelpartParticleEmitter::set_blend_mode(int mode) {
	if(nativeParticleEmitter) {
		nativeParticleEmitter->blendMode = static_cast<pixelpart::BlendMode>(mode);
	}
}
void PixelpartParticleEmitter::set_color_mode(int mode) {	
	if(nativeParticleEmitter) {
		nativeParticleEmitter->colorMode = static_cast<pixelpart::ColorMode>(mode);
	}
}
void PixelpartParticleEmitter::set_layer(int layer) {
	if(nativeParticleEmitter) {
		nativeParticleEmitter->layer = static_cast<uint32_t>(std::max(layer, 0));
	}
}
void PixelpartParticleEmitter::set_visible(bool visible) {	
	if(nativeParticleEmitter) {
		nativeParticleEmitter->visible = visible;
	}
}
float PixelpartParticleEmitter::get_alpha_threshold() const {
	if(nativeParticleEmitter) {
		return static_cast<float>(nativeParticleEmitter->alphaThreshold);
	}

	return 0.0f;
}
int PixelpartParticleEmitter::get_blend_mode() const {
	if(nativeParticleEmitter) {
		return static_cast<int>(nativeParticleEmitter->blendMode);
	}

	return static_cast<int>(pixelpart::BlendMode::normal);
}
int PixelpartParticleEmitter::get_color_mode() const {
	if(nativeParticleEmitter) {
		return static_cast<float>(nativeParticleEmitter->colorMode);
	}

	return static_cast<int>(pixelpart::ColorMode::multiply);
}
int PixelpartParticleEmitter::get_layer() const {
	if(nativeParticleEmitter) {
		return static_cast<int>(nativeParticleEmitter->layer);
	}

	return 0;
}
bool PixelpartParticleEmitter::is_visible() const {
	if(nativeParticleEmitter) {
		return nativeParticleEmitter->visible;
	}

	return false;
}

void PixelpartParticleEmitter::set_particle_rotation_mode(int mode) {	
	if(nativeParticleEmitter) {
		nativeParticleEmitter->particleRotationMode = static_cast<pixelpart::ParticleEmitter::RotationMode>(mode);
	}
}
int PixelpartParticleEmitter::get_particle_rotation_mode() const {
	if(nativeParticleEmitter) {
		return static_cast<int>(nativeParticleEmitter->particleRotationMode);
	}

	return static_cast<int>(pixelpart::ParticleEmitter::RotationMode::angle);
}

void PixelpartParticleEmitter::set_particle_lifespan_variance(float variance) {	
	if(nativeParticleEmitter) {
		nativeParticleEmitter->particleLifespanVariance = static_cast<pixelpart::floatd>(variance);
	}
}
void PixelpartParticleEmitter::set_particle_initial_velocity_variance(float variance) {	
	if(nativeParticleEmitter) {
		nativeParticleEmitter->particleInitialVelocityVariance = static_cast<pixelpart::floatd>(variance);
	}
}
void PixelpartParticleEmitter::set_particle_rotation_variance(float variance) {
	if(nativeParticleEmitter) {
		nativeParticleEmitter->particleRotationVariance = static_cast<pixelpart::floatd>(variance);
	}
}
void PixelpartParticleEmitter::set_particle_angular_velocity_variance(float variance) {	
	if(nativeParticleEmitter) {
		nativeParticleEmitter->particleAngularVelocityVariance = static_cast<pixelpart::floatd>(variance);
	}
}
void PixelpartParticleEmitter::set_particle_size_variance(float variance) {	
	if(nativeParticleEmitter) {
		nativeParticleEmitter->particleSizeVariance = static_cast<pixelpart::floatd>(variance);
	}
}
void PixelpartParticleEmitter::set_particle_opacity_variance(float variance) {	
	if(nativeParticleEmitter) {
		nativeParticleEmitter->particleOpacityVariance = static_cast<pixelpart::floatd>(variance);
	}
}

float PixelpartParticleEmitter::get_particle_lifespan_variance() const {
	if(nativeParticleEmitter) {
		return static_cast<float>(nativeParticleEmitter->particleLifespanVariance);
	}

	return 0.0f;
}
float PixelpartParticleEmitter::get_particle_initial_velocity_variance() const {
	if(nativeParticleEmitter) {
		return static_cast<float>(nativeParticleEmitter->particleInitialVelocityVariance);
	}

	return 0.0f;
}
float PixelpartParticleEmitter::get_particle_rotation_variance() const {
	if(nativeParticleEmitter) {
		return static_cast<float>(nativeParticleEmitter->particleRotationVariance);
	}

	return 0.0f;
}
float PixelpartParticleEmitter::get_particle_angular_velocity_variance() const {
	if(nativeParticleEmitter) {
		return static_cast<float>(nativeParticleEmitter->particleAngularVelocityVariance);
	}

	return 0.0f;
}
float PixelpartParticleEmitter::get_particle_size_variance() const {
	if(nativeParticleEmitter) {
		return static_cast<float>(nativeParticleEmitter->particleSizeVariance);
	}

	return 0.0f;
}
float PixelpartParticleEmitter::get_particle_opacity_variance() const {
	if(nativeParticleEmitter) {
		return static_cast<float>(nativeParticleEmitter->particleOpacityVariance);
	}

	return 0.0f;
}

Ref<PixelpartCurve> PixelpartParticleEmitter::get_width() const {	
	if(nativeParticleEmitter) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeParticleEmitter->width, nativeParticleEngine, PixelpartCurve::ObjectType::particle_emitter);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
Ref<PixelpartCurve> PixelpartParticleEmitter::get_height() const {
	if(nativeParticleEmitter) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeParticleEmitter->height, nativeParticleEngine, PixelpartCurve::ObjectType::particle_emitter);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
Ref<PixelpartCurve> PixelpartParticleEmitter::get_orientation() const {
	if(nativeParticleEmitter) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeParticleEmitter->orientation, nativeParticleEngine, PixelpartCurve::ObjectType::particle_emitter);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
Ref<PixelpartCurve> PixelpartParticleEmitter::get_direction() const {
	if(nativeParticleEmitter) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeParticleEmitter->direction, nativeParticleEngine, PixelpartCurve::ObjectType::particle_emitter);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
Ref<PixelpartCurve> PixelpartParticleEmitter::get_spread() const {
	if(nativeParticleEmitter) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeParticleEmitter->spread, nativeParticleEngine, PixelpartCurve::ObjectType::particle_emitter);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
Ref<PixelpartCurve> PixelpartParticleEmitter::get_num_particles() const {
	if(nativeParticleEmitter) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeParticleEmitter->numParticles, nativeParticleEngine, PixelpartCurve::ObjectType::particle_emitter);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
Ref<PixelpartCurve> PixelpartParticleEmitter::get_particle_lifespan() const {
	if(nativeParticleEmitter) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeParticleEmitter->particleLifespan, nativeParticleEngine, PixelpartCurve::ObjectType::particle_emitter);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
Ref<PixelpartPath> PixelpartParticleEmitter::get_motion_path() const {	
	if(nativeParticleEmitter) {
		Ref<PixelpartPath> path;
		path.instance();
		path->init(&nativeParticleEmitter->motionPath, nativeParticleEngine, PixelpartPath::ObjectType::particle_emitter);

		return path;
	}

	return Ref<PixelpartPath>();
}
Ref<PixelpartPath> PixelpartParticleEmitter::get_particle_motion_path() const {
	if(nativeParticleEmitter) {
		Ref<PixelpartPath> path;
		path.instance();
		path->init(&nativeParticleEmitter->particleMotionPath, nativeParticleEngine, PixelpartPath::ObjectType::particle_emitter);

		return path;
	}

	return Ref<PixelpartPath>();
}
Ref<PixelpartCurve> PixelpartParticleEmitter::get_particle_initial_velocity() const {
	if(nativeParticleEmitter) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeParticleEmitter->particleInitialVelocity, nativeParticleEngine, PixelpartCurve::ObjectType::particle_emitter);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
Ref<PixelpartCurve> PixelpartParticleEmitter::get_particle_acceleration() const {
	if(nativeParticleEmitter) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeParticleEmitter->particleAcceleration, nativeParticleEngine, PixelpartCurve::ObjectType::particle_emitter);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
Ref<PixelpartCurve> PixelpartParticleEmitter::get_particle_radial_acceleration() const {
	if(nativeParticleEmitter) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeParticleEmitter->particleRadialAcceleration, nativeParticleEngine, PixelpartCurve::ObjectType::particle_emitter);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
Ref<PixelpartCurve> PixelpartParticleEmitter::get_particle_damping() const {
	if(nativeParticleEmitter) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeParticleEmitter->particleDamping, nativeParticleEngine, PixelpartCurve::ObjectType::particle_emitter);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
Ref<PixelpartCurve> PixelpartParticleEmitter::get_particle_initial_rotation() const {
	if(nativeParticleEmitter) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeParticleEmitter->particleInitialRotation, nativeParticleEngine, PixelpartCurve::ObjectType::particle_emitter);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
Ref<PixelpartCurve> PixelpartParticleEmitter::get_particle_rotation() const {
	if(nativeParticleEmitter) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeParticleEmitter->particleRotation, nativeParticleEngine, PixelpartCurve::ObjectType::particle_emitter);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
Ref<PixelpartCurve> PixelpartParticleEmitter::get_particle_weight() const {
	if(nativeParticleEmitter) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeParticleEmitter->particleWeight, nativeParticleEngine, PixelpartCurve::ObjectType::particle_emitter);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
Ref<PixelpartCurve> PixelpartParticleEmitter::get_particle_bounce() const {
	if(nativeParticleEmitter) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeParticleEmitter->particleBounce, nativeParticleEngine, PixelpartCurve::ObjectType::particle_emitter);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
Ref<PixelpartCurve> PixelpartParticleEmitter::get_particle_friction() const {
	if(nativeParticleEmitter) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeParticleEmitter->particleFriction, nativeParticleEngine, PixelpartCurve::ObjectType::particle_emitter);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
Ref<PixelpartCurve> PixelpartParticleEmitter::get_particle_initial_size() const {
	if(nativeParticleEmitter) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeParticleEmitter->particleInitialSize, nativeParticleEngine, PixelpartCurve::ObjectType::particle_emitter);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
Ref<PixelpartCurve> PixelpartParticleEmitter::get_particle_size() const {
	if(nativeParticleEmitter) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeParticleEmitter->particleSize, nativeParticleEngine, PixelpartCurve::ObjectType::particle_emitter);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
Ref<PixelpartCurve> PixelpartParticleEmitter::get_particle_width() const {
	if(nativeParticleEmitter) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeParticleEmitter->particleWidth, nativeParticleEngine, PixelpartCurve::ObjectType::particle_emitter);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
Ref<PixelpartCurve> PixelpartParticleEmitter::get_particle_height() const {
	if(nativeParticleEmitter) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeParticleEmitter->particleHeight, nativeParticleEngine, PixelpartCurve::ObjectType::particle_emitter);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
Ref<PixelpartGradient> PixelpartParticleEmitter::get_particle_color() const {
	if(nativeParticleEmitter) {
		Ref<PixelpartGradient> gradient;
		gradient.instance();
		gradient->init(&nativeParticleEmitter->particleColor, nativeParticleEngine, PixelpartGradient::ObjectType::particle_emitter);

		return gradient;
	}

	return Ref<PixelpartGradient>();
}
Ref<PixelpartCurve> PixelpartParticleEmitter::get_particle_initial_opacity() const {
	if(nativeParticleEmitter) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeParticleEmitter->particleInitialOpacity, nativeParticleEngine, PixelpartCurve::ObjectType::particle_emitter);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
Ref<PixelpartCurve> PixelpartParticleEmitter::get_particle_opacity() const {
	if(nativeParticleEmitter) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeParticleEmitter->particleOpacity, nativeParticleEngine, PixelpartCurve::ObjectType::particle_emitter);

		return curve;
	}

	return Ref<PixelpartCurve>();
}

void PixelpartParticleEmitter::spawn_particles(int count) {
	if(nativeParticleEmitter && count > 0) {
		nativeParticleEngine->spawnParticles(nativeParticleEmitter->id, static_cast<uint32_t>(count));
	}
}
}