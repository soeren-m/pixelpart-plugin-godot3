#include "PixelpartSprite.h"
#include "PixelpartUtil.h"

namespace godot {
void PixelpartSprite::_register_methods() {
	register_property<PixelpartSprite, float>("lifetime_start", &PixelpartSprite::set_lifetime_start, &PixelpartSprite::get_lifetime_start, 0.0f);
	register_property<PixelpartSprite, float>("lifetime_duration", &PixelpartSprite::set_lifetime_duration, &PixelpartSprite::get_lifetime_duration, 0.0f);
	register_property<PixelpartSprite, bool>("repeat", &PixelpartSprite::set_repeat, &PixelpartSprite::get_repeat, false);
	register_property<PixelpartSprite, int>("blend_mode", &PixelpartSprite::set_blend_mode, &PixelpartSprite::get_blend_mode, 0);
	register_property<PixelpartSprite, int>("color_mode", &PixelpartSprite::set_color_mode, &PixelpartSprite::get_color_mode, 0);
	register_property<PixelpartSprite, int>("layer", &PixelpartSprite::set_layer, &PixelpartSprite::get_layer, 0);
	register_property<PixelpartSprite, bool>("visible", &PixelpartSprite::set_visible, &PixelpartSprite::is_visible, true);
	register_method("_init", &PixelpartSprite::_init);
	register_method("get_name", &PixelpartSprite::get_name);
	register_method("is_active", &PixelpartSprite::is_active);
	register_method("get_local_time", &PixelpartSprite::get_local_time);
	register_method("get_width", &PixelpartSprite::get_width);
	register_method("get_height", &PixelpartSprite::get_height);
	register_method("get_orientation", &PixelpartSprite::get_orientation);
	register_method("get_motion_path", &PixelpartSprite::get_motion_path);
	register_method("get_color", &PixelpartSprite::get_color);
	register_method("get_opacity", &PixelpartSprite::get_opacity);
}

PixelpartSprite::PixelpartSprite() {

}
PixelpartSprite::~PixelpartSprite() {

}

void PixelpartSprite::_init() {

}
void PixelpartSprite::init(Ref<PixelpartEffectResource> resource, pixelpart::Sprite* sprite, pixelpart::ParticleEngine* engine) {
	effectResource = resource;
	nativeSprite = sprite;
	nativeParticleEngine = engine;
}

String PixelpartSprite::get_name() const {
	if(nativeSprite) {
		return String(nativeSprite->name.c_str());
	}

	return String();
}

void PixelpartSprite::set_lifetime_start(float time) {
	if(nativeSprite) {
		nativeSprite->lifetimeStart = time;
	}
}
void PixelpartSprite::set_lifetime_duration(float time) {
	if(nativeSprite) {
		nativeSprite->lifetimeDuration = time;
	}
}
void PixelpartSprite::set_repeat(bool value) {
	if(nativeSprite) {
		nativeSprite->repeat = value;
	}
}
float PixelpartSprite::get_lifetime_start() const {
	if(nativeSprite) {
		return static_cast<float>(nativeSprite->lifetimeStart);
	}

	return 0.0f;
}
float PixelpartSprite::get_lifetime_duration() const {
	if(nativeSprite) {
		return static_cast<float>(nativeSprite->lifetimeDuration);
	}

	return 0.0f;
}
bool PixelpartSprite::get_repeat() const {
	if(nativeSprite) {
		return nativeSprite->repeat;
	}

	return false;
}
bool PixelpartSprite::is_active() const {
	if(nativeSprite) {
		return
			nativeParticleEngine->getTime() >= nativeSprite->lifetimeStart &&
			(nativeParticleEngine->getTime() <= nativeSprite->lifetimeStart + nativeSprite->lifetimeDuration || nativeSprite->repeat);
	}

	return false;
}
float PixelpartSprite::get_local_time() const {
	return static_cast<float>(std::fmod(
		nativeParticleEngine->getTime() - nativeSprite->lifetimeStart, nativeSprite->lifetimeDuration) / nativeSprite->lifetimeDuration);
}

void PixelpartSprite::set_blend_mode(int mode) {	
	if(nativeSprite) {
		nativeSprite->blendMode = static_cast<pixelpart::BlendMode>(mode);
	}
}
void PixelpartSprite::set_color_mode(int mode) {	
	if(nativeSprite) {
		nativeSprite->colorMode = static_cast<pixelpart::ColorMode>(mode);
	}
}
void PixelpartSprite::set_layer(int layer) {
	if(nativeSprite) {
		nativeSprite->layer = static_cast<uint32_t>(std::max(layer, 0));
	}
}
void PixelpartSprite::set_visible(bool visible) {	
	if(nativeSprite) {
		nativeSprite->visible = visible;
	}
}
int PixelpartSprite::get_blend_mode() const {
	if(nativeSprite) {
		return static_cast<int>(nativeSprite->blendMode);
	}

	return static_cast<int>(pixelpart::BlendMode::normal);
}
int PixelpartSprite::get_color_mode() const {
	if(nativeSprite) {
		return static_cast<int>(nativeSprite->colorMode);
	}

	return static_cast<int>(pixelpart::ColorMode::multiply);
}
int PixelpartSprite::get_layer() const {
	if(nativeSprite) {
		return static_cast<int>(nativeSprite->layer);
	}

	return 0;
}
bool PixelpartSprite::is_visible() const {	
	if(nativeSprite) {
		return nativeSprite->visible;
	}

	return false;
}

Ref<PixelpartCurve> PixelpartSprite::get_width() const {	
	if(nativeSprite) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeSprite->width, nativeParticleEngine, PixelpartCurve::ObjectType::sprite);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
Ref<PixelpartCurve> PixelpartSprite::get_height() const {	
	if(nativeSprite) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeSprite->height, nativeParticleEngine, PixelpartCurve::ObjectType::sprite);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
Ref<PixelpartCurve> PixelpartSprite::get_orientation() const {	
	if(nativeSprite) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeSprite->orientation, nativeParticleEngine, PixelpartCurve::ObjectType::sprite);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
Ref<PixelpartPath> PixelpartSprite::get_motion_path() const {	
	if(nativeSprite) {
		Ref<PixelpartPath> path;
		path.instance();
		path->init(&nativeSprite->motionPath, nativeParticleEngine, PixelpartPath::ObjectType::sprite);

		return path;
	}

	return Ref<PixelpartPath>();
}
Ref<PixelpartGradient> PixelpartSprite::get_color() const {	
	if(nativeSprite) {
		Ref<PixelpartGradient> gradient;
		gradient.instance();
		gradient->init(&nativeSprite->color, nativeParticleEngine, PixelpartGradient::ObjectType::sprite);

		return gradient;
	}

	return Ref<PixelpartGradient>();
}
Ref<PixelpartCurve> PixelpartSprite::get_opacity() const {	
	if(nativeSprite) {
		Ref<PixelpartCurve> curve;
		curve.instance();
		curve->init(&nativeSprite->opacity, nativeParticleEngine, PixelpartCurve::ObjectType::sprite);

		return curve;
	}

	return Ref<PixelpartCurve>();
}
}