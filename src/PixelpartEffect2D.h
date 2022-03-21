#ifndef PIXELPART_EFFECT2D_H
#define PIXELPART_EFFECT2D_H

#include "PixelpartEffectResource.h"
#include "PixelpartParticleEmitter.h"
#include "PixelpartForceField.h"
#include "PixelpartCollider.h"
#include "PixelpartSprite.h"
#include "ParticleEngine.h"
#include <memory>
#include <unordered_map>
#include <Godot.hpp>
#include <Node2D.hpp>
#include <VisualServer.hpp>
#include <ImageTexture.hpp>

namespace godot {
class PixelpartEffect2D : public Node2D {
	GODOT_CLASS(PixelpartEffect2D, Node2D)

public:
	static void _register_methods();

	PixelpartEffect2D();
	~PixelpartEffect2D();

	void _init();
	void _enter_tree();
	void _exit_tree();

	void _process(float dt);
	void _update_draw();

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

	void set_flip_h(bool flip_h);
	void set_flip_v(bool flip_v);
	bool get_flip_h() const;
	bool get_flip_v() const;

	float get_import_scale() const;

	void set_effect(Ref<PixelpartEffectResource> effectRes);
	Ref<PixelpartEffectResource> get_effect() const;

	Ref<PixelpartParticleEmitter> get_particle_emitter(String name) const;
	Ref<PixelpartForceField> get_force_field(String name) const;
	Ref<PixelpartCollider> get_collider(String name) const;
	Ref<PixelpartSprite> get_sprite(String name) const;
	Ref<PixelpartParticleEmitter> get_particle_emitter_by_id(int id) const;
	Ref<PixelpartParticleEmitter> get_particle_emitter_by_index(int index) const;
	Ref<PixelpartForceField> get_force_field_by_index(int index) const;
	Ref<PixelpartCollider> get_collider_by_index(int index) const;
	Ref<PixelpartSprite> get_sprite_by_index(int index) const;

private:
	struct InstanceData {
		RID canvasItem;
		RID material;
		RID texture;
	};
	
	void draw_emitter2d(const pixelpart::ParticleEmitter& emitter, RID canvasItem, RID material, RID spriteTexture);
	void draw_sprite2d(const pixelpart::Sprite& sprite, RID canvasItem, RID material, RID spriteTexture);

	Ref<PixelpartEffectResource> effectResource;
	pixelpart::Effect nativeEffect;

	std::unordered_map<std::string, Ref<PixelpartParticleEmitter>> particleEmitters;
	std::unordered_map<std::string, Ref<PixelpartForceField>> forceFields;
	std::unordered_map<std::string, Ref<PixelpartCollider>> colliders;
	std::unordered_map<std::string, Ref<PixelpartSprite>> sprites;

	pixelpart::ParticleEngine particleEngine;
	float simulationTime = 0.0f;

	bool playing = true;
	bool loop = false;
	float loopTime = 1.0f;
	float speed = 1.0f;
	float timeStep = 1.0f / 60.0f;

	bool flipH = false;
	bool flipV = true;

	std::vector<InstanceData> emitterInstances;
	std::vector<InstanceData> spriteInstances;
};
}

#endif