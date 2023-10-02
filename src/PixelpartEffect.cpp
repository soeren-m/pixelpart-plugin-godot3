#include "PixelpartEffect.h"
#include "PixelpartShaders.h"
#include "PixelpartUtil.h"
#include "ParticleSolverCPU.h"
#include <VisualServer.hpp>
#include <ProjectSettings.hpp>
#include <World.hpp>
#include <Viewport.hpp>
#include <Camera.hpp>
#include <Mesh.hpp>
#include <ImageTexture.hpp>

namespace godot {
void PixelpartEffect::_register_methods() {
	register_property<PixelpartEffect, Ref<PixelpartEffectResource>>("effect", &PixelpartEffect::set_effect, &PixelpartEffect::get_effect, nullptr);
	register_property<PixelpartEffect, bool>("playing", &PixelpartEffect::play, &PixelpartEffect::is_playing, true);
	register_property<PixelpartEffect, bool>("loop", &PixelpartEffect::set_loop, &PixelpartEffect::get_loop, false);
	register_property<PixelpartEffect, float>("loop_time", &PixelpartEffect::set_loop_time, &PixelpartEffect::get_loop_time, 1.0f,
		GODOT_METHOD_RPC_MODE_DISABLED,
		GODOT_PROPERTY_USAGE_DEFAULT,
		GODOT_PROPERTY_HINT_EXP_RANGE, "0.0,1000.0,0.01");
	register_property<PixelpartEffect, float>("speed", &PixelpartEffect::set_speed, &PixelpartEffect::get_speed, 1.0f,
		GODOT_METHOD_RPC_MODE_DISABLED,
		GODOT_PROPERTY_USAGE_DEFAULT,
		GODOT_PROPERTY_HINT_EXP_RANGE, "0.0,100.0,0.01");
	register_property<PixelpartEffect, float>("frame_rate", &PixelpartEffect::set_frame_rate, &PixelpartEffect::get_frame_rate, 60.0f,
		GODOT_METHOD_RPC_MODE_DISABLED,
		GODOT_PROPERTY_USAGE_DEFAULT,
		GODOT_PROPERTY_HINT_EXP_RANGE, "1.0,100.0,1.0");
	register_property<PixelpartEffect, bool>("flip_h", &PixelpartEffect::set_flip_h, &PixelpartEffect::get_flip_h, false);
	register_property<PixelpartEffect, bool>("flip_v", &PixelpartEffect::set_flip_v, &PixelpartEffect::get_flip_v, false);
	register_method("_init", &PixelpartEffect::_init);
	register_method("_enter_tree", &PixelpartEffect::_enter_tree);
	register_method("_exit_tree", &PixelpartEffect::_exit_tree);
	register_method("_process", &PixelpartEffect::_process);
	register_method("draw", &PixelpartEffect::draw);
	register_method("play", &PixelpartEffect::play);
	register_method("pause", &PixelpartEffect::pause);
	register_method("restart", &PixelpartEffect::restart);
	register_method("reset", &PixelpartEffect::reset);
	register_method("is_playing", &PixelpartEffect::is_playing);
	register_method("get_time", &PixelpartEffect::get_time);
	register_method("get_import_scale", &PixelpartEffect::get_import_scale);
	register_method("find_particle_emitter", &PixelpartEffect::find_particle_emitter);
	register_method("find_particle_type", &PixelpartEffect::find_particle_type);
	register_method("find_force_field", &PixelpartEffect::find_force_field);
	register_method("find_collider", &PixelpartEffect::find_collider);
	register_method("get_particle_emitter", &PixelpartEffect::get_particle_emitter);
	register_method("get_particle_type", &PixelpartEffect::get_particle_type);
	register_method("get_force_field", &PixelpartEffect::get_force_field);
	register_method("get_collider", &PixelpartEffect::get_collider);
	register_method("get_particle_emitter_at_index", &PixelpartEffect::get_particle_emitter_at_index);
	register_method("get_particle_type_at_index", &PixelpartEffect::get_particle_type_at_index);
	register_method("get_force_field_at_index", &PixelpartEffect::get_force_field_at_index);
	register_method("get_collider_at_index", &PixelpartEffect::get_collider_at_index);
}

PixelpartEffect::PixelpartEffect() {
	auto settings = ProjectSettings::get_singleton();

	uint32_t particleCapacity = 10000;
	if(settings->has_setting("pixelpart/particle_capacity")) {
		particleCapacity = static_cast<uint32_t>(std::max((int)settings->get_setting("pixelpart/particle_capacity"), 1));
	}

	particleEngine = pixelpart::ParticleEngine::createUnique<pixelpart::ParticleSolverCPU>(nullptr, particleCapacity);
}
PixelpartEffect::~PixelpartEffect() {
	VisualServer* vs = VisualServer::get_singleton();

	for(ParticleMeshInstance& inst : particleMeshInstances) {
		vs->free_rid(inst.immediate);
		vs->free_rid(inst.instance);
		vs->free_rid(inst.material);
	}

	for(auto& entry : textures) {
		vs->free_rid(entry.second);
	}
}

void PixelpartEffect::_init() {
	simulationTime = 0.0f;

	playing = true;
	loop = false;
	loopTime = 1.0f;
	speed = 1.0f;
	timeStep = 1.0f / 60.0f;

	flipH = false;
	flipV = false;
}
void PixelpartEffect::_enter_tree() {
	VisualServer::get_singleton()->connect("frame_pre_draw", this, "draw");
}
void PixelpartEffect::_exit_tree() {
	VisualServer::get_singleton()->disconnect("frame_pre_draw", this, "draw");
}

void PixelpartEffect::_process(float dt) {
	if(!particleEngine->getEffect()) {
		return;
	}

	if(playing) {
		simulationTime += std::clamp(dt, 0.0f, 1.0f) * speed;

		while(simulationTime > timeStep) {
			simulationTime -= timeStep;
			particleEngine->step(timeStep);

			if(loop) {
				if(particleEngine->getTime() > loopTime) {
					particleEngine->reset();
				}
			}
		}
	}
}
void PixelpartEffect::draw() {
	Viewport* viewport = get_viewport();
	if(!viewport) {
		return;
	}

	const pixelpart::Effect* effect = particleEngine->getEffect();
	if(!effect) {
		return;
	}

	for(uint32_t particleTypeIndex = 0; particleTypeIndex < effect->particleTypes.getCount(); particleTypeIndex++) {
		draw_particles(effect->particleTypes.getByIndex(particleTypeIndex), particleMeshInstances[particleTypeIndex]);
	}
}

void PixelpartEffect::play(bool p) {
	playing = p;
}
void PixelpartEffect::pause() {
	playing = false;
}
void PixelpartEffect::restart() {
	particleEngine->restart();
}
void PixelpartEffect::reset() {
	particleEngine->reset();
}
bool PixelpartEffect::is_playing() const {
	return playing;
}
float PixelpartEffect::get_time() const {
	return static_cast<float>(particleEngine->getTime());
}

void PixelpartEffect::set_loop(bool l) {
	loop = l;
}
bool PixelpartEffect::get_loop() const {
	return loop;
}

void PixelpartEffect::set_loop_time(float l) {
	loopTime = std::max(l, 0.0f);
}
float PixelpartEffect::get_loop_time() const {
	return loopTime;
}

void PixelpartEffect::set_speed(float s) {
	speed = std::max(s, 0.0f);
}
float PixelpartEffect::get_speed() const {
	return speed;
}

void PixelpartEffect::set_frame_rate(float r) {
	timeStep = 1.0f / std::min(std::max(r, 1.0f), 100.0f);
}
float PixelpartEffect::get_frame_rate() const {
	return 1.0f / timeStep;
}

void PixelpartEffect::set_flip_h(bool flip) {
	flipH = flip;
}
void PixelpartEffect::set_flip_v(bool flip) {
	flipV = flip;
}
bool PixelpartEffect::get_flip_h() const {
	return flipH;
}
bool PixelpartEffect::get_flip_v() const {
	return flipV;
}

float PixelpartEffect::get_import_scale() const {
	if(!effectResource.is_valid()) {
		return 1.0f;
	}

	return effectResource->get_scale();
}

void PixelpartEffect::set_effect(Ref<PixelpartEffectResource> effectRes) {
	VisualServer* vs = VisualServer::get_singleton();
	PixelpartShaders* shaders = PixelpartShaders::get_instance();

	for(ParticleMeshInstance& inst : particleMeshInstances) {
		vs->free_rid(inst.immediate);
		vs->free_rid(inst.instance);
		vs->free_rid(inst.material);
	}

	for(auto& entry : textures) {
		vs->free_rid(entry.second);
	}

	particleMeshInstances.clear();
	particleEmitters.clear();
	particleTypes.clear();
	forceFields.clear();
	colliders.clear();
	textures.clear();

	effectResource = effectRes;

	if(effectResource.is_valid()) {
		effectResource->load();

		nativeEffect = effectResource->get_project().effect;
		particleEngine->setEffect(&nativeEffect);

		try {
			for(pixelpart::ParticleEmitter& particleEmitter : nativeEffect.particleEmitters) {
				Ref<PixelpartParticleEmitter> emitterRef;
				emitterRef.instance();
				emitterRef->init(effectResource, &particleEmitter, particleEngine.get());
				particleEmitters[particleEmitter.name] = emitterRef;
			}

			for(pixelpart::ParticleType& particleType : nativeEffect.particleTypes) {
				Ref<PixelpartParticleType> particleTypeRef;
				particleTypeRef.instance();
				particleTypeRef->init(effectResource, &particleType, particleEngine.get());
				particleTypes[particleType.name] = particleTypeRef;
			}

			for(pixelpart::ForceField& forceField : nativeEffect.forceFields) {
				Ref<PixelpartForceField> forceFieldRef;
				forceFieldRef.instance();
				forceFieldRef->init(effectResource, &forceField, particleEngine.get());
				forceFields[forceField.name] = forceFieldRef;
			}

			for(pixelpart::Collider& collider : nativeEffect.colliders) {
				Ref<PixelpartCollider> colliderRef;
				colliderRef.instance();
				colliderRef->init(effectResource, &collider, particleEngine.get());
				colliders[collider.name] = colliderRef;
			}

			for(const pixelpart::ParticleType& particleType : nativeEffect.particleTypes) {
				pixelpart::ShaderGraph::BuildResult buildResult;
				particleType.shader.build(buildResult);

				particleMeshInstances.push_back(ParticleMeshInstance{
					vs->immediate_create(),
					vs->instance_create(),
					vs->material_create(),
					shaders->get_shader(buildResult.code,
						"spatial",
						(particleType.blendMode == pixelpart::BlendMode::additive) ? "cull_disabled,unshaded,blend_add" :
						(particleType.blendMode == pixelpart::BlendMode::subtractive) ? "cull_disabled,unshaded,blend_sub" :
						"cull_disabled,unshaded,blend_mix"),
					buildResult.textureIds
				});
			}

			for(const auto& resource : effectResource->get_project_resources().images) {
				PoolByteArray imageData;
				imageData.resize(resource.second.data.size());
				memcpy(imageData.write().ptr(), resource.second.data.data(), resource.second.data.size());

				Ref<Image> image;
				image.instance();
				image->create_from_data(resource.second.width, resource.second.height, false, Image::FORMAT_RGBA8, imageData);

				textures[resource.first] = vs->texture_create_from_image(image, Texture::FLAG_FILTER | Texture::FLAG_REPEAT);
			}
		}
		catch(std::exception& e) {
			particleEngine->setEffect(nullptr);

			Godot::print_error(String(e.what()), __FUNCTION__, "PixelpartEffect.cpp", __LINE__);
		}
	}
	else {
		particleEngine->setEffect(nullptr);
	}
}
Ref<PixelpartEffectResource> PixelpartEffect::get_effect() const {
	return effectResource;
}

Ref<PixelpartParticleEmitter> PixelpartEffect::find_particle_emitter(String name) const {
	CharString nameCharString = name.utf8();
	std::string nameStdString = std::string(nameCharString.get_data(), nameCharString.length());

	if(particleEmitters.count(nameStdString)) {
		return particleEmitters.at(nameStdString);
	}

	return Ref<PixelpartParticleEmitter>();
}
Ref<PixelpartParticleType> PixelpartEffect::find_particle_type(String name) const {
	CharString nameCharString = name.utf8();
	std::string nameStdString = std::string(nameCharString.get_data(), nameCharString.length());

	if(particleTypes.count(nameStdString)) {
		return particleTypes.at(nameStdString);
	}

	return Ref<PixelpartParticleType>();
}
Ref<PixelpartForceField> PixelpartEffect::find_force_field(String name) const {
	CharString nameCharString = name.utf8();
	std::string nameStdString = std::string(nameCharString.get_data(), nameCharString.length());

	if(forceFields.count(nameStdString)) {
		return forceFields.at(nameStdString);
	}

	return Ref<PixelpartForceField>();
}
Ref<PixelpartCollider> PixelpartEffect::find_collider(String name) const {
	CharString nameCharString = name.utf8();
	std::string nameStdString = std::string(nameCharString.get_data(), nameCharString.length());

	if(colliders.count(nameStdString)) {
		return colliders.at(nameStdString);
	}

	return Ref<PixelpartCollider>();
}
Ref<PixelpartParticleEmitter> PixelpartEffect::get_particle_emitter(int id) const {
	if(id >= 0 && nativeEffect.particleEmitters.contains(static_cast<uint32_t>(id))) {
		std::string name = nativeEffect.particleEmitters.get(static_cast<uint32_t>(id)).name;

		if(particleEmitters.count(name)) {
			return particleEmitters.at(name);
		}
	}

	return Ref<PixelpartParticleEmitter>();
}
Ref<PixelpartParticleType> PixelpartEffect::get_particle_type(int id) const {
	if(id >= 0 && nativeEffect.particleTypes.contains(static_cast<uint32_t>(id))) {
		std::string name = nativeEffect.particleTypes.get(static_cast<uint32_t>(id)).name;

		if(particleTypes.count(name)) {
			return particleTypes.at(name);
		}
	}

	return Ref<PixelpartParticleType>();
}
Ref<PixelpartForceField> PixelpartEffect::get_force_field(int id) const {
	if(id >= 0 && nativeEffect.forceFields.contains(static_cast<uint32_t>(id))) {
		std::string name = nativeEffect.forceFields.get(static_cast<uint32_t>(id)).name;

		if(forceFields.count(name)) {
			return forceFields.at(name);
		}
	}

	return Ref<PixelpartForceField>();
}
Ref<PixelpartCollider> PixelpartEffect::get_collider(int id) const {
	if(id >= 0 && nativeEffect.colliders.contains(static_cast<uint32_t>(id))) {
		std::string name = nativeEffect.colliders.get(static_cast<uint32_t>(id)).name;

		if(colliders.count(name)) {
			return colliders.at(name);
		}
	}

	return Ref<PixelpartCollider>();
}
Ref<PixelpartParticleEmitter> PixelpartEffect::get_particle_emitter_at_index(int index) const {
	if(index >= 0 && nativeEffect.particleEmitters.containsIndex(static_cast<uint32_t>(index))) {
		std::string name = nativeEffect.particleEmitters.getByIndex(static_cast<uint32_t>(index)).name;

		if(particleEmitters.count(name)) {
			return particleEmitters.at(name);
		}
	}

	return Ref<PixelpartParticleEmitter>();
}
Ref<PixelpartParticleType> PixelpartEffect::get_particle_type_at_index(int index) const {
	if(index >= 0 && nativeEffect.particleTypes.containsIndex(static_cast<uint32_t>(index))) {
		std::string name = nativeEffect.particleTypes.getByIndex(static_cast<uint32_t>(index)).name;

		if(particleTypes.count(name)) {
			return particleTypes.at(name);
		}
	}

	return Ref<PixelpartParticleType>();
}
Ref<PixelpartForceField> PixelpartEffect::get_force_field_at_index(int index) const {
	if(index >= 0 && nativeEffect.forceFields.containsIndex(static_cast<uint32_t>(index))) {
		std::string name = nativeEffect.forceFields.getByIndex(static_cast<uint32_t>(index)).name;

		if(forceFields.count(name)) {
			return forceFields.at(name);
		}
	}

	return Ref<PixelpartForceField>();
}
Ref<PixelpartCollider> PixelpartEffect::get_collider_at_index(int index) const {
	if(index >= 0 && nativeEffect.colliders.containsIndex(static_cast<uint32_t>(index))) {
		std::string name = nativeEffect.colliders.getByIndex(static_cast<uint32_t>(index)).name;

		if(colliders.count(name)) {
			return colliders.at(name);
		}
	}

	return Ref<PixelpartCollider>();
}

void PixelpartEffect::draw_particles(const pixelpart::ParticleType& particleType, ParticleMeshInstance& meshInstance) {
	VisualServer* vs = VisualServer::get_singleton();
	vs->immediate_clear(meshInstance.immediate);

	if(!is_visible() || !particleType.visible) {
		return;
	}

	const pixelpart::Effect* effect = particleEngine->getEffect();
	const pixelpart::ParticleEmitter& particleEmitter = effect->particleEmitters.get(particleType.parentId);
	const uint32_t particleTypeIndex = effect->particleTypes.findById(particleType.id);
	const uint32_t numParticles = particleEngine->getNumParticles(particleTypeIndex);
	const pixelpart::ParticleData& particles = particleEngine->getParticles(particleTypeIndex);
	const pixelpart::vec3d scale = pixelpart::vec3d(
		flipH ? -1.0 : +1.0,
		flipV ? -1.0 : +1.0,
		1.0) * static_cast<pixelpart::floatd>(get_import_scale());

	vs->material_set_shader(meshInstance.material, meshInstance.shader);
	vs->material_set_param(meshInstance.material, "EFFECT_TIME", static_cast<float>(particleEngine->getTime()));
	vs->material_set_param(meshInstance.material, "OBJECT_TIME", static_cast<float>(particleEngine->getTime() - particleEmitter.lifetimeStart));
	for(std::size_t t = 0; t < meshInstance.textures.size(); t++) {
		vs->material_set_param(meshInstance.material, String("TEXTURE") + String::num_int64(t), textures.at(meshInstance.textures[t]));
	}

	vs->instance_set_base(meshInstance.instance, meshInstance.immediate);
	vs->instance_geometry_set_material_override(meshInstance.instance, meshInstance.material);
	vs->instance_set_scenario(meshInstance.instance, get_world()->get_scenario());
	vs->instance_set_transform(meshInstance.instance, get_global_transform());

	add_particle_mesh(meshInstance,
		particleType,
		particles,
		numParticles,
		scale);
}

void PixelpartEffect::add_particle_mesh(ParticleMeshInstance& meshInstance, const pixelpart::ParticleType& particleType, const pixelpart::ParticleData& particles, uint32_t numParticles, const pixelpart::vec3d& scale) {
	switch(particleType.renderer) {
		case pixelpart::ParticleType::Renderer::sprite:
			add_particle_sprites(meshInstance, particleType, particles, numParticles, scale);
			break;
		case pixelpart::ParticleType::Renderer::trail:
			add_particle_trails(meshInstance, particleType, particles, numParticles, scale);
			break;
	}
}

void PixelpartEffect::add_particle_sprites(ParticleMeshInstance& meshInstance, const pixelpart::ParticleType& particleType, const pixelpart::ParticleData& particles, uint32_t numParticles, const pixelpart::vec3d& scale) {
	const pixelpart::floatd packFactor = 10000.0;

	const pixelpart::ParticleEmitter& particleEmitter = nativeEffect.particleEmitters.get(particleType.parentId);
	pixelpart::floatd alpha = std::fmod(particleEngine->getTime() - particleEmitter.lifetimeStart, particleEmitter.lifetimeDuration) / particleEmitter.lifetimeDuration;

	Viewport* viewport = get_viewport();
	if(!viewport) {
		return;
	}

	Camera* camera = viewport->get_camera();
	if(!camera) {
		return;
	}

	const pixelpart::ParticleData* particleRenderData = &particles;

	if(particleType.spriteRendererSettings.sortCriterion != pixelpart::ParticleSortCriterion::none && numParticles > 1) {
		meshInstance.sortedParticleData.resize(particles.id.size());
		meshInstance.sortKeys.resize(particles.id.size());
		std::iota(meshInstance.sortKeys.begin(), meshInstance.sortKeys.begin() + numParticles, 0);

		switch(particleType.spriteRendererSettings.sortCriterion) {
			case pixelpart::ParticleSortCriterion::age: {
				insertionSort(meshInstance.sortKeys.begin(), meshInstance.sortKeys.begin() + numParticles,
					[&particles](uint32_t i, uint32_t j) {
						return particles.id[i] < particles.id[j];
					});

				break;
			}

			case pixelpart::ParticleSortCriterion::distance: {
				pixelpart::vec3d cameraPosition = fromGd(camera->get_global_transform().origin);

				insertionSort(meshInstance.sortKeys.begin(), meshInstance.sortKeys.begin() + numParticles,
					[&particles, cameraPosition](uint32_t i, uint32_t j) {
						return glm::distance2(particles.globalPosition[i], cameraPosition) > glm::distance2(particles.globalPosition[j], cameraPosition);
					});

				break;
			}
		}

		for(uint32_t i = 0; i < numParticles; i++) {
			uint32_t j = meshInstance.sortKeys[i];

			meshInstance.sortedParticleData.id[i] = particles.id[j];
			meshInstance.sortedParticleData.life[i] = particles.life[j];
			meshInstance.sortedParticleData.globalPosition[i] = particles.globalPosition[j];
			meshInstance.sortedParticleData.velocity[i] = particles.velocity[j];
			meshInstance.sortedParticleData.force[i] = particles.force[j];
			meshInstance.sortedParticleData.rotation[i] = particles.rotation[j];
			meshInstance.sortedParticleData.size[i] = particles.size[j];
			meshInstance.sortedParticleData.color[i] = particles.color[j];
		}

		particleRenderData = &meshInstance.sortedParticleData;
	}

	pixelpart::vec3d cameraRight = fromGd(camera->get_global_transform().basis.x);
	pixelpart::vec3d cameraUp = fromGd(camera->get_global_transform().basis.y);

	VisualServer* vs = VisualServer::get_singleton();
	vs->immediate_begin(meshInstance.immediate, Mesh::PRIMITIVE_TRIANGLES);

	for(std::size_t p = 0; p < numParticles; p++) {
		pixelpart::mat3d rotationMatrix = rotation3d(particleRenderData->rotation[p]);
		pixelpart::vec3d pivot = particleType.pivot * particleRenderData->size[p];
		pixelpart::vec3d worldPosition[4];
		pixelpart::vec3d localPosition[4] = {
			rotationMatrix * (pixelpart::vec3d(-0.5, -0.5, 0.0) * particleRenderData->size[p] - pivot) + pivot,
			rotationMatrix * (pixelpart::vec3d(+0.5, -0.5, 0.0) * particleRenderData->size[p] - pivot) + pivot,
			rotationMatrix * (pixelpart::vec3d(+0.5, +0.5, 0.0) * particleRenderData->size[p] - pivot) + pivot,
			rotationMatrix * (pixelpart::vec3d(-0.5, +0.5, 0.0) * particleRenderData->size[p] - pivot) + pivot };

		switch(particleType.alignmentMode) {
			case pixelpart::AlignmentMode::camera: {
				worldPosition[0] = particleRenderData->globalPosition[p] + cameraRight * localPosition[0].x + cameraUp * localPosition[0].y;
				worldPosition[1] = particleRenderData->globalPosition[p] + cameraRight * localPosition[1].x + cameraUp * localPosition[1].y;
				worldPosition[2] = particleRenderData->globalPosition[p] + cameraRight * localPosition[2].x + cameraUp * localPosition[2].y;
				worldPosition[3] = particleRenderData->globalPosition[p] + cameraRight * localPosition[3].x + cameraUp * localPosition[3].y;
				break;
			}
			case pixelpart::AlignmentMode::motion: {
				pixelpart::mat3d lookAtMatrix = lookAt(particleRenderData->velocity[p]);
				worldPosition[0] = particleRenderData->globalPosition[p] + lookAtMatrix * localPosition[0];
				worldPosition[1] = particleRenderData->globalPosition[p] + lookAtMatrix * localPosition[1];
				worldPosition[2] = particleRenderData->globalPosition[p] + lookAtMatrix * localPosition[2];
				worldPosition[3] = particleRenderData->globalPosition[p] + lookAtMatrix * localPosition[3];
				break;
			}
			case pixelpart::AlignmentMode::emission: {
				pixelpart::mat3d lookAtMatrix = lookAt(particleEmitter.position.get(alpha) - particleRenderData->globalPosition[p]);
				worldPosition[0] = particleRenderData->globalPosition[p] + lookAtMatrix * localPosition[0];
				worldPosition[1] = particleRenderData->globalPosition[p] + lookAtMatrix * localPosition[1];
				worldPosition[2] = particleRenderData->globalPosition[p] + lookAtMatrix * localPosition[2];
				worldPosition[3] = particleRenderData->globalPosition[p] + lookAtMatrix * localPosition[3];
				break;
			}
			case pixelpart::AlignmentMode::emitter: {
				pixelpart::mat3d lookAtMatrix = rotation3d(particleEmitter.orientation.get(alpha));
				worldPosition[0] = particleRenderData->globalPosition[p] + lookAtMatrix * localPosition[0];
				worldPosition[1] = particleRenderData->globalPosition[p] + lookAtMatrix * localPosition[1];
				worldPosition[2] = particleRenderData->globalPosition[p] + lookAtMatrix * localPosition[2];
				worldPosition[3] = particleRenderData->globalPosition[p] + lookAtMatrix * localPosition[3];
				break;
			}
			default: {
				worldPosition[0] = particleRenderData->globalPosition[p] + localPosition[0];
				worldPosition[1] = particleRenderData->globalPosition[p] + localPosition[1];
				worldPosition[2] = particleRenderData->globalPosition[p] + localPosition[2];
				worldPosition[3] = particleRenderData->globalPosition[p] + localPosition[3];
				break;
			}
		}

		vs->immediate_tangent(meshInstance.immediate, Plane(
			toGd(particleRenderData->velocity[p]), 0.0f));
		vs->immediate_color(meshInstance.immediate,
			toGd(particleRenderData->color[p] * 0.1 +
			pixelpart::vec4d(glm::floor(particleRenderData->force[p] * packFactor) + pixelpart::vec3d(packFactor), packFactor)));
		vs->immediate_uv2(meshInstance.immediate, Vector2(
			static_cast<float>(particleRenderData->life[p]),
			static_cast<float>(particleRenderData->id[p])));

		vs->immediate_uv(meshInstance.immediate, Vector2(0.0f, 0.0f));
		vs->immediate_vertex(meshInstance.immediate, toGd(worldPosition[0] * scale));

		vs->immediate_uv(meshInstance.immediate, Vector2(1.0f, 0.0f));
		vs->immediate_vertex(meshInstance.immediate, toGd(worldPosition[1] * scale));

		vs->immediate_uv(meshInstance.immediate, Vector2(0.0f, 1.0f));
		vs->immediate_vertex(meshInstance.immediate, toGd(worldPosition[3] * scale));

		vs->immediate_uv(meshInstance.immediate, Vector2(1.0f, 0.0f));
		vs->immediate_vertex(meshInstance.immediate, toGd(worldPosition[1] * scale));

		vs->immediate_uv(meshInstance.immediate, Vector2(1.0f, 1.0f));
		vs->immediate_vertex(meshInstance.immediate, toGd(worldPosition[2] * scale));

		vs->immediate_uv(meshInstance.immediate, Vector2(0.0f, 1.0f));
		vs->immediate_vertex(meshInstance.immediate, toGd(worldPosition[3] * scale));
	}

	vs->immediate_end(meshInstance.immediate);
}

void PixelpartEffect::add_particle_trails(ParticleMeshInstance& meshInstance, const pixelpart::ParticleType& particleType, const pixelpart::ParticleData& particles, uint32_t numParticles, const pixelpart::vec3d& scale) {
	const pixelpart::floatd packFactor = 10000.0;

	if(numParticles < 2) {
		return;
	}

	std::vector<uint32_t> sortKeys(numParticles);
	std::iota(sortKeys.begin(), sortKeys.end(), 0);
	std::sort(sortKeys.begin(), sortKeys.end(), [&particles](uint32_t p0, uint32_t p1) {
		return (particles.parentId[p0] != particles.parentId[p1])
			? particles.parentId[p0] > particles.parentId[p1]
			: particles.id[p0] > particles.id[p1];
	});

	for(auto& entry : meshInstance.trails) {
		entry.second.numParticles = 0;
	}

	ParticleMeshInstance::ParticleTrail* trail = nullptr;
	uint32_t trailId = 0xFFFFFFFF - 1;
	uint32_t particleParentId = 0;

	for(uint32_t p = 0; p < numParticles; p++) {
		particleParentId = particles.parentId[sortKeys[p]];

		if(particleParentId != trailId) {
			trailId = particleParentId;

			if(meshInstance.trails.count(trailId) == 0) {
				meshInstance.trails[trailId] = ParticleMeshInstance::ParticleTrail();
			}

			trail = &meshInstance.trails.at(trailId);
		}

		trail->numParticles++;
	}

	for(auto it = meshInstance.trails.begin(); it != meshInstance.trails.end(); ) {
		if(it->second.numParticles == 0) {
			it = meshInstance.trails.erase(it);
		}
		else {
			it++;
		}
	}

	trail = nullptr;
	trailId = 0xFFFFFFFF - 1;
	particleParentId = 0;

	if(particleType.trailRendererSettings.numTrailSegments > 0) {
		for(uint32_t p = 0; p < numParticles; p++) {
			uint32_t particleIndex = sortKeys[p];
			particleParentId = particles.parentId[particleIndex];

			if(particleParentId != trailId) {
				trailId = particleParentId;
				trail = &meshInstance.trails.at(trailId);

				trail->length = 0.0;
				std::vector<pixelpart::floatd> relativeParticlePosition(trail->numParticles, 0.0);
				for(uint32_t q = 1; q < trail->numParticles; q++) {
					trail->length += glm::length(particles.globalPosition[sortKeys[p + q]] - particles.globalPosition[sortKeys[p + q - 1]]);
					relativeParticlePosition[q] = trail->length;
				}
				for(uint32_t q = 0; q < trail->numParticles; q++) {
					relativeParticlePosition[q] /= trail->length;
				}

				pixelpart::Curve<pixelpart::vec3d> positionCurve(pixelpart::CurveInterpolation::spline);
				positionCurve.enableFixedCache(particleType.trailRendererSettings.numTrailSegments);
				positionCurve.setPointsOrdered<uint32_t>(relativeParticlePosition.data(), particles.globalPosition.data(), sortKeys.data() + p, trail->numParticles);

				pixelpart::Curve<pixelpart::vec3d> sizeCurve(pixelpart::CurveInterpolation::spline);
				sizeCurve.enableFixedCache(particleType.trailRendererSettings.numTrailSegments);
				sizeCurve.setPointsOrdered<uint32_t>(relativeParticlePosition.data(), particles.size.data(), sortKeys.data() + p, trail->numParticles);

				pixelpart::Curve<pixelpart::vec4d> colorCurve(pixelpart::CurveInterpolation::spline);
				colorCurve.enableFixedCache(particleType.trailRendererSettings.numTrailSegments);
				colorCurve.setPointsOrdered<uint32_t>(relativeParticlePosition.data(), particles.color.data(), sortKeys.data() + p, trail->numParticles);

				pixelpart::Curve<pixelpart::vec3d> velocityCurve(pixelpart::CurveInterpolation::spline);
				velocityCurve.enableFixedCache(particleType.trailRendererSettings.numTrailSegments);
				velocityCurve.setPointsOrdered<uint32_t>(relativeParticlePosition.data(), particles.velocity.data(), sortKeys.data() + p, trail->numParticles);

				pixelpart::Curve<pixelpart::vec3d> forceCurve(pixelpart::CurveInterpolation::spline);
				forceCurve.enableFixedCache(particleType.trailRendererSettings.numTrailSegments);
				forceCurve.setPointsOrdered<uint32_t>(relativeParticlePosition.data(), particles.force.data(), sortKeys.data() + p, trail->numParticles);

				pixelpart::Curve<pixelpart::floatd> lifeCurve(pixelpart::CurveInterpolation::spline);
				lifeCurve.enableFixedCache(particleType.trailRendererSettings.numTrailSegments);
				lifeCurve.setPointsOrdered<uint32_t>(relativeParticlePosition.data(), particles.life.data(), sortKeys.data() + p, trail->numParticles);

				trail->position = positionCurve.getCache();
				trail->size = sizeCurve.getCache();
				trail->color = colorCurve.getCache();
				trail->velocity = velocityCurve.getCache();
				trail->force = forceCurve.getCache();
				trail->direction.resize(particleType.trailRendererSettings.numTrailSegments);
				trail->index.resize(particleType.trailRendererSettings.numTrailSegments);
				trail->life = lifeCurve.getCache();
			}
		}
	}
	else {
		for(uint32_t p = 0, q = 0; p < numParticles; p++, q++) {
			uint32_t particleIndex = sortKeys[p];
			particleParentId = particles.parentId[particleIndex];

			if(particleParentId != trailId) {
				trailId = particleParentId;
				q = 0;

				trail = &meshInstance.trails.at(trailId);
				trail->position.resize(trail->numParticles);
				trail->size.resize(trail->numParticles);
				trail->color.resize(trail->numParticles);
				trail->velocity.resize(trail->numParticles);
				trail->force.resize(trail->numParticles);
				trail->direction.resize(trail->numParticles);
				trail->index.resize(trail->numParticles);
				trail->life.resize(trail->numParticles);
			}

			trail->position[q] = particles.globalPosition[particleIndex];
			trail->size[q] = particles.size[particleIndex];
			trail->color[q] = particles.color[particleIndex];
			trail->velocity[q] = particles.velocity[particleIndex];
			trail->force[q] = particles.force[particleIndex];
			trail->life[q] = particles.life[particleIndex];
		}
	}

	VisualServer* vs = VisualServer::get_singleton();
	vs->immediate_begin(meshInstance.immediate, Mesh::PRIMITIVE_TRIANGLES);

	for(auto& entry : meshInstance.trails) {
		ParticleMeshInstance::ParticleTrail& trail = entry.second;
		trail.length = 0.0;

		if(trail.numParticles < 2) {
			continue;
		}

		std::size_t last = trail.numParticles - 1;
		for(std::size_t i = 0; i < last; i++) {
			pixelpart::vec3d toNext = trail.position[i + 1] - trail.position[i];
			pixelpart::floatd distance = std::max(glm::length(toNext), 0.000001);

			trail.direction[i] = toNext / distance;
			trail.index[i] = trail.length;
			trail.length += distance;
		}

		trail.direction[last] = trail.direction[last - 1];
		trail.index[last] = trail.length;

		trail.direction[0] = pixelpart::vec3d(-trail.direction[0].y, trail.direction[0].x, trail.direction[0].z);

		for(std::size_t i = last; i > 0; i--) {
			pixelpart::vec3d h = trail.direction[i] - trail.direction[i - 1];
			pixelpart::floatd l = glm::length(h);
			if(l < 0.0001) {
				trail.direction[i] = pixelpart::vec3d(
					-trail.direction[i].y,
					trail.direction[i].x,
					trail.direction[i].z);
			}
			else {
				trail.direction[i] = (glm::dot(glm::cross(trail.direction[i], pixelpart::vec3d(0.0, 0.0, 1.0)), h / l) < 0.0)
					? +h / l
					: -h / l;
			}
		}

		for(pixelpart::floatd& x : trail.index) {
			x /= trail.length;
		}

		for(uint32_t p = 0; p < trail.numParticles - 1; p++) {
			pixelpart::vec3d n0 = trail.direction[p] * std::max(trail.size[p].x, trail.size[p].y) * 0.5;
			pixelpart::vec3d n1 = trail.direction[p + 1] * std::max(trail.size[p + 1].x, trail.size[p + 1].y) * 0.5;
			pixelpart::vec3d p0 = (trail.position[p] + n0) * scale;
			pixelpart::vec3d p1 = (trail.position[p] - n0) * scale;
			pixelpart::vec3d p2 = (trail.position[p + 1] + n1) * scale;
			pixelpart::vec3d p3 = (trail.position[p + 1] - n1) * scale;
			pixelpart::vec2d t0, t1, t2, t3;

			switch(particleType.trailRendererSettings.textureRotation) {
				case 1:
					t0 = pixelpart::vec2d(trail.index[p] * particleType.trailRendererSettings.textureUVFactor, 1.0);
					t1 = pixelpart::vec2d(trail.index[p] * particleType.trailRendererSettings.textureUVFactor, 0.0);
					t2 = pixelpart::vec2d(trail.index[p + 1] * particleType.trailRendererSettings.textureUVFactor, 1.0);
					t3 = pixelpart::vec2d(trail.index[p + 1] * particleType.trailRendererSettings.textureUVFactor, 0.0);
					break;

				case 2:
					t0 = pixelpart::vec2d(1.0, 1.0 - trail.index[p] * particleType.trailRendererSettings.textureUVFactor);
					t1 = pixelpart::vec2d(0.0, 1.0 - trail.index[p] * particleType.trailRendererSettings.textureUVFactor);
					t2 = pixelpart::vec2d(1.0, 1.0 - trail.index[p + 1] * particleType.trailRendererSettings.textureUVFactor);
					t3 = pixelpart::vec2d(0.0, 1.0 - trail.index[p + 1] * particleType.trailRendererSettings.textureUVFactor);
					break;

				case 3: {
					t0 = pixelpart::vec2d(1.0 - trail.index[p] * particleType.trailRendererSettings.textureUVFactor, 0.0);
					t1 = pixelpart::vec2d(1.0 - trail.index[p] * particleType.trailRendererSettings.textureUVFactor, 1.0);
					t2 = pixelpart::vec2d(1.0 - trail.index[p + 1] * particleType.trailRendererSettings.textureUVFactor, 0.0);
					t3 = pixelpart::vec2d(1.0 - trail.index[p + 1] * particleType.trailRendererSettings.textureUVFactor, 1.0);
					break;
				}

				default: {
					t0 = pixelpart::vec2d(0.0, trail.index[p] * particleType.trailRendererSettings.textureUVFactor);
					t1 = pixelpart::vec2d(1.0, trail.index[p] * particleType.trailRendererSettings.textureUVFactor);
					t2 = pixelpart::vec2d(0.0, trail.index[p + 1] * particleType.trailRendererSettings.textureUVFactor);
					t3 = pixelpart::vec2d(1.0, trail.index[p + 1] * particleType.trailRendererSettings.textureUVFactor);
					break;
				}
			}

			vs->immediate_tangent(meshInstance.immediate, Plane(toGd(trail.velocity[p]), 0.0f));
			vs->immediate_color(meshInstance.immediate, toGd(trail.color[p] * 0.1 + pixelpart::vec4d(glm::floor(trail.force[p] * packFactor) + pixelpart::vec3d(packFactor), 0.0)));
			vs->immediate_uv2(meshInstance.immediate, Vector2(trail.life[p], static_cast<float>(entry.first)));
			vs->immediate_uv(meshInstance.immediate, toGd(t0));
			vs->immediate_vertex(meshInstance.immediate, toGd(p0));

			vs->immediate_tangent(meshInstance.immediate, Plane(toGd(trail.velocity[p + 1]), 0.0f));
			vs->immediate_color(meshInstance.immediate, toGd(trail.color[p + 1] * 0.1 + pixelpart::vec4d(glm::floor(trail.force[p + 1] * packFactor) + pixelpart::vec3d(packFactor), 0.0)));
			vs->immediate_uv2(meshInstance.immediate, Vector2(trail.life[p + 1], static_cast<float>(entry.first)));
			vs->immediate_uv(meshInstance.immediate, toGd(t2));
			vs->immediate_vertex(meshInstance.immediate, toGd(p2));

			vs->immediate_tangent(meshInstance.immediate, Plane(toGd(trail.velocity[p]), 0.0f));
			vs->immediate_color(meshInstance.immediate, toGd(trail.color[p] * 0.1 + pixelpart::vec4d(glm::floor(trail.force[p] * packFactor) + pixelpart::vec3d(packFactor), 0.0)));
			vs->immediate_uv2(meshInstance.immediate, Vector2(trail.life[p], static_cast<float>(entry.first)));
			vs->immediate_uv(meshInstance.immediate, toGd(t1));
			vs->immediate_vertex(meshInstance.immediate, toGd(p1));

			vs->immediate_tangent(meshInstance.immediate, Plane(toGd(trail.velocity[p + 1]), 0.0f));
			vs->immediate_color(meshInstance.immediate, toGd(trail.color[p + 1] * 0.1 + pixelpart::vec4d(glm::floor(trail.force[p + 1] * packFactor) + pixelpart::vec3d(packFactor), 0.0)));
			vs->immediate_uv2(meshInstance.immediate, Vector2(trail.life[p + 1], static_cast<float>(entry.first)));
			vs->immediate_uv(meshInstance.immediate, toGd(t2));
			vs->immediate_vertex(meshInstance.immediate, toGd(p2));
			vs->immediate_uv(meshInstance.immediate, toGd(t3));
			vs->immediate_vertex(meshInstance.immediate, toGd(p3));

			vs->immediate_tangent(meshInstance.immediate, Plane(toGd(trail.velocity[p]), 0.0f));
			vs->immediate_color(meshInstance.immediate, toGd(trail.color[p] * 0.1 + pixelpart::vec4d(glm::floor(trail.force[p] * packFactor) + pixelpart::vec3d(packFactor), 0.0)));
			vs->immediate_uv2(meshInstance.immediate, Vector2(trail.life[p], static_cast<float>(entry.first)));
			vs->immediate_uv(meshInstance.immediate, toGd(t1));
			vs->immediate_vertex(meshInstance.immediate, toGd(p1));
		}
	}

	vs->immediate_end(meshInstance.immediate);
}

pixelpart::mat3d PixelpartEffect::rotation3d(const pixelpart::vec3d& angle) {
	pixelpart::vec3d rotation = glm::radians(angle);
	pixelpart::floatd cy = std::cos(rotation.y);
	pixelpart::floatd sy = std::sin(rotation.y);
	pixelpart::floatd cp = std::cos(rotation.z);
	pixelpart::floatd sp = std::sin(rotation.z);
	pixelpart::floatd cr = std::cos(rotation.x);
	pixelpart::floatd sr = std::sin(rotation.x);

	return pixelpart::mat3d(
		pixelpart::vec3d(cy * cr + sy * sp * sr, sr * cp, -sy * cr + cy * sp * sr),
		pixelpart::vec3d(-cy * sr + sy * sp * cr, cr * cp, sr * sy + cy * sp * cr),
		pixelpart::vec3d(sy * cp, -sp, cy * cp));
}
pixelpart::mat3d PixelpartEffect::lookAt(const pixelpart::vec3d& direction) {
	pixelpart::vec3d up = pixelpart::vec3d(0.0, 1.0, 0.0);
	pixelpart::vec3d front = glm::normalize(direction);
	pixelpart::vec3d right = glm::normalize(glm::cross(front, up));
	up = glm::normalize(glm::cross(right, front));

	return pixelpart::mat3d(right, up, front);
}
}