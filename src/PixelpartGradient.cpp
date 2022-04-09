#include "PixelpartGradient.h"
#include "PixelpartUtil.h"

namespace godot {
void PixelpartGradient::_register_methods() {
	register_property<PixelpartGradient, int>("interpolation", &PixelpartGradient::set_interpolation, &PixelpartGradient::get_interpolation, 0);
	register_method("_init", &PixelpartGradient::_init);
	register_method("get", &PixelpartGradient::get);
	register_method("get_point", &PixelpartGradient::get_point);
	register_method("set", &PixelpartGradient::set);
	register_method("add_point", &PixelpartGradient::add_point);
	register_method("set_point", &PixelpartGradient::set_point);
	register_method("move_point", &PixelpartGradient::move_point);
	register_method("shift_point", &PixelpartGradient::shift_point);
	register_method("remove_point", &PixelpartGradient::remove_point);
	register_method("clear", &PixelpartGradient::clear);
	register_method("get_num_points", &PixelpartGradient::get_num_points);
	register_method("move", &PixelpartGradient::move);
	register_method("shift", &PixelpartGradient::shift);
	register_method("enable_adaptive_cache", &PixelpartGradient::enable_adaptive_cache);
	register_method("enable_fixed_cache", &PixelpartGradient::enable_fixed_cache);
	register_method("get_cache_size", &PixelpartGradient::get_cache_size);
}

PixelpartGradient::PixelpartGradient() {

}

void PixelpartGradient::_init() {

}
void PixelpartGradient::init(pixelpart::Curve<pixelpart::vec4d>* gradient, pixelpart::ParticleEngine* engine, ObjectType type) {
	nativeGradient = gradient;
	nativeParticleEngine = engine;
	objectType = type;
}

Color PixelpartGradient::get(float t) const {
	if(nativeGradient) {
		return pp2gd(nativeGradient->get(t));
	}

	return Color(0.0f, 0.0f, 0.0f, 0.0f);
}
Color PixelpartGradient::get_point(int index) const {
	if(nativeGradient && index >= 0 && static_cast<std::size_t>(index) < nativeGradient->getNumPoints()) {
		return pp2gd(nativeGradient->getPoints()[static_cast<std::size_t>(index)].value);
	}

	return Color(0.0f, 0.0f, 0.0f, 0.0f);
}

void PixelpartGradient::set(Color value) {
	if(nativeGradient) {
		nativeGradient->clear();
		nativeGradient->setPoints({ pixelpart::Curve<pixelpart::vec4d>::Point{ 0.5, gd2pp(value) } });
		update_simulation();
	}
}
void PixelpartGradient::add_point(float t, Color value) {
	if(nativeGradient) {
		nativeGradient->addPoint(t, gd2pp(value));
		update_simulation();
	}
}
void PixelpartGradient::set_point(int index, Color value) {
	if(nativeGradient) {
		nativeGradient->setPoint(static_cast<std::size_t>(index), gd2pp(value));
		update_simulation();
	}
}
void PixelpartGradient::move_point(int index, Color delta) {
	if(nativeGradient) {
		nativeGradient->movePoint(static_cast<std::size_t>(index), gd2pp(delta));
		update_simulation();
	}
}
void PixelpartGradient::shift_point(int index, float delta) {
	if(nativeGradient) {
		nativeGradient->shiftPoint(static_cast<std::size_t>(index), delta);
		update_simulation();
	}
}
void PixelpartGradient::remove_point(int index) {
	if(nativeGradient) {
		nativeGradient->removePoint(static_cast<std::size_t>(index));
		update_simulation();
	}
}
void PixelpartGradient::clear() {
	if(nativeGradient) {
		nativeGradient->clear();
		update_simulation();
	}
}
int PixelpartGradient::get_num_points() const {
	if(nativeGradient) {
		return static_cast<int>(nativeGradient->getNumPoints());
	}

	return 0;
}

void PixelpartGradient::move(Color delta) {
	if(nativeGradient) {
		nativeGradient->move(gd2pp(delta));
		update_simulation();
	}
}
void PixelpartGradient::shift(float delta) {
	if(nativeGradient) {
		nativeGradient->shift(delta);
		update_simulation();
	}
}

void PixelpartGradient::set_interpolation(int method) {
	if(nativeGradient) {
		nativeGradient->setInterpolation(static_cast<pixelpart::CurveInterpolation>(method));
		update_simulation();
	}
}
int PixelpartGradient::get_interpolation() const {
	if(nativeGradient) {
		return static_cast<int>(nativeGradient->getInterpolation());
	}

	return static_cast<int>(pixelpart::CurveInterpolation::step);
}

void PixelpartGradient::enable_adaptive_cache() {
	if(nativeGradient) {
		nativeGradient->enableAdaptiveCache();
		update_simulation();
	}
}
void PixelpartGradient::enable_fixed_cache(int size) {
	if(nativeGradient && size > 0) {
		nativeGradient->enableFixedCache(static_cast<std::size_t>(size));
		update_simulation();
	}
}
int PixelpartGradient::get_cache_size() const {
	if(nativeGradient) {
		return nativeGradient->getCacheSize();
	}

	return 0;
}

void PixelpartGradient::update_simulation() {
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