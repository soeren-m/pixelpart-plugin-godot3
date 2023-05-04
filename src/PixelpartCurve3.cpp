#include "PixelpartCurve3.h"
#include "PixelpartUtil.h"

namespace godot {
void PixelpartCurve3::_register_methods() {
	register_property<PixelpartCurve3, int>("interpolation", &PixelpartCurve3::set_interpolation, &PixelpartCurve3::get_interpolation, 0);
	register_method("_init", &PixelpartCurve3::_init);
	register_method("get", &PixelpartCurve3::get);
	register_method("get_point", &PixelpartCurve3::get_point);
	register_method("set", &PixelpartCurve3::set);
	register_method("add_point", &PixelpartCurve3::add_point);
	register_method("set_point", &PixelpartCurve3::set_point);
	register_method("move_point", &PixelpartCurve3::move_point);
	register_method("shift_point", &PixelpartCurve3::shift_point);
	register_method("remove_point", &PixelpartCurve3::remove_point);
	register_method("clear", &PixelpartCurve3::clear);
	register_method("get_num_points", &PixelpartCurve3::get_num_points);
	register_method("move", &PixelpartCurve3::move);
	register_method("shift", &PixelpartCurve3::shift);
	register_method("enable_adaptive_cache", &PixelpartCurve3::enable_adaptive_cache);
	register_method("enable_fixed_cache", &PixelpartCurve3::enable_fixed_cache);
	register_method("get_cache_size", &PixelpartCurve3::get_cache_size);
}

PixelpartCurve3::PixelpartCurve3() {

}

void PixelpartCurve3::_init() {

}
void PixelpartCurve3::init(pixelpart::Curve<pixelpart::vec3d>* curve, pixelpart::ParticleEngine* engine, ObjectType type) {
	nativeCurve = curve;
	nativeParticleEngine = engine;
	objectType = type;
}

Vector3 PixelpartCurve3::get(float t) const {
	if(nativeCurve) {
		return toGd(nativeCurve->get(t));
	}

	return Vector3(0.0f, 0.0f, 0.0f);
}
Vector3 PixelpartCurve3::get_point(int index) const {
	if(nativeCurve) {
		return toGd(nativeCurve->getPoint(static_cast<std::size_t>(index)).value);
	}

	return Vector3(0.0f, 0.0f, 0.0f);
}

void PixelpartCurve3::set(Vector3 value) {
	if(nativeCurve) {
		nativeCurve->clear();
		nativeCurve->setPoints({ pixelpart::Curve<pixelpart::vec3d>::Point{ 0.5, fromGd(value) } });
		update_simulation();
	}
}
void PixelpartCurve3::add_point(float t, Vector3 value) {
	if(nativeCurve) {
		nativeCurve->addPoint(t, fromGd(value));
		update_simulation();
	}
}
void PixelpartCurve3::set_point(int index, Vector3 value) {
	if(nativeCurve) {
		nativeCurve->setPoint(static_cast<std::size_t>(index), fromGd(value));
		update_simulation();
	}
}
void PixelpartCurve3::move_point(int index, Vector3 delta) {
	if(nativeCurve) {
		nativeCurve->movePoint(static_cast<std::size_t>(index), fromGd(delta));
		update_simulation();
	}
}
void PixelpartCurve3::shift_point(int index, float delta) {
	if(nativeCurve) {
		nativeCurve->shiftPoint(static_cast<std::size_t>(index), delta);
		update_simulation();
	}
}
void PixelpartCurve3::remove_point(int index) {
	if(nativeCurve) {
		nativeCurve->removePoint(static_cast<std::size_t>(index));
		update_simulation();
	}
}
void PixelpartCurve3::clear() {
	if(nativeCurve) {
		nativeCurve->clear();
		update_simulation();
	}
}
int PixelpartCurve3::get_num_points() const {
	if(nativeCurve) {
		return static_cast<int>(nativeCurve->getNumPoints());
	}

	return 0;
}

void PixelpartCurve3::move(Vector3 delta) {
	if(nativeCurve) {
		nativeCurve->move(fromGd(delta));
		update_simulation();
	}
}
void PixelpartCurve3::shift(float delta) {
	if(nativeCurve) {
		nativeCurve->shift(delta);
		update_simulation();
	}
}

void PixelpartCurve3::set_interpolation(int method) {
	if(nativeCurve) {
		nativeCurve->setInterpolation(static_cast<pixelpart::CurveInterpolation>(method));
		update_simulation();
	}
}
int PixelpartCurve3::get_interpolation() const {
	if(nativeCurve) {
		return static_cast<int>(nativeCurve->getInterpolation());
	}

	return static_cast<int>(pixelpart::CurveInterpolation::none);
}

void PixelpartCurve3::enable_adaptive_cache() {
	if(nativeCurve) {
		nativeCurve->enableAdaptiveCache();
		update_simulation();
	}
}
void PixelpartCurve3::enable_fixed_cache(int size) {
	if(nativeCurve && size > 0) {
		nativeCurve->enableFixedCache(static_cast<std::size_t>(size));
		update_simulation();
	}
}
int PixelpartCurve3::get_cache_size() const {
	if(nativeCurve) {
		return nativeCurve->getCacheSize();
	}

	return 0;
}

void PixelpartCurve3::update_simulation() {
	if(!nativeParticleEngine) {
		return;
	}

	if(objectType == ObjectType::force_field) {
		nativeParticleEngine->updateForceSolver();
	}
	else if(objectType == ObjectType::collider) {
		nativeParticleEngine->updateCollisionSolver();
	}
}
}