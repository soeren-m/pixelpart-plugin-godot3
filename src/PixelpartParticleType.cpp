#include "PixelpartParticleType.h"
#include "PixelpartUtil.h"

namespace godot {
void PixelpartParticleType::_register_methods() {
	register_property<PixelpartParticleType, float>("lifespan_variance", &PixelpartParticleType::set_lifespan_variance, &PixelpartParticleType::get_lifespan_variance, 0.0f);
	register_property<PixelpartParticleType, bool>("position_relative", &PixelpartParticleType::set_position_relative, &PixelpartParticleType::is_position_relative, false);
	register_property<PixelpartParticleType, float>("motion_path_force", &PixelpartParticleType::set_motion_path_force, &PixelpartParticleType::get_motion_path_force, 0.0f);
	register_property<PixelpartParticleType, float>("velocity_variance", &PixelpartParticleType::set_velocity_variance, &PixelpartParticleType::get_velocity_variance, 0.0f);
	register_property<PixelpartParticleType, int>("rotation_mode", &PixelpartParticleType::set_rotation_mode, &PixelpartParticleType::get_rotation_mode, static_cast<int>(pixelpart::RotationMode::angle));
	register_property<PixelpartParticleType, int>("alignment_mode", &PixelpartParticleType::set_alignment_mode, &PixelpartParticleType::get_alignment_mode, static_cast<int>(pixelpart::AlignmentMode::none));
	register_property<PixelpartParticleType, Vector3>("rotation_variance", &PixelpartParticleType::set_rotation_variance, &PixelpartParticleType::get_rotation_variance, Vector3(0.0f, 0.0f, 0.0f));
	register_property<PixelpartParticleType, Vector3>("angular_velocity_variance", &PixelpartParticleType::set_angular_velocity_variance, &PixelpartParticleType::get_angular_velocity_variance, Vector3(0.0f, 0.0f, 0.0f));
	register_property<PixelpartParticleType, Vector3>("pivot", &PixelpartParticleType::set_pivot, &PixelpartParticleType::get_pivot, Vector3(0.0f, 0.0f, 0.0f));
	register_property<PixelpartParticleType, bool>("visible", &PixelpartParticleType::set_visible, &PixelpartParticleType::is_visible, true);
	register_property<PixelpartParticleType, int>("layer", &PixelpartParticleType::set_layer, &PixelpartParticleType::get_layer, 0);
	register_property<PixelpartParticleType, float>("size_variance", &PixelpartParticleType::set_size_variance, &PixelpartParticleType::get_size_variance, 0.0f);
	register_property<PixelpartParticleType, float>("color_hue_variance", &PixelpartParticleType::set_color_hue_variance, &PixelpartParticleType::get_color_hue_variance, 0.0f);
	register_property<PixelpartParticleType, float>("color_saturation_variance", &PixelpartParticleType::set_color_saturation_variance, &PixelpartParticleType::get_color_saturation_variance, 0.0f);
	register_property<PixelpartParticleType, float>("color_value_variance", &PixelpartParticleType::set_color_value_variance, &PixelpartParticleType::get_color_value_variance, 0.0f);
	register_property<PixelpartParticleType, float>("opacity_variance", &PixelpartParticleType::set_opacity_variance, &PixelpartParticleType::get_opacity_variance, 0.0f);
	register_method("_init", &PixelpartParticleType::_init);
	register_method("get_id", &PixelpartParticleType::get_id);
	register_method("get_parent_id", &PixelpartParticleType::get_parent_id);
	register_method("get_name", &PixelpartParticleType::get_name);
	register_method("get_position", &PixelpartParticleType::get_position);
	register_method("get_num_particles", &PixelpartParticleType::get_num_particles);
	register_method("get_lifespan", &PixelpartParticleType::get_lifespan);
	register_method("get_initial_velocity", &PixelpartParticleType::get_initial_velocity);
	register_method("get_acceleration", &PixelpartParticleType::get_acceleration);
	register_method("get_radial_acceleration", &PixelpartParticleType::get_radial_acceleration);
	register_method("get_damping", &PixelpartParticleType::get_damping);
	register_method("get_initial_rotation", &PixelpartParticleType::get_initial_rotation);
	register_method("get_rotation", &PixelpartParticleType::get_rotation);
	register_method("get_weight", &PixelpartParticleType::get_weight);
	register_method("get_bounce", &PixelpartParticleType::get_bounce);
	register_method("get_friction", &PixelpartParticleType::get_friction);
	register_method("get_blend_mode", &PixelpartParticleType::get_blend_mode);
	register_method("get_initial_size", &PixelpartParticleType::get_initial_size);
	register_method("get_size", &PixelpartParticleType::get_size);
	register_method("get_color", &PixelpartParticleType::get_color);
	register_method("get_initial_opacity", &PixelpartParticleType::get_initial_opacity);
	register_method("get_opacity", &PixelpartParticleType::get_opacity);
	register_method("spawn_particles", &PixelpartParticleType::spawn_particles);
}

PixelpartParticleType::PixelpartParticleType() {

}

void PixelpartParticleType::_init() {

}
void PixelpartParticleType::init(Ref<PixelpartEffectResource> resource, pixelpart::ParticleType* particleType, pixelpart::ParticleEngine* engine) {
	effectResource = resource;
	nativeParticleType = particleType;
	nativeParticleEngine = engine;
}

int PixelpartParticleType::get_id() const {
	if(nativeParticleType) {
		return static_cast<int>(nativeParticleType->id);
	}

	return -1;
}
int PixelpartParticleType::get_parent_id() const {
	if(nativeParticleType) {
		return static_cast<int>(nativeParticleType->parentId);
	}

	return -1;
}
String PixelpartParticleType::get_name() const {
	if(nativeParticleType) {
		return String(nativeParticleType->name.c_str());
	}

	return String();
}

Ref<PixelpartCurve3> PixelpartParticleType::get_position() const {
	if(nativeParticleType && nativeParticleEngine) {
		Ref<PixelpartCurve3> curve;
		curve.instance();
		curve->init(&nativeParticleType->position, nativeParticleEngine, PixelpartCurve3::ObjectType::particle_type);

		return curve;
	}

	return Ref<PixelpartCurve3>();
}

Ref<PixelpartCurve> PixelpartParticleType::get_num_particles() const {
	if(nativeParticleType && nativeParticleEngine) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeParticleType->numParticles, nativeParticleEngine, PixelpartCurve::ObjectType::particle_type);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
Ref<PixelpartCurve> PixelpartParticleType::get_lifespan() const {
	if(nativeParticleType && nativeParticleEngine) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeParticleType->lifespan, nativeParticleEngine, PixelpartCurve::ObjectType::particle_type);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
void PixelpartParticleType::set_lifespan_variance(float variance) {
	if(nativeParticleType) {
		nativeParticleType->lifespanVariance = static_cast<pixelpart::floatd>(variance);
	}
}
float PixelpartParticleType::get_lifespan_variance() const {
	if(nativeParticleType) {
		return static_cast<float>(nativeParticleType->lifespanVariance);
	}

	return 0.0f;
}

void PixelpartParticleType::set_position_relative(bool mode) {
	if(nativeParticleType) {
		nativeParticleType->positionRelative = mode;
	}
}
bool PixelpartParticleType::is_position_relative() const {
	if(nativeParticleType) {
		return nativeParticleType->positionRelative;
	}

	return false;
}

void PixelpartParticleType::set_motion_path_force(float force) {
	if(nativeParticleType) {
		nativeParticleType->motionPathForce = static_cast<pixelpart::floatd>(force);
	}
}
float PixelpartParticleType::get_motion_path_force() const {
	if(nativeParticleType) {
		return static_cast<float>(nativeParticleType->motionPathForce);
	}

	return 0.0f;
}

Ref<PixelpartCurve> PixelpartParticleType::get_initial_velocity() const {
	if(nativeParticleType && nativeParticleEngine) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeParticleType->initialVelocity, nativeParticleEngine, PixelpartCurve::ObjectType::particle_type);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
void PixelpartParticleType::set_velocity_variance(float variance) {
	if(nativeParticleType) {
		nativeParticleType->velocityVariance = static_cast<pixelpart::floatd>(variance);
	}
}
float PixelpartParticleType::get_velocity_variance() const {
	if(nativeParticleType) {
		return static_cast<float>(nativeParticleType->velocityVariance);
	}

	return 0.0f;
}

Ref<PixelpartCurve> PixelpartParticleType::get_acceleration() const {
	if(nativeParticleType && nativeParticleEngine) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeParticleType->acceleration, nativeParticleEngine, PixelpartCurve::ObjectType::particle_type);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
Ref<PixelpartCurve> PixelpartParticleType::get_radial_acceleration() const {
	if(nativeParticleType && nativeParticleEngine) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeParticleType->radialAcceleration, nativeParticleEngine, PixelpartCurve::ObjectType::particle_type);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
Ref<PixelpartCurve> PixelpartParticleType::get_damping() const {
	if(nativeParticleType && nativeParticleEngine) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeParticleType->damping, nativeParticleEngine, PixelpartCurve::ObjectType::particle_type);

