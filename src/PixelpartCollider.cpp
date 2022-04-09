#include "PixelpartCollider.h"
#include "PixelpartUtil.h"

namespace godot {
void PixelpartCollider::_register_methods() {
	register_property<PixelpartCollider, float>("lifetime_start", &PixelpartCollider::set_lifetime_start, &PixelpartCollider::get_lifetime_start, 0.0f);
	register_property<PixelpartCollider, float>("lifetime_duration", &PixelpartCollider::set_lifetime_duration, &PixelpartCollider::get_lifetime_duration, 0.0f);
	register_property<PixelpartCollider, bool>("repeat", &PixelpartCollider::set_repeat, &PixelpartCollider::get_repeat, false);
	register_method("_init", &PixelpartCollider::_init);
	register_method("get_name", &PixelpartCollider::get_name);
	register_method("is_active", &PixelpartCollider::is_active);
	register_method("get_local_time", &PixelpartCollider::get_local_time);
	register_method("add_point", &PixelpartCollider::add_point);
	register_method("set_point", &PixelpartCollider::set_point);
	register_method("remove_point", &PixelpartCollider::remove_point);
	register_method("get_point", &PixelpartCollider::get_point);
	register_method("get_num_points", &PixelpartCollider::get_num_points);
	register_method("get_bounce", &PixelpartCollider::get_bounce);
	register_method("get_friction", &PixelpartCollider::get_friction);
}

PixelpartCollider::PixelpartCollider() {

}

void PixelpartCollider::_init() {

}
void PixelpartCollider::init(Ref<PixelpartEffectResource> resource, pixelpart::Collider* collider, pixelpart::ParticleEngine* engine) {
	effectResource = resource;
	nativeCollider = collider;
	nativeParticleEngine = engine;
}

String PixelpartCollider::get_name() const {
	if(nativeCollider) {
		return String(nativeCollider->name.c_str());
	}

	return String();
}

void PixelpartCollider::set_lifetime_start(float time) {
	if(nativeCollider) {
		nativeCollider->lifetimeStart = time;
		nativeParticleEngine->onColliderUpdate();
	}
}
void PixelpartCollider::set_lifetime_duration(float time) {
	if(nativeCollider) {
		nativeCollider->lifetimeDuration = time;
		nativeParticleEngine->onColliderUpdate();
	}
}
void PixelpartCollider::set_repeat(bool value) {
	if(nativeCollider) {
		nativeCollider->repeat = value;
		nativeParticleEngine->onColliderUpdate();
	}
}
float PixelpartCollider::get_lifetime_start() const {
	if(nativeCollider) {
		return static_cast<float>(nativeCollider->lifetimeStart);
	}

	return 0.0f;
}
float PixelpartCollider::get_lifetime_duration() const {
	if(nativeCollider) {
		return static_cast<float>(nativeCollider->lifetimeDuration);
	}

	return 0.0f;
}
bool PixelpartCollider::get_repeat() const {
	if(nativeCollider) {
		return nativeCollider->repeat;
	}

	return false;
}
bool PixelpartCollider::is_active() const {
	if(nativeCollider) {
		return
			nativeParticleEngine->getTime() >= nativeCollider->lifetimeStart &&
			(nativeParticleEngine->getTime() <= nativeCollider->lifetimeStart + nativeCollider->lifetimeDuration || nativeCollider->repeat);
	}

	return false;
}
float PixelpartCollider::get_local_time() const {
	return static_cast<float>(std::fmod(
		nativeParticleEngine->getTime() - nativeCollider->lifetimeStart, nativeCollider->lifetimeDuration) / nativeCollider->lifetimeDuration);
}

void PixelpartCollider::add_point(Vector2 point) {
	if(nativeCollider) {
		nativeCollider->points.push_back(gd2pp(point / effectResource->get_scale()));
		nativeParticleEngine->onColliderUpdate();
	}
}
void PixelpartCollider::set_point(int index, Vector2 point) {
	if(nativeCollider) {
		if(index >= 0 && index < static_cast<int>(nativeCollider->points.size())) {
			nativeCollider->points[index] = gd2pp(point / effectResource->get_scale());
			nativeParticleEngine->onColliderUpdate();
		}
	}
}
void PixelpartCollider::remove_point(int index) {
	if(nativeCollider) {
		if(index >= 0 && index < static_cast<int>(nativeCollider->points.size())) {
			nativeCollider->points.erase(nativeCollider->points.begin() + index);
			nativeParticleEngine->onColliderUpdate();
		}
	}
}
Vector2 PixelpartCollider::get_point(int index) const {
	if(nativeCollider) {
		if(index >= 0 && index < static_cast<int>(nativeCollider->points.size())) {
			return pp2gd(nativeCollider->points[index]) * effectResource->get_scale();
		}
	}

	return Vector2(0.0f, 0.0f);
}
int PixelpartCollider::get_num_points() const {
	if(nativeCollider) {
		return static_cast<int>(nativeCollider->points.size());
	}

	return 0;
}

Ref<PixelpartCurve> PixelpartCollider::get_bounce() const {
	if(nativeCollider) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeCollider->bounce, nativeParticleEngine, PixelpartCurve::ObjectType::collider);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
Ref<PixelpartCurve> PixelpartCollider::get_friction() const {
	if(nativeCollider) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeCollider->friction, nativeParticleEngine, PixelpartCurve::ObjectType::collider);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
}