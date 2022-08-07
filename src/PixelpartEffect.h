#ifndef PIXELPART_EFFECT_H
#define PIXELPART_EFFECT_H

#include "PixelpartEffectResource.h"
#include "PixelpartParticleEmitter.h"
#include "PixelpartSprite.h"
#include "PixelpartForceField.h"
#include "PixelpartCollider.h"
#include "ParticleEngine.h"
#include "RenderUtil.h"
#include <Godot.hpp>
#include <Spatial.hpp>
#include <SpatialMaterial.hpp>

namespace godot {
class PixelpartEffect : public Spatial {
	GODOT_CLASS(PixelpartEffect, Spatial)

public:
	static void _register_methods();

	PixelpartEffect();
	~PixelpartEffect();

	void _init();
	void _enter_tree();
	void _exit_tree();

	void _process(float dt);

	void draw();

	void play(bool p);
	void pause();
	void restart();
	void reset();
	bool is_playing() const;
	float get_time() const;

	void set_loop(bool l);
	bool get_loop() const;

	void set_loop_time(float l);
	float get_loop_time() const;

	void set_speed(float s);
	float get_speed() const;

	void set_frame_rate(float r);
	float get_frame_rate() const;

	void set_flip_h(bool flip);
	void set_flip_v(bool flip);
	void set_billboard_mode(int mode);
	bool get_flip_h() const;
	bool get_flip_v() const;
	int get_billboard_mode() const;

	float get_import_scale() const;

	void set_effect(Ref<PixelpartEffectResource> effectRes);
	Ref<PixelpartEffectResource> get_effect() const;

	Ref<PixelpartParticleEmitter> get_particle_emitter(String name) const;
	Ref<PixelpartSprite> get_sprite(String name) const;
	Ref<PixelpartForceField> get_force_field(String name) const;
	Ref<PixelpartCollider> get_collider(String name) const;
	Ref<PixelpartParticleEmitter> get_particle_emitter_by_id(int id) const;
	Ref<PixelpartSprite> get_sprite_by_id(int id) const;
	Ref<PixelpartForceField> get_force_field_by_id(int id) const;
	Ref<PixelpartCollider> get_collider_by_id(int id) const;
	Ref<PixelpartParticleEmitter> get_particle_emitter_by_index(int index) const;
	Ref<PixelpartSprite> get_sprite_by_index(int index) const;
	Ref<PixelpartForceField> get_force_field_by_index(int index) const;
	Ref<PixelpartCollider> get_collider_by_index(int index) const;

private:
	struct EmitterInstance {
		pixelpart::ParticleMeshBuilder meshBuilder;
		RID immediate;
		RID instance;
		RID material;
		RID shader;
		std::vector<std::string> textures;
	};
	struct SpriteInstance {
		RID immediate;
		RID instance;
		RID material;
		RID shader;
		std::vector<std::string> textures;
	};

	void draw_emitter(const pixelpart::ParticleEmitter& emitter, pixelpart::ParticleMeshBuilder& meshBuilder, RID instance, RID immediate, RID shader, RID material, const std::vector<RID>& textures);
	void draw_sprite(const pixelpart::Sprite& sprite, RID instance, RID immediate, RID shader, RID material, const std::vector<RID>& textures);

	Transform get_final_transform();

	Ref<PixelpartEffectResource> effectResource;
	pixelpart::Effect nativeEffect;

	std::unordered_map<std::string, Ref<PixelpartParticleEmitter>> particleEmitters;
	std::unordered_map<std::string, Ref<PixelpartSprite>> sprites;
	std::unordered_map<std::string, Ref<PixelpartForceField>> forceFields;
	std::unordered_map<std::string, Ref<PixelpartCollider>> colliders;

	pixelpart::ParticleEngine particleEngine;
	float simulationTime = 0.0f;

	bool playing = true;
	bool loop = false;
	float loopTime = 1.0f;
	float speed = 1.0f;
	float timeStep = 1.0f / 60.0f;

	bool flipH = false;
	bool flipV = false;
	int billboardMode = SpatialMaterial::BILLBOARD_DISABLED;

	std::vector<EmitterInstance> emitterInstances;
	std::vector<SpriteInstance> spriteInstances;
	std::unordered_map<std::string, RID> textures;
};
}

#endif