		return curve;
	}

	return Ref<PixelpartCurve>();
}

void PixelpartParticleType::set_rotation_mode(int mode) {
	if(nativeParticleType) {
		nativeParticleType->rotationMode = static_cast<pixelpart::RotationMode>(mode);
	}
}
void PixelpartParticleType::set_alignment_mode(int mode) {
	if(nativeParticleType) {
		nativeParticleType->alignmentMode = static_cast<pixelpart::AlignmentMode>(mode);
	}
}
int PixelpartParticleType::get_rotation_mode() const {
	if(nativeParticleType) {
		return static_cast<int>(nativeParticleType->rotationMode);
	}

	return static_cast<int>(pixelpart::RotationMode::angle);
}
int PixelpartParticleType::get_alignment_mode() const {
	if(nativeParticleType) {
		return static_cast<int>(nativeParticleType->alignmentMode);
	}

	return static_cast<int>(pixelpart::AlignmentMode::none);
}

Ref<PixelpartCurve3> PixelpartParticleType::get_initial_rotation() const {
	if(nativeParticleType && nativeParticleEngine) {
		Ref<PixelpartCurve3> curve;
		curve.instance();
		curve->init(&nativeParticleType->initialRotation, nativeParticleEngine, PixelpartCurve3::ObjectType::particle_type);

		return curve;
	}

	return Ref<PixelpartCurve3>();
}
Ref<PixelpartCurve3> PixelpartParticleType::get_rotation() const {
	if(nativeParticleType && nativeParticleEngine) {
		Ref<PixelpartCurve3> curve;
		curve.instance();
		curve->init(&nativeParticleType->rotation, nativeParticleEngine, PixelpartCurve3::ObjectType::particle_type);

		return curve;
	}

	return Ref<PixelpartCurve3>();
}
void PixelpartParticleType::set_rotation_variance(Vector3 variance) {
	if(nativeParticleType) {
		nativeParticleType->rotationVariance = fromGd(variance);
	}
}
void PixelpartParticleType::set_angular_velocity_variance(Vector3 variance) {
	if(nativeParticleType) {
		nativeParticleType->angularVelocityVariance = fromGd(variance);
	}
}
Vector3 PixelpartParticleType::get_rotation_variance() const {
	if(nativeParticleType) {
		return toGd(nativeParticleType->rotationVariance);
	}

	return Vector3(0.0f, 0.0f, 0.0f);
}
Vector3 PixelpartParticleType::get_angular_velocity_variance() const {
	if(nativeParticleType) {
		return toGd(nativeParticleType->angularVelocityVariance);
	}

	return Vector3(0.0f, 0.0f, 0.0f);
}

