#include "PixelpartPath.h"
#include "PixelpartUtil.h"

namespace godot {
void PixelpartPath::_register_methods() {
	register_property<PixelpartPath, int>("interpolation", &PixelpartPath::set_interpolation, &PixelpartPath::get_interpolation, 0);
	register_method("_init", &PixelpartPath::_init);
	register_method("get", &PixelpartPath::get);
	register_method("get_point", &PixelpartPath::get_point);
	register_method("set", &PixelpartPath::set);
	register_method("add_point", &PixelpartPath::add_point);
	register_method("set_point", &PixelpartPath::set_point);
	register_method("move_point", &PixelpartPath::move_point);
	register_method("shift_point", &PixelpartPath::shift_point);
	register_method("remove_point", &PixelpartPath::remove_point);
	register_method("clear", &PixelpartPath::clear);
	register_method("get_num_points", &PixelpartPath::get_num_points);
	register_method("move", &PixelpartPath::move);
	register_method("shift", &PixelpartPath::shift);
	register_method("enable_adaptive_cache", &PixelpartPath::enable_adaptive_cache);
	register_method("enable_fixed_cache", &PixelpartPath::enable_fixed_cache);
	register_method("get_cache_size", &PixelpartPath::get_cache_size);
}

PixelpartPath::PixelpartPath() {

}

void PixelpartPath::_init() {

}
void PixelpartPath::init(pixelpart::Curve<pixelpart::vec2d>* path, pixelpart::ParticleEngine* engine, ObjectType type) {
	nativePath = path;
	nativeParticleEngine = engine;
	objectType = type;
}

Vector2 PixelpartPath::get(float t) const {
	if(nativePath) {
		return pp2gd(nativePath->get(t));
	}

	return Vector2(0.0f, 0.0f);
}
Vector2 PixelpartPath::get_point(int index) const {
	if(nativePath && index >= 0 && static_cast<std::size_t>(index) < nativePath->getNumPoints()) {
		return pp2gd(nativePath->getPoints()[static_cast<std::size_t>(index)].value);
	}

	return Vector2(0.0f, 0.0f);
}

void PixelpartPath::set(Vector2 value) {
	if(nativePath) {
		nativePath->clear();
		nativePath->setPoints({ pixelpart::Curve<pixelpart::vec2d>::Point{ 0.5, gd2pp(value) } });
		update_simulation();
	}
}
void PixelpartPath::add_point(float t, Vector2 value) {
	if(nativePath) {
		nativePath->addPoint(t, gd2pp(value));
		update_simulation();
	}
}
void PixelpartPath::set_point(int index, Vector2 value) {
	if(nativePath) {
		nativePath->setPoint(static_cast<std::size_t>(index), gd2pp(value));
		update_simulation();
	}
}
void PixelpartPath::move_point(int index, Vector2 delta) {
	if(nativePath) {
		nativePath->movePoint(static_cast<std::size_t>(index), gd2pp(delta));
		update_simulation();
	}
}
void PixelpartPath::shift_point(int index, float delta) {
	if(nativePath) {
		nativePath->shiftPoint(static_cast<std::size_t>(index), delta);
		update_simulation();
	}
}
void PixelpartPath::remove_point(int index) {
	if(nativePath) {
		nativePath->removePoint(static_cast<std::size_t>(index));
		update_simulation();
	}
}
void PixelpartPath::clear() {
	if(nativePath) {
		nativePath->clear();
		update_simulation();
	}
}
int PixelpartPath::get_num_points() const {
	if(nativePath) {
		return static_cast<int>(nativePath->getNumPoints());
	}

	return 0;
}

void PixelpartPath::move(Vector2 delta) {
	if(nativePath) {
		nativePath->move(gd2pp(delta));
		update_simulation();
	}
}
void PixelpartPath::shift(float delta) {
	if(nativePath) {
		nativePath->shift(delta);
		update_simulation();
	}
}

void PixelpartPath::set_interpolation(int method) {
	if(nativePath) {
		nativePath->setInterpolation(static_cast<pixelpart::CurveInterpolation>(method));
		update_simulation();
	}
}
int PixelpartPath::get_interpolation() const {
	if(nativePath) {
		return static_cast<int>(nativePath->getInterpolation());
	}

	return static_cast<int>(pixelpart::CurveInterpolation::step);
}

void PixelpartPath::enable_adaptive_cache() {
	if(nativePath) {
		nativePath->enableAdaptiveCache();
		update_simulation();
	}
}
void PixelpartPath::enable_fixed_cache(int size) {
	if(nativePath && size > 0) {
		nativePath->enableFixedCache(static_cast<std::size_t>(size));
		update_simulation();
	}
}
int PixelpartPath::get_cache_size() const {
	if(nativePath) {
		return nativePath->getCacheSize();
	}

	return 0;
}

void PixelpartPath::update_simulation() {
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