#ifndef PIXELPART_EFFECT_H
#define PIXELPART_EFFECT_H

#include "PixelpartEffectResource.h"
#include "PixelpartParticleEmitter.h"
#include "PixelpartParticleType.h"
#include "PixelpartForceField.h"
#include "PixelpartCollider.h"
#include "PixelpartSortUtil.h"
#include "ParticleEngine.h"
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
	bool get_flip_h() const;
	bool get_flip_v() const;

	float get_import_scale() const;

	void set_effect(Ref<PixelpartEffectResource> effectRes);
	Ref<PixelpartEffectResource> get_effect() const;

	Ref<PixelpartParticleEmitter> find_particle_emitter(String name) const;
	Ref<PixelpartParticleType> find_particle_type(String name) const;
	Ref<PixelpartForceField> find_force_field(String name) const;
	Ref<PixelpartCollider> find_collider(String name) const;
	Ref<PixelpartParticleEmitter> get_particle_emitter(int id) const;
	Ref<PixelpartParticleType> get_particle_type(int id) const;
	Ref<PixelpartForceField> get_force_field(int id) const;
	Ref<PixelpartCollider> get_collider(int id) const;
	Ref<PixelpartParticleEmitter> get_particle_emitter_at_index(int index) const;
	Ref<PixelpartParticleType> get_particle_type_at_index(int index) const;
	Ref<PixelpartForceField> get_force_field_at_index(int index) const;
	Ref<PixelpartCollider> get_collider_at_index(int index) const;

private:
	struct ParticleMeshInstance {
		struct ParticleTrail {
			uint32_t numParticles = 0;
			pixelpart::floatd length = 0.0;

			std::vector<pixelpart::vec3d> position;
			std::vector<pixelpart::vec3d> size;
			std::vector<pixelpart::vec4d> color;
			std::vector<pixelpart::vec3d> velocity;
			std::vector<pixelpart::vec3d> force;
			std::vector<pixelpart::vec3d> direction;
			std::vector<pixelpart::floatd> index;
			std::vector<pixelpart::floatd> life;
		};

		RID immediate;
		RID instance;
		RID material;
		RID shader;

		std::vector<std::string> textures;

		pixelpart::ParticleData sortedParticleData;
		std::vector<uint32_t> sortKeys;

		std::unordered_map<uint32_t, ParticleTrail> trails;
	};

	void draw_particles(const pixelpart::ParticleType& particleType, ParticleMeshInstance& meshInstance);

	void add_particle_mesh(ParticleMeshInstance& meshInstance, const pixelpart::ParticleType& particleType, const pixelpart::ParticleData& particles, uint32_t numParticles, const pixelpart::vec3d& scale);
	void add_particle_sprites(ParticleMeshInstance& meshInstance, const pixelpart::ParticleType& particleType, const pixelpart::ParticleData& particles, uint32_t numParticles, const pixelpart::vec3d& scale);
	void add_particle_trails(ParticleMeshInstance& meshInstance, const pixelpart::ParticleType& particleType, const pixelpart::ParticleData& particles, uint32_t numParticles, const pixelpart::vec3d& scale);

	pixelpart::mat3d rotation3d(const pixelpart::vec3d& angle);
	pixelpart::mat3d lookAt(const pixelpart::vec3d& direction);

	Ref<PixelpartEffectResource> effectResource;
	pixelpart::Effect nativeEffect;

	std::unordered_map<std::string, Ref<PixelpartParticleEmitter>> particleEmitters;
	std::unordered_map<std::string, Ref<PixelpartParticleType>> particleTypes;
	std::unordered_map<std::string, Ref<PixelpartForceField>> forceFields;
	std::unordered_map<std::string, Ref<PixelpartCollider>> colliders;

	std::unique_ptr<pixelpart::ParticleEngine> particleEngine;
	float simulationTime = 0.0f;

	bool playing = true;
	bool loop = false;
	float loopTime = 1.0f;
	float speed = 1.0f;
	float timeStep = 1.0f / 60.0f;

	bool flipH = false;
	bool flipV = false;

	std::vector<ParticleMeshInstance> particleMeshInstances;
	std::unordered_map<std::string, RID> textures;
};
}

#endif