void PixelpartParticleType::set_pivot(Vector3 pivot) {
	if(nativeParticleType) {
		nativeParticleType->pivot = fromGd(pivot);
	}
}
Vector3 PixelpartParticleType::get_pivot() const {
	if(nativeParticleType) {
		return toGd(nativeParticleType->pivot);
	}

	return Vector3(0.0f, 0.0f, 0.0f);
}

Ref<PixelpartCurve> PixelpartParticleType::get_weight() const {
	if(nativeParticleType && nativeParticleEngine) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeParticleType->weight, nativeParticleEngine, PixelpartCurve::ObjectType::particle_type);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
Ref<PixelpartCurve> PixelpartParticleType::get_bounce() const {
	if(nativeParticleType && nativeParticleEngine) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeParticleType->bounce, nativeParticleEngine, PixelpartCurve::ObjectType::particle_type);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
Ref<PixelpartCurve> PixelpartParticleType::get_friction() const {
	if(nativeParticleType && nativeParticleEngine) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeParticleType->friction, nativeParticleEngine, PixelpartCurve::ObjectType::particle_type);

		return curve;
	}

	return Ref<PixelpartCurve>();
}

void PixelpartParticleType::set_visible(bool mode) {
	if(nativeParticleType) {
		nativeParticleType->visible = mode;
	}
}
void PixelpartParticleType::set_layer(int layer) {
	if(nativeParticleType) {
		nativeParticleType->layer = static_cast<uint32_t>(std::max(layer, 0));
	}
}
bool PixelpartParticleType::is_visible() const {
	if(nativeParticleType) {
		return nativeParticleType->visible;
	}

	return false;
}
int PixelpartParticleType::get_layer() const {
	if(nativeParticleType) {
		return static_cast<int>(nativeParticleType->layer);
	}

	return 0;
}
int PixelpartParticleType::get_blend_mode() const {
	if(nativeParticleType) {
		return static_cast<int>(nativeParticleType->blendMode);
	}

	return static_cast<int>(pixelpart::BlendMode::normal);
}

