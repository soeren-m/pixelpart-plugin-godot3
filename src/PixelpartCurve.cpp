#include "PixelpartCurve.h"

namespace godot {
void PixelpartCurve::_register_methods() {
	register_property<PixelpartCurve, int>("interpolation", &PixelpartCurve::set_interpolation, &PixelpartCurve::get_interpolation, 0);
	register_method("_init", &PixelpartCurve::_init);
	register_method("get", &PixelpartCurve::get);
	register_method("get_point", &PixelpartCurve::get_point);
	register_method("set", &PixelpartCurve::set);
	register_method("add_point", &PixelpartCurve::add_point);
	register_method("set_point", &PixelpartCurve::set_point);
	register_method("move_point", &PixelpartCurve::move_point);
	register_method("shift_point", &PixelpartCurve::shift_point);
	register_method("remove_point", &PixelpartCurve::remove_point);
	register_method("clear", &PixelpartCurve::clear);
	register_method("get_num_points", &PixelpartCurve::get_num_points);
	register_method("move", &PixelpartCurve::move);
	register_method("shift", &PixelpartCurve::shift);
	register_method("enable_adaptive_cache", &PixelpartCurve::enable_adaptive_cache);
	register_method("enable_fixed_cache", &PixelpartCurve::enable_fixed_cache);
	register_method("get_cache_size", &PixelpartCurve::get_cache_size);
}

PixelpartCurve::PixelpartCurve() {

}

void PixelpartCurve::_init() {

}
void PixelpartCurve::init(pixelpart::Curve<pixelpart::floatd>* curve, pixelpart::ParticleEngine* engine, ObjectType type) {
	nativeCurve = curve;
	nativeParticleEngine = engine;
	objectType = type;
}

float PixelpartCurve::get(float t) const {
	if(nativeCurve) {
		return static_cast<float>(nativeCurve->get(t));
	}

	return 0.0f;
}
float PixelpartCurve::get_point(int index) const {
	if(nativeCurve && index >= 0 && static_cast<std::size_t>(index) < nativeCurve->getNumPoints()) {
		return static_cast<float>(nativeCurve->getPoints()[static_cast<std::size_t>(index)].value);
	}

	return 0.0f;
}

void PixelpartCurve::set(float value) {
	if(nativeCurve) {
		nativeCurve->clear();
		nativeCurve->setPoints({ pixelpart::Curve<pixelpart::floatd>::Point{ 0.5, value } });
		update_simulation();
	}
}
void PixelpartCurve::add_point(float t, float value) {
	if(nativeCurve) {
		nativeCurve->addPoint(t, value);
		update_simulation();
	}
}
void PixelpartCurve::set_point(int index, float value) {
	if(nativeCurve) {
		nativeCurve->setPoint(static_cast<std::size_t>(index), value);
		update_simulation();
	}
}
void PixelpartCurve::move_point(int index, float delta) {
	if(nativeCurve) {
		nativeCurve->movePoint(static_cast<std::size_t>(index), delta);
		update_simulation();
	}
}
void PixelpartCurve::shift_point(int index, float delta) {
	if(nativeCurve) {
		nativeCurve->shiftPoint(static_cast<std::size_t>(index), delta);
		update_simulation();
	}
}
void PixelpartCurve::remove_point(int index) {
	if(nativeCurve) {
		nativeCurve->removePoint(static_cast<std::size_t>(index));
		update_simulation();
	}
}
void PixelpartCurve::clear() {
	if(nativeCurve) {
		nativeCurve->clear();
		update_simulation();
	}
}
int PixelpartCurve::get_num_points() const {
	if(nativeCurve) {
		return static_cast<int>(nativeCurve->getNumPoints());
	}

	return 0;
}

void PixelpartCurve::move(float delta) {
	if(nativeCurve) {
		nativeCurve->move(delta);
		update_simulation();
	}
}
void PixelpartCurve::shift(float delta) {
	if(nativeCurve) {
		nativeCurve->shift(delta);
		update_simulation();
	}
}

void PixelpartCurve::set_interpolation(int method) {
	if(nativeCurve) {
		nativeCurve->setInterpolation(static_cast<pixelpart::CurveInterpolation>(method));
		update_simulation();
	}
}
int PixelpartCurve::get_interpolation() const {
	if(nativeCurve) {
		return static_cast<int>(nativeCurve->getInterpolation());
	}

	return static_cast<int>(pixelpart::CurveInterpolation::step);
}

void PixelpartCurve::enable_adaptive_cache() {
	if(nativeCurve) {
		nativeCurve->enableAdaptiveCache();
		update_simulation();
	}
}
void PixelpartCurve::enable_fixed_cache(int size) {
	if(nativeCurve && size > 0) {
		nativeCurve->enableFixedCache(static_cast<std::size_t>(size));
		update_simulation();
	}
}
int PixelpartCurve::get_cache_size() const {
	if(nativeCurve) {
		return nativeCurve->getCacheSize();
	}

	return 0;
}

void PixelpartCurve::update_simulation() {
	if(!nativeParticleEngine) {
		return;
	}

	if(objectType == ObjectType::force_field) {
		nativeParticleEngine->onForceFieldUpdate();
	}
	else if(objectType == ObjectType::collider) {
		nativeParticleEngine->onColliderUpdate();
	}
}
}