Ref<PixelpartCurve> PixelpartParticleType::get_initial_size() const {
	if(nativeParticleType && nativeParticleEngine) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeParticleType->initialSize, nativeParticleEngine, PixelpartCurve::ObjectType::particle_type);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
Ref<PixelpartCurve3> PixelpartParticleType::get_size() const {
	if(nativeParticleType && nativeParticleEngine) {
		Ref<PixelpartCurve3> curve;
		curve.instance();
		curve->init(&nativeParticleType->size, nativeParticleEngine, PixelpartCurve3::ObjectType::particle_type);

		return curve;
	}

	return Ref<PixelpartCurve3>();
}
void PixelpartParticleType::set_size_variance(float variance) {
	if(nativeParticleType) {
		nativeParticleType->sizeVariance = static_cast<pixelpart::floatd>(variance);
	}
}
float PixelpartParticleType::get_size_variance() const {
	if(nativeParticleType) {
		return static_cast<float>(nativeParticleType->sizeVariance);
	}

	return 0.0f;
}

Ref<PixelpartGradient> PixelpartParticleType::get_color() const {
	if(nativeParticleType && nativeParticleEngine) {
		Ref<PixelpartGradient> curve;
		curve.instance();
		curve->init(&nativeParticleType->color, nativeParticleEngine, PixelpartGradient::ObjectType::particle_type);

		return curve;
	}

	return Ref<PixelpartGradient>();
}
void PixelpartParticleType::set_color_hue_variance(float variance) {
	if(nativeParticleType) {
		nativeParticleType->colorVariance.x = static_cast<pixelpart::floatd>(variance);
	}
}
void PixelpartParticleType::set_color_saturation_variance(float variance) {
	if(nativeParticleType) {
		nativeParticleType->colorVariance.y = static_cast<pixelpart::floatd>(variance);
	}
}
void PixelpartParticleType::set_color_value_variance(float variance) {
	if(nativeParticleType) {
		nativeParticleType->colorVariance.z = static_cast<pixelpart::floatd>(variance);
	}
}
float PixelpartParticleType::get_color_hue_variance() const {
	if(nativeParticleType) {
		return static_cast<float>(nativeParticleType->colorVariance.x);
	}

	return 0.0f;
}
float PixelpartParticleType::get_color_saturation_variance() const {
	if(nativeParticleType) {
		return static_cast<float>(nativeParticleType->colorVariance.y);
	}

	return 0.0f;
}
float PixelpartParticleType::get_color_value_variance() const {
	if(nativeParticleType) {
		return static_cast<float>(nativeParticleType->colorVariance.z);
	}

	return 0.0f;
}

Ref<PixelpartCurve> PixelpartParticleType::get_initial_opacity() const {
	if(nativeParticleType && nativeParticleEngine) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeParticleType->initialOpacity, nativeParticleEngine, PixelpartCurve::ObjectType::particle_type);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
Ref<PixelpartCurve> PixelpartParticleType::get_opacity() const {
	if(nativeParticleType && nativeParticleEngine) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeParticleType->opacity, nativeParticleEngine, PixelpartCurve::ObjectType::particle_type);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
void PixelpartParticleType::set_opacity_variance(float variance) {
	if(nativeParticleType) {
		nativeParticleType->opacityVariance = static_cast<pixelpart::floatd>(variance);
	}
}
float PixelpartParticleType::get_opacity_variance() const {
	if(nativeParticleType) {
		return static_cast<float>(nativeParticleType->opacityVariance);
	}

	return 0.0f;
}

void PixelpartParticleType::spawn_particles(int count) {
	if(nativeParticleType && nativeParticleEngine && count > 0) {
		nativeParticleEngine->spawnParticles(nativeParticleType->id, static_cast<uint32_t>(count));
	}
}
}