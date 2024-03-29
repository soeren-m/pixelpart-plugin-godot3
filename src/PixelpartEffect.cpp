#include "PixelpartEffect.h"
#include "PixelpartShaders.h"
#include "PixelpartUtil.h"
#include "ParticleSolverCPU.h"
#include <Engine.hpp>
#include <VisualServer.hpp>
#include <ProjectSettings.hpp>
#include <World.hpp>
#include <Viewport.hpp>
#include <Camera.hpp>

namespace godot {
void PixelpartEffect::_register_methods() {
	register_property<PixelpartEffect, Ref<PixelpartEffectResource>>("effect",
		&PixelpartEffect::set_effect, &PixelpartEffect::get_effect, nullptr,
		GODOT_METHOD_RPC_MODE_DISABLED,
		GODOT_PROPERTY_USAGE_DEFAULT,
		GODOT_PROPERTY_HINT_RESOURCE_TYPE);

	register_property<PixelpartEffect, String>("Playback",
		nullptr, "",
		GODOT_METHOD_RPC_MODE_DISABLED,
		GODOT_PROPERTY_USAGE_GROUP,
		GODOT_PROPERTY_HINT_NONE);
	register_property<PixelpartEffect, bool>("playing",
		&PixelpartEffect::play, &PixelpartEffect::is_playing, true);
	register_property<PixelpartEffect, bool>("loop",
		&PixelpartEffect::set_loop, &PixelpartEffect::get_loop, false);
	register_property<PixelpartEffect, float>("loop_time",
		&PixelpartEffect::set_loop_time, &PixelpartEffect::get_loop_time, 1.0f,
		GODOT_METHOD_RPC_MODE_DISABLED,
		GODOT_PROPERTY_USAGE_DEFAULT,
		GODOT_PROPERTY_HINT_EXP_RANGE, "0.0,1000.0,0.01");
	register_property<PixelpartEffect, float>("speed",
		&PixelpartEffect::set_speed, &PixelpartEffect::get_speed, 1.0f,
		GODOT_METHOD_RPC_MODE_DISABLED,
		GODOT_PROPERTY_USAGE_DEFAULT,
		GODOT_PROPERTY_HINT_EXP_RANGE, "0.0,100.0,0.01");
	register_property<PixelpartEffect, float>("frame_rate",
		&PixelpartEffect::set_frame_rate, &PixelpartEffect::get_frame_rate, 60.0f,
		GODOT_METHOD_RPC_MODE_DISABLED,
		GODOT_PROPERTY_USAGE_DEFAULT,
		GODOT_PROPERTY_HINT_EXP_RANGE, "1.0,100.0,1.0");

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

}

void PixelpartEffect::_init() {
	simulationTime = 0.0f;

	playing = true;
	loop = false;
	loopTime = 1.0f;
	speed = 1.0f;
	timeStep = 1.0f / 60.0f;
}
void PixelpartEffect::_enter_tree() {
	VisualServer::get_singleton()->connect("frame_pre_draw", this, "draw");
}
void PixelpartEffect::_exit_tree() {
	VisualServer::get_singleton()->disconnect("frame_pre_draw", this, "draw");
}

void PixelpartEffect::_process(float dt) {
	if(!particleEngine || !particleEngine->getEffect()) {
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
	if(!particleEngine) {
		return;
	}

	const pixelpart::Effect* effect = particleEngine->getEffect();
	if(!effect) {
		return;
	}

	Viewport* viewport = get_viewport();
	if(!viewport) {
		return;
	}

	for(uint32_t particleTypeIndex = 0u; particleTypeIndex < effect->particleTypes.getCount(); particleTypeIndex++) {
		if(particleTypeIndex >= particleMeshInstances.size()) {
			return;
		}

		draw_particles(particleTypeIndex);
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

float PixelpartEffect::get_import_scale() const {
	if(!effectResource.is_valid()) {
		return 1.0f;
	}

	return effectResource->get_scale();
}

void PixelpartEffect::set_effect(Ref<PixelpartEffectResource> effectRes) {
	VisualServer* vs = VisualServer::get_singleton();
	PixelpartShaders* shaders = PixelpartShaders::get_instance();

	particleMeshInstances.clear();
	particleEmitters.clear();
	particleTypes.clear();
	forceFields.clear();
	colliders.clear();
	textures.clear();

	effectResource = effectRes;
	if(effectResource.is_null()) {
		particleEngine->setEffect(nullptr);
		property_list_changed_notify();

		return;
	}

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

		for(uint32_t particleTypeIndex = 0u; particleTypeIndex < nativeEffect.particleTypes.getCount(); particleTypeIndex++) {
			particleMeshInstances.emplace_back(std::make_unique<ParticleMeshInstance>(
				nativeEffect.particleTypes.getByIndex(particleTypeIndex)));
		}

		for(const auto& resource : effectResource->get_project_resources().images) {
			PoolByteArray imageData;
			imageData.resize(static_cast<int64_t>(resource.second.data.size()));

			std::memcpy(imageData.write().ptr(), resource.second.data.data(), resource.second.data.size());

			Ref<Image> image;
			image.instance();
			image->create_from_data(
				static_cast<int32_t>(resource.second.width),
				static_cast<int32_t>(resource.second.height),
				false, Image::FORMAT_RGBA8, imageData);

			Ref<ImageTexture> imageTexture;
			imageTexture.instance();
			imageTexture->create_from_image(image);
			textures[resource.first] =  imageTexture;
		}
	}
	catch(std::exception& e) {
		particleEngine->setEffect(nullptr);

		Godot::print_error(String(e.what()), __FUNCTION__, "PixelpartEffect.cpp", __LINE__);
	}

	property_list_changed_notify();
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

PixelpartEffect::ParticleMeshInstance::ParticleMeshInstance(const pixelpart::ParticleType& particleType) {
	VisualServer* vs = VisualServer::get_singleton();

	mesh.instance();
	shaderMaterial.instance();

	instanceRID = vs->instance_create();
	vs->instance_set_base(instanceRID, mesh->get_rid());

	particleType.shader.build(shaderBuildResult);
	update_shader(particleType);
}
PixelpartEffect::ParticleMeshInstance::~ParticleMeshInstance() {
	VisualServer* vs = VisualServer::get_singleton();

	vs->free_rid(instanceRID);
}

void PixelpartEffect::ParticleMeshInstance::update_shader(const pixelpart::ParticleType& particleType) {
	shader = PixelpartShaders::get_instance()->get_spatial_shader(shaderBuildResult.code,
		particleType.blendMode,
		true, false,
		SpatialMaterial::DIFFUSE_BURLEY,
		SpatialMaterial::SPECULAR_SCHLICK_GGX);

	shaderMaterial->set_shader(shader);
}

Ref<Shader> PixelpartEffect::ParticleMeshInstance::get_shader() const {
	return shader;
}
Ref<ShaderMaterial> PixelpartEffect::ParticleMeshInstance::get_shader_material() const {
	return shaderMaterial;
}
Ref<ArrayMesh> PixelpartEffect::ParticleMeshInstance::get_mesh() const {
	return mesh;
}
RID PixelpartEffect::ParticleMeshInstance::get_instance_rid() const {
	return instanceRID;
}

std::string PixelpartEffect::ParticleMeshInstance::get_texture_id(std::size_t index) const {
	return index < shaderBuildResult.textureIds.size()
		? shaderBuildResult.textureIds.at(index)
		: "";
}
std::size_t PixelpartEffect::ParticleMeshInstance::get_texture_count() const {
	return shaderBuildResult.textureIds.size();
}

pixelpart::ParticleData& PixelpartEffect::ParticleMeshInstance::get_sorted_particle_data() {
	return sortedParticleData;
}
std::vector<uint32_t>& PixelpartEffect::ParticleMeshInstance::get_sort_keys() {
	return sortKeys;
}

std::unordered_map<uint32_t, PixelpartEffect::ParticleMeshInstance::ParticleTrail>& PixelpartEffect::ParticleMeshInstance::get_trails() {
	return trails;
}

void PixelpartEffect::draw_particles(uint32_t particleTypeIndex) {
	const pixelpart::Effect* effect = particleEngine->getEffect();
	const pixelpart::ParticleType& particleType = effect->particleTypes.getByIndex(particleTypeIndex);
	const pixelpart::ParticleEmitter& particleEmitter = effect->particleEmitters.get(particleType.parentId);
	pixelpart::floatd alpha = std::fmod(particleEngine->getTime() - particleEmitter.lifetimeStart, particleEmitter.lifetimeDuration) / particleEmitter.lifetimeDuration;

	ParticleMeshInstance& meshInstance = *(particleMeshInstances.at(particleTypeIndex));
	meshInstance.get_mesh()->clear_surfaces();

	if(!is_visible() || !particleType.visible) {
		return;
	}

	const pixelpart::ParticleData& particles = particleEngine->getParticles(particleTypeIndex);
	uint32_t numParticles = particleEngine->getNumParticles(particleTypeIndex);
	pixelpart::floatd scale = static_cast<pixelpart::floatd>(get_import_scale());

	Ref<ShaderMaterial> shaderMaterial = meshInstance.get_shader_material();
	shaderMaterial->set_shader_param("u_EffectTime", static_cast<float>(particleEngine->getTime()));
	shaderMaterial->set_shader_param("u_ObjectTime", static_cast<float>(particleEngine->getTime() - particleEmitter.lifetimeStart));

	for(std::size_t t = 0; t < meshInstance.get_texture_count(); t++) {
		String samplerName = String("TEXTURE") + String::num_int64(t);
		Ref<ImageTexture> texture = textures.at(meshInstance.get_texture_id(t));

		shaderMaterial->set_shader_param(samplerName, texture);
	}

	Transform particleMeshTransform = get_global_transform();
	particleMeshTransform.translate(toGd(particleEmitter.position.get(alpha) * scale));

	VisualServer* vs = VisualServer::get_singleton();
	vs->instance_set_scenario(meshInstance.get_instance_rid(), get_world()->get_scenario());
	vs->instance_set_transform(meshInstance.get_instance_rid(), particleMeshTransform);

	add_particle_mesh(meshInstance,
		particleType,
		particles,
		numParticles,
		scale);

	if(meshInstance.get_mesh()->get_surface_count() > 0) {
		meshInstance.get_mesh()->surface_set_material(0, shaderMaterial);
	}
}

void PixelpartEffect::add_particle_mesh(ParticleMeshInstance& meshInstance, const pixelpart::ParticleType& particleType, const pixelpart::ParticleData& particles, uint32_t numParticles, pixelpart::floatd scale) {
	switch(particleType.renderer) {
		case pixelpart::ParticleType::Renderer::sprite:
			add_particle_sprites(meshInstance, particleType, particles, numParticles, scale);
			break;
		case pixelpart::ParticleType::Renderer::trail:
			add_particle_trails(meshInstance, particleType, particles, numParticles, scale);
			break;
	}
}

void PixelpartEffect::add_particle_sprites(ParticleMeshInstance& meshInstance, const pixelpart::ParticleType& particleType, const pixelpart::ParticleData& particles, uint32_t numParticles, pixelpart::floatd scale) {
	const pixelpart::floatd packFactor = 10000.0;

	if(numParticles == 0u) {
		return;
	}

	Camera* camera = get_viewport()->get_camera();
	if(!camera) {
		return;
	}

	const pixelpart::ParticleEmitter& particleEmitter = nativeEffect.particleEmitters.get(particleType.parentId);
	pixelpart::floatd alpha = std::fmod(particleEngine->getTime() - particleEmitter.lifetimeStart, particleEmitter.lifetimeDuration) / particleEmitter.lifetimeDuration;

	const pixelpart::ParticleData* particleRenderData = &particles;

	if(particleType.spriteRendererSettings.sortCriterion != pixelpart::ParticleSortCriterion::none && numParticles > 1) {
		pixelpart::ParticleData& sortedParticleData = meshInstance.get_sorted_particle_data();
		std::vector<uint32_t>& sortKeys = meshInstance.get_sort_keys();

		sortedParticleData.resize(particles.id.size());
		sortKeys.resize(particles.id.size());
		std::iota(sortKeys.begin(), sortKeys.begin() + numParticles, 0);

		switch(particleType.spriteRendererSettings.sortCriterion) {
			case pixelpart::ParticleSortCriterion::age: {
				insertionSort(sortKeys.begin(), sortKeys.begin() + numParticles,
					[&particles](uint32_t i, uint32_t j) {
						return particles.id[i] < particles.id[j];
					});

				break;
			}

			case pixelpart::ParticleSortCriterion::distance: {
				pixelpart::vec3d cameraPosition = fromGd(camera->get_global_transform().origin);

				insertionSort(sortKeys.begin(), sortKeys.begin() + numParticles,
					[&particles, cameraPosition](uint32_t i, uint32_t j) {
						return glm::distance2(particles.globalPosition[i], cameraPosition) > glm::distance2(particles.globalPosition[j], cameraPosition);
					});

				break;
			}
		}

		for(uint32_t i = 0; i < numParticles; i++) {
			uint32_t j = sortKeys[i];

			sortedParticleData.id[i] = particles.id[j];
			sortedParticleData.life[i] = particles.life[j];
			sortedParticleData.globalPosition[i] = particles.globalPosition[j];
			sortedParticleData.velocity[i] = particles.velocity[j];
			sortedParticleData.force[i] = particles.force[j];
			sortedParticleData.rotation[i] = particles.rotation[j];
			sortedParticleData.size[i] = particles.size[j];
			sortedParticleData.color[i] = particles.color[j];
		}

		particleRenderData = &sortedParticleData;
	}

	pixelpart::vec3d cameraPosition = fromGd(camera->get_global_transform().origin);
	pixelpart::vec3d cameraRight = fromGd(camera->get_global_transform().basis.x);
	pixelpart::vec3d cameraUp = fromGd(camera->get_global_transform().basis.y);

	Ref<ArrayMesh> mesh = meshInstance.get_mesh();

	PoolIntArray indexArray;
	PoolVector3Array vertexArray;
	PoolVector3Array normalArray;
	PoolRealArray tangentArray;
	PoolVector2Array uvArray;
	PoolVector2Array uv2Array;
	PoolColorArray colorArray;
	indexArray.resize(numParticles * 6);
	vertexArray.resize(numParticles * 4);
	normalArray.resize(numParticles * 4);
	tangentArray.resize(numParticles * 4 * 4);
	uvArray.resize(numParticles * 4);
	uv2Array.resize(numParticles * 4);
	colorArray.resize(numParticles * 4);

	int32_t* indices = indexArray.write().ptr();
	Vector3* positions = vertexArray.write().ptr();
	Vector3* normals = normalArray.write().ptr();
	float* tangents = tangentArray.write().ptr();
	Color* colors = colorArray.write().ptr();
	float* uvs = reinterpret_cast<float*>(uvArray.write().ptr());
	float* uvs2 = reinterpret_cast<float*>(uv2Array.write().ptr());

	for(int32_t p = 0; p < static_cast<int32_t>(numParticles); p++) {
		indices[p * 6 + 0] = p * 4 + 0;
		indices[p * 6 + 1] = p * 4 + 1;
		indices[p * 6 + 2] = p * 4 + 3;
		indices[p * 6 + 3] = p * 4 + 1;
		indices[p * 6 + 4] = p * 4 + 2;
		indices[p * 6 + 5] = p * 4 + 3;
	}

	for(uint32_t p = 0; p < numParticles; p++) {
		pixelpart::mat3d rotationMatrix = rotation3d(particleRenderData->rotation[p]);
		pixelpart::vec3d pivot = particleType.pivot * particleRenderData->size[p];
		pixelpart::vec3d position[4] = {
			rotationMatrix * (pixelpart::vec3d(-0.5, -0.5, 0.0) * particleRenderData->size[p] - pivot) + pivot,
			rotationMatrix * (pixelpart::vec3d(-0.5, +0.5, 0.0) * particleRenderData->size[p] - pivot) + pivot,
			rotationMatrix * (pixelpart::vec3d(+0.5, +0.5, 0.0) * particleRenderData->size[p] - pivot) + pivot,
			rotationMatrix * (pixelpart::vec3d(+0.5, -0.5, 0.0) * particleRenderData->size[p] - pivot) + pivot };
		pixelpart::vec3d normal = rotationMatrix * pixelpart::vec3d(0.0, 0.0, 1.0);

		switch(particleType.alignmentMode) {
			case pixelpart::AlignmentMode::camera: {
				position[0] = particleRenderData->globalPosition[p] + cameraRight * position[0].x + cameraUp * position[0].y;
				position[1] = particleRenderData->globalPosition[p] + cameraRight * position[1].x + cameraUp * position[1].y;
				position[2] = particleRenderData->globalPosition[p] + cameraRight * position[2].x + cameraUp * position[2].y;
				position[3] = particleRenderData->globalPosition[p] + cameraRight * position[3].x + cameraUp * position[3].y;
				normal = cameraPosition - particleRenderData->globalPosition[p];
				break;
			}
			case pixelpart::AlignmentMode::motion: {
				pixelpart::mat3d lookAtMatrix = lookAt(particleRenderData->velocity[p]);
				position[0] = particleRenderData->globalPosition[p] + lookAtMatrix * position[0];
				position[1] = particleRenderData->globalPosition[p] + lookAtMatrix * position[1];
				position[2] = particleRenderData->globalPosition[p] + lookAtMatrix * position[2];
				position[3] = particleRenderData->globalPosition[p] + lookAtMatrix * position[3];
				normal = lookAtMatrix * normal;
				break;
			}
			case pixelpart::AlignmentMode::emission: {
				pixelpart::mat3d lookAtMatrix = lookAt(particleEmitter.position.get(alpha) - particleRenderData->globalPosition[p]);
				position[0] = particleRenderData->globalPosition[p] + lookAtMatrix * position[0];
				position[1] = particleRenderData->globalPosition[p] + lookAtMatrix * position[1];
				position[2] = particleRenderData->globalPosition[p] + lookAtMatrix * position[2];
				position[3] = particleRenderData->globalPosition[p] + lookAtMatrix * position[3];
				normal = lookAtMatrix * normal;
				break;
			}
			case pixelpart::AlignmentMode::emitter: {
				pixelpart::mat3d lookAtMatrix = rotation3d(particleEmitter.orientation.get(alpha));
				position[0] = particleRenderData->globalPosition[p] + lookAtMatrix * position[0];
				position[1] = particleRenderData->globalPosition[p] + lookAtMatrix * position[1];
				position[2] = particleRenderData->globalPosition[p] + lookAtMatrix * position[2];
				position[3] = particleRenderData->globalPosition[p] + lookAtMatrix * position[3];
				normal = lookAtMatrix * normal;
				break;
			}
			default: {
				position[0] = particleRenderData->globalPosition[p] + position[0];
				position[1] = particleRenderData->globalPosition[p] + position[1];
				position[2] = particleRenderData->globalPosition[p] + position[2];
				position[3] = particleRenderData->globalPosition[p] + position[3];
				break;
			}
		}

		positions[p * 4 + 0] = toGd((position[0] - particleEmitter.position.get(alpha)) * scale);
		positions[p * 4 + 1] = toGd((position[1] - particleEmitter.position.get(alpha)) * scale);
		positions[p * 4 + 2] = toGd((position[2] - particleEmitter.position.get(alpha)) * scale);
		positions[p * 4 + 3] = toGd((position[3] - particleEmitter.position.get(alpha)) * scale);
		normals[p * 4 + 0] = toGd(normal);
		normals[p * 4 + 1] = toGd(normal);
		normals[p * 4 + 2] = toGd(normal);
		normals[p * 4 + 3] = toGd(normal);
	}

	for(uint32_t p = 0; p < numParticles; p++) {
		Vector3 velocity = toGd(particleRenderData->velocity[p]);

		tangents[p * 4 * 4 + 0] = velocity.x;
		tangents[p * 4 * 4 + 1] = velocity.y;
		tangents[p * 4 * 4 + 2] = velocity.z;
		tangents[p * 4 * 4 + 3] = 0.0f;
		tangents[p * 4 * 4 + 4] = velocity.x;
		tangents[p * 4 * 4 + 5] = velocity.y;
		tangents[p * 4 * 4 + 6] = velocity.z;
		tangents[p * 4 * 4 + 7] = 0.0f;
		tangents[p * 4 * 4 + 8] = velocity.x;
		tangents[p * 4 * 4 + 9] = velocity.y;
		tangents[p * 4 * 4 + 10] = velocity.z;
		tangents[p * 4 * 4 + 11] = 0.0f;
		tangents[p * 4 * 4 + 12] = velocity.x;
		tangents[p * 4 * 4 + 13] = velocity.y;
		tangents[p * 4 * 4 + 14] = velocity.z;
		tangents[p * 4 * 4 + 15] = 0.0f;
	}

	for(uint32_t p = 0; p < numParticles; p++) {
		Color packedColor = toGd(particleRenderData->color[p] * 0.1 +
			pixelpart::vec4d(glm::floor(particleRenderData->force[p] * packFactor) + pixelpart::vec3d(packFactor), packFactor));

		colors[p * 4 + 0] = packedColor;
		colors[p * 4 + 1] = packedColor;
		colors[p * 4 + 2] = packedColor;
		colors[p * 4 + 3] = packedColor;
	}

	for(uint32_t p = 0; p < numParticles; p++) {
		uvs[p * 4 * 2 + 0] = 0.0f;
		uvs[p * 4 * 2 + 1] = 0.0f;
		uvs[p * 4 * 2 + 2] = 0.0f;
		uvs[p * 4 * 2 + 3] = 1.0f;
		uvs[p * 4 * 2 + 4] = 1.0f;
		uvs[p * 4 * 2 + 5] = 1.0f;
		uvs[p * 4 * 2 + 6] = 1.0f;
		uvs[p * 4 * 2 + 7] = 0.0f;
	}

	for(uint32_t p = 0; p < numParticles; p++) {
		uvs2[p * 4 * 2 + 0] = static_cast<float>(particleRenderData->life[p]);
		uvs2[p * 4 * 2 + 1] = static_cast<float>(particleRenderData->id[p]);
		uvs2[p * 4 * 2 + 2] = static_cast<float>(particleRenderData->life[p]);
		uvs2[p * 4 * 2 + 3] = static_cast<float>(particleRenderData->id[p]);
		uvs2[p * 4 * 2 + 4] = static_cast<float>(particleRenderData->life[p]);
		uvs2[p * 4 * 2 + 5] = static_cast<float>(particleRenderData->id[p]);
		uvs2[p * 4 * 2 + 6] = static_cast<float>(particleRenderData->life[p]);
		uvs2[p * 4 * 2 + 7] = static_cast<float>(particleRenderData->id[p]);
	}

	Array meshArray;
	meshArray.resize(Mesh::ARRAY_MAX);
	meshArray[Mesh::ARRAY_VERTEX] = vertexArray;
	meshArray[Mesh::ARRAY_NORMAL] = normalArray;
	meshArray[Mesh::ARRAY_TANGENT] = tangentArray;
	meshArray[Mesh::ARRAY_COLOR] = colorArray;
	meshArray[Mesh::ARRAY_TEX_UV] = uvArray;
	meshArray[Mesh::ARRAY_TEX_UV2] = uv2Array;
	meshArray[Mesh::ARRAY_INDEX] = indexArray;

	mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, meshArray, Array(),
		Mesh::ARRAY_FORMAT_VERTEX | Mesh::ARRAY_FORMAT_NORMAL | Mesh::ARRAY_FORMAT_TANGENT |
		Mesh::ARRAY_FORMAT_COLOR | Mesh::ARRAY_FORMAT_TEX_UV | Mesh::ARRAY_FORMAT_TEX_UV2 |
		Mesh::ARRAY_FORMAT_INDEX);
}

void PixelpartEffect::add_particle_trails(ParticleMeshInstance& meshInstance, const pixelpart::ParticleType& particleType, const pixelpart::ParticleData& particles, uint32_t numParticles, pixelpart::floatd scale) {
	const pixelpart::floatd epsilon = 0.000001;
	const pixelpart::floatd packFactor = 10000.0;

	if(numParticles < 2u) {
		return;
	}

	const pixelpart::ParticleEmitter& particleEmitter = nativeEffect.particleEmitters.get(particleType.parentId);
	pixelpart::floatd alpha = std::fmod(particleEngine->getTime() - particleEmitter.lifetimeStart, particleEmitter.lifetimeDuration) / particleEmitter.lifetimeDuration;

	std::vector<uint32_t> sortKeys(numParticles);
	std::iota(sortKeys.begin(), sortKeys.end(), 0u);
	std::sort(sortKeys.begin(), sortKeys.end(), [&particles](uint32_t p0, uint32_t p1) {
		return (particles.parentId[p0] != particles.parentId[p1])
			? particles.parentId[p0] > particles.parentId[p1]
			: particles.id[p0] > particles.id[p1];
	});

	std::unordered_map<uint32_t, ParticleMeshInstance::ParticleTrail>& trails = meshInstance.get_trails();

	for(auto& entry : trails) {
		entry.second.numParticles = 0u;
	}

	ParticleMeshInstance::ParticleTrail* currentTrail = nullptr;
	for(uint32_t p = 0u, particleParentId = 0u, trailId = 0xFFFFFFFEu; p < numParticles; p++) {
		particleParentId = particles.parentId[sortKeys[p]];

		if(particleParentId != trailId) {
			trailId = particleParentId;
			currentTrail = &trails[trailId];
		}

		currentTrail->numParticles++;
	}

	for(auto it = trails.begin(); it != trails.end(); ) {
		if(it->second.numParticles == 0u) {
			it = trails.erase(it);
		}
		else {
			it++;
		}
	}

	currentTrail = nullptr;

	if(particleType.trailRendererSettings.numTrailSegments > 0u) {
		for(uint32_t p = 0u, particleParentId = 0u, trailId = 0xFFFFFFFEu; p < numParticles; p++) {
			uint32_t particleIndex = sortKeys[p];
			particleParentId = particles.parentId[particleIndex];

			if(particleParentId != trailId) {
				trailId = particleParentId;
				currentTrail = &trails.at(trailId);

				std::vector<pixelpart::floatd> relativeParticlePosition(currentTrail->numParticles);
				relativeParticlePosition[0u] = 0.0;

				currentTrail->length = 0.0;
				for(uint32_t q = 1u; q < currentTrail->numParticles; q++) {
					currentTrail->length += glm::length(particles.globalPosition[sortKeys[p + q]] - particles.globalPosition[sortKeys[p + q - 1u]]);
					relativeParticlePosition[q] = currentTrail->length;
				}
				for(uint32_t q = 1u; q < currentTrail->numParticles; q++) {
					relativeParticlePosition[q] /= currentTrail->length;
				}

				pixelpart::Curve<pixelpart::vec3d> positionCurve(pixelpart::CurveInterpolation::spline);
				positionCurve.enableFixedCache(particleType.trailRendererSettings.numTrailSegments);
				positionCurve.setPointsOrdered<uint32_t>(relativeParticlePosition.data(), particles.globalPosition.data(), sortKeys.data() + p, currentTrail->numParticles);

				pixelpart::Curve<pixelpart::vec3d> sizeCurve(pixelpart::CurveInterpolation::spline);
				sizeCurve.enableFixedCache(particleType.trailRendererSettings.numTrailSegments);
				sizeCurve.setPointsOrdered<uint32_t>(relativeParticlePosition.data(), particles.size.data(), sortKeys.data() + p, currentTrail->numParticles);

				pixelpart::Curve<pixelpart::vec4d> colorCurve(pixelpart::CurveInterpolation::spline);
				colorCurve.enableFixedCache(particleType.trailRendererSettings.numTrailSegments);
				colorCurve.setPointsOrdered<uint32_t>(relativeParticlePosition.data(), particles.color.data(), sortKeys.data() + p, currentTrail->numParticles);

				pixelpart::Curve<pixelpart::vec3d> velocityCurve(pixelpart::CurveInterpolation::spline);
				velocityCurve.enableFixedCache(particleType.trailRendererSettings.numTrailSegments);
				velocityCurve.setPointsOrdered<uint32_t>(relativeParticlePosition.data(), particles.velocity.data(), sortKeys.data() + p, currentTrail->numParticles);

				pixelpart::Curve<pixelpart::vec3d> forceCurve(pixelpart::CurveInterpolation::spline);
				forceCurve.enableFixedCache(particleType.trailRendererSettings.numTrailSegments);
				forceCurve.setPointsOrdered<uint32_t>(relativeParticlePosition.data(), particles.force.data(), sortKeys.data() + p, currentTrail->numParticles);

				pixelpart::Curve<pixelpart::floatd> lifeCurve(pixelpart::CurveInterpolation::spline);
				lifeCurve.enableFixedCache(particleType.trailRendererSettings.numTrailSegments);
				lifeCurve.setPointsOrdered<uint32_t>(relativeParticlePosition.data(), particles.life.data(), sortKeys.data() + p, currentTrail->numParticles);

				currentTrail->position = positionCurve.getCache();
				currentTrail->size = sizeCurve.getCache();
				currentTrail->color = colorCurve.getCache();
				currentTrail->velocity = velocityCurve.getCache();
				currentTrail->force = forceCurve.getCache();
				currentTrail->life = lifeCurve.getCache();
				currentTrail->direction.resize(particleType.trailRendererSettings.numTrailSegments);
				currentTrail->directionToEdge.resize(particleType.trailRendererSettings.numTrailSegments);
				currentTrail->index.resize(particleType.trailRendererSettings.numTrailSegments);
			}
		}
	}
	else {
		for(uint32_t p = 0u, q = 0u, particleParentId = 0u, trailId = 0xFFFFFFFEu; p < numParticles; p++, q++) {
			uint32_t particleIndex = sortKeys[p];
			particleParentId = particles.parentId[particleIndex];

			if(particleParentId != trailId) {
				trailId = particleParentId;
				q = 0u;

				currentTrail = &trails.at(trailId);
				currentTrail->position.resize(currentTrail->numParticles);
				currentTrail->size.resize(currentTrail->numParticles);
				currentTrail->color.resize(currentTrail->numParticles);
				currentTrail->velocity.resize(currentTrail->numParticles);
				currentTrail->force.resize(currentTrail->numParticles);
				currentTrail->life.resize(currentTrail->numParticles);
				currentTrail->direction.resize(currentTrail->numParticles);
				currentTrail->directionToEdge.resize(currentTrail->numParticles);
				currentTrail->index.resize(currentTrail->numParticles);
			}

			currentTrail->position[q] = particles.globalPosition[particleIndex];
			currentTrail->size[q] = particles.size[particleIndex];
			currentTrail->color[q] = particles.color[particleIndex];
			currentTrail->velocity[q] = particles.velocity[particleIndex];
			currentTrail->force[q] = particles.force[particleIndex];
			currentTrail->life[q] = particles.life[particleIndex];
		}
	}

	int64_t numTotalTrailSegments = 0;
	for(auto& entry : trails) {
		ParticleMeshInstance::ParticleTrail& trail = entry.second;
		if(trail.position.size() < 2u) {
			continue;
		}

		numTotalTrailSegments += static_cast<int64_t>(trail.position.size()) - 1;
	}

	if(numTotalTrailSegments < 1) {
		return;
	}

	Ref<ArrayMesh> mesh = meshInstance.get_mesh();

	PoolIntArray indexArray;
	PoolVector3Array vertexArray;
	PoolVector3Array normalArray;
	PoolRealArray tangentArray;
	PoolVector2Array uvArray;
	PoolVector2Array uv2Array;
	PoolColorArray colorArray;
	indexArray.resize(numTotalTrailSegments * 12);
	vertexArray.resize(numTotalTrailSegments * 5);
	normalArray.resize(numTotalTrailSegments * 5);
	tangentArray.resize(numTotalTrailSegments * 5 * 4);
	uvArray.resize(numTotalTrailSegments * 5);
	uv2Array.resize(numTotalTrailSegments * 5);
	colorArray.resize(numTotalTrailSegments * 5);

	int64_t vertexIndex = 0;
	int32_t* indices = indexArray.write().ptr();
	Vector3* positions = vertexArray.write().ptr();
	Vector3* normals = normalArray.write().ptr();
	float* tangents = tangentArray.write().ptr();
	Color* colors = colorArray.write().ptr();
	Vector2* uvs = uvArray.write().ptr();
	Vector2* uvs2 = uv2Array.write().ptr();

	for(auto& entry : trails) {
		ParticleMeshInstance::ParticleTrail& trail = entry.second;
		trail.length = 0.0;

		if(trail.position.size() < 2u) {
			continue;
		}

		std::size_t last = trail.position.size() - 1u;
		for(std::size_t i = 0u; i < last; i++) {
			pixelpart::vec3d toNext = trail.position[i + 1u] - trail.position[i];
			pixelpart::floatd distance = std::max(glm::length(toNext), epsilon);

			trail.direction[i] = toNext / distance;
			trail.index[i] = trail.length;
			trail.length += distance;
		}

		trail.direction[last] = trail.direction[last - 1u];
		trail.index[last] = trail.length;

		for(std::size_t i = last; i > 0u; i--) {
			pixelpart::vec3d toEdge = trail.direction[i] - trail.direction[i - 1u];
			pixelpart::floatd toEdgeLength = glm::length(toEdge);
			if(toEdgeLength < epsilon) {
				trail.directionToEdge[i] = pixelpart::vec3d(
					-trail.direction[i].y,
					trail.direction[i].x,
					trail.direction[i].z);
			}
			else {
				trail.directionToEdge[i] = (glm::dot(glm::cross(trail.direction[i], pixelpart::vec3d(0.0, 0.0, 1.0)), toEdge / toEdgeLength) < 0.0)
					? +toEdge / toEdgeLength
					: -toEdge / toEdgeLength;
			}
		}

		trail.directionToEdge[0u] = pixelpart::vec3d(-trail.direction[0u].y, trail.direction[0u].x, trail.direction[0u].z);

		for(pixelpart::floatd& index : trail.index) {
			index /= trail.length;
		}

		for(uint32_t p = 0u; p < static_cast<uint32_t>(trail.position.size()) - 1u; p++) {
			pixelpart::vec3d startToEdge = trail.directionToEdge[p] * std::max(trail.size[p].x, trail.size[p].y) * 0.5;
			pixelpart::vec3d endToEdge = trail.directionToEdge[p + 1u] * std::max(trail.size[p + 1u].x, trail.size[p + 1u].y) * 0.5;

			pixelpart::vec2d vertexTextureCoords[5];
			pixelpart::vec3d vertexPositions[5] = {
				(trail.position[p] + startToEdge - particleEmitter.position.get(alpha)) * scale,
				(trail.position[p] - startToEdge - particleEmitter.position.get(alpha)) * scale,
				(trail.position[p + 1u] + endToEdge - particleEmitter.position.get(alpha)) * scale,
				(trail.position[p + 1u] - endToEdge - particleEmitter.position.get(alpha)) * scale,
				((trail.position[p] + trail.position[p + 1u]) * 0.5  - particleEmitter.position.get(alpha)) * scale
			};
			pixelpart::vec3d vertexNormals[3] = {
				glm::cross(trail.directionToEdge[p], trail.direction[p]),
				glm::cross(trail.directionToEdge[p + 1u], trail.direction[p + 1u]),
				pixelpart::vec3d(0.0)
			};
			vertexNormals[2u] = (vertexNormals[0u] + vertexNormals[1u]) * 0.5;

			switch(particleType.trailRendererSettings.textureRotation) {
				case 1u:
					vertexTextureCoords[0] = pixelpart::vec2d(trail.index[p] * particleType.trailRendererSettings.textureUVFactor, 1.0);
					vertexTextureCoords[1] = pixelpart::vec2d(trail.index[p] * particleType.trailRendererSettings.textureUVFactor, 0.0);
					vertexTextureCoords[2] = pixelpart::vec2d(trail.index[p + 1u] * particleType.trailRendererSettings.textureUVFactor, 1.0);
					vertexTextureCoords[3] = pixelpart::vec2d(trail.index[p + 1u] * particleType.trailRendererSettings.textureUVFactor, 0.0);
					vertexTextureCoords[4] = pixelpart::vec2d((trail.index[p] + trail.index[p + 1u]) * 0.5 * particleType.trailRendererSettings.textureUVFactor, 0.5);
					break;

				case 2u:
					vertexTextureCoords[0] = pixelpart::vec2d(1.0, 1.0 - trail.index[p] * particleType.trailRendererSettings.textureUVFactor);
					vertexTextureCoords[1] = pixelpart::vec2d(0.0, 1.0 - trail.index[p] * particleType.trailRendererSettings.textureUVFactor);
					vertexTextureCoords[2] = pixelpart::vec2d(1.0, 1.0 - trail.index[p + 1u] * particleType.trailRendererSettings.textureUVFactor);
					vertexTextureCoords[3] = pixelpart::vec2d(0.0, 1.0 - trail.index[p + 1u] * particleType.trailRendererSettings.textureUVFactor);
					vertexTextureCoords[4] = pixelpart::vec2d(0.5, 1.0 - (trail.index[p] + trail.index[p + 1u]) * 0.5 * particleType.trailRendererSettings.textureUVFactor);
					break;

				case 3u:
					vertexTextureCoords[0] = pixelpart::vec2d(1.0 - trail.index[p] * particleType.trailRendererSettings.textureUVFactor, 0.0);
					vertexTextureCoords[1] = pixelpart::vec2d(1.0 - trail.index[p] * particleType.trailRendererSettings.textureUVFactor, 1.0);
					vertexTextureCoords[2] = pixelpart::vec2d(1.0 - trail.index[p + 1u] * particleType.trailRendererSettings.textureUVFactor, 0.0);
					vertexTextureCoords[3] = pixelpart::vec2d(1.0 - trail.index[p + 1u] * particleType.trailRendererSettings.textureUVFactor, 1.0);
					vertexTextureCoords[4] = pixelpart::vec2d(1.0 - (trail.index[p] + trail.index[p + 1u]) * 0.5 * particleType.trailRendererSettings.textureUVFactor, 0.5);
					break;

				default:
					vertexTextureCoords[0] = pixelpart::vec2d(0.0, trail.index[p] * particleType.trailRendererSettings.textureUVFactor);
					vertexTextureCoords[1] = pixelpart::vec2d(1.0, trail.index[p] * particleType.trailRendererSettings.textureUVFactor);
					vertexTextureCoords[2] = pixelpart::vec2d(0.0, trail.index[p + 1u] * particleType.trailRendererSettings.textureUVFactor);
					vertexTextureCoords[3] = pixelpart::vec2d(1.0, trail.index[p + 1u] * particleType.trailRendererSettings.textureUVFactor);
					vertexTextureCoords[4] = pixelpart::vec2d(0.5, (trail.index[p] + trail.index[p + 1u]) * 0.5 * particleType.trailRendererSettings.textureUVFactor);
					break;
			}

			indices[vertexIndex * 12 + 0] = vertexIndex * 5 + 0;
			indices[vertexIndex * 12 + 1] = vertexIndex * 5 + 1;
			indices[vertexIndex * 12 + 2] = vertexIndex * 5 + 4;
			indices[vertexIndex * 12 + 3] = vertexIndex * 5 + 2;
			indices[vertexIndex * 12 + 4] = vertexIndex * 5 + 0;
			indices[vertexIndex * 12 + 5] = vertexIndex * 5 + 4;
			indices[vertexIndex * 12 + 6] = vertexIndex * 5 + 1;
			indices[vertexIndex * 12 + 7] = vertexIndex * 5 + 3;
			indices[vertexIndex * 12 + 8] = vertexIndex * 5 + 4;
			indices[vertexIndex * 12 + 9] = vertexIndex * 5 + 3;
			indices[vertexIndex * 12 + 10] = vertexIndex * 5 + 2;
			indices[vertexIndex * 12 + 11] = vertexIndex * 5 + 4;

			positions[vertexIndex * 5 + 0] = toGd(vertexPositions[0]);
			positions[vertexIndex * 5 + 1] = toGd(vertexPositions[1]);
			positions[vertexIndex * 5 + 2] = toGd(vertexPositions[2]);
			positions[vertexIndex * 5 + 3] = toGd(vertexPositions[3]);
			positions[vertexIndex * 5 + 4] = toGd(vertexPositions[4]);

			normals[vertexIndex * 5 + 0] = toGd(vertexNormals[0]);
			normals[vertexIndex * 5 + 1] = toGd(vertexNormals[0]);
			normals[vertexIndex * 5 + 2] = toGd(vertexNormals[1]);
			normals[vertexIndex * 5 + 3] = toGd(vertexNormals[1]);
			normals[vertexIndex * 5 + 4] = toGd(vertexNormals[2]);

			tangents[vertexIndex * 5 * 4 + 0] = static_cast<float>(trail.velocity[p].x);
			tangents[vertexIndex * 5 * 4 + 1] = static_cast<float>(trail.velocity[p].y);
			tangents[vertexIndex * 5 * 4 + 2] = static_cast<float>(trail.velocity[p].z);
			tangents[vertexIndex * 5 * 4 + 3] = 0.0f;
			tangents[vertexIndex * 5 * 4 + 4] = static_cast<float>(trail.velocity[p].x);
			tangents[vertexIndex * 5 * 4 + 5] = static_cast<float>(trail.velocity[p].y);
			tangents[vertexIndex * 5 * 4 + 6] = static_cast<float>(trail.velocity[p].z);
			tangents[vertexIndex * 5 * 4 + 7] = 0.0f;
			tangents[vertexIndex * 5 * 4 + 8] = static_cast<float>(trail.velocity[p + 1u].x);
			tangents[vertexIndex * 5 * 4 + 9] = static_cast<float>(trail.velocity[p + 1u].y);
			tangents[vertexIndex * 5 * 4 + 10] = static_cast<float>(trail.velocity[p + 1u].z);
			tangents[vertexIndex * 5 * 4 + 11] = 0.0f;
			tangents[vertexIndex * 5 * 4 + 12] = static_cast<float>(trail.velocity[p + 1u].x);
			tangents[vertexIndex * 5 * 4 + 13] = static_cast<float>(trail.velocity[p + 1u].y);
			tangents[vertexIndex * 5 * 4 + 14] = static_cast<float>(trail.velocity[p + 1u].z);
			tangents[vertexIndex * 5 * 4 + 15] = 0.0f;
			tangents[vertexIndex * 5 * 4 + 16] = static_cast<float>(trail.velocity[p].x + trail.velocity[p + 1u].x) * 0.5f;
			tangents[vertexIndex * 5 * 4 + 17] = static_cast<float>(trail.velocity[p].y + trail.velocity[p + 1u].y) * 0.5f;
			tangents[vertexIndex * 5 * 4 + 18] = static_cast<float>(trail.velocity[p].z + trail.velocity[p + 1u].z) * 0.5f;
			tangents[vertexIndex * 5 * 4 + 19] = 0.0f;

			colors[vertexIndex * 5 + 0] = toGd(trail.color[p] * 0.1 +
				pixelpart::vec4d(glm::floor(trail.force[p] * packFactor) + pixelpart::vec3d(packFactor), 0.0));
			colors[vertexIndex * 5 + 1] = toGd(trail.color[p] * 0.1 +
				pixelpart::vec4d(glm::floor(trail.force[p] * packFactor) + pixelpart::vec3d(packFactor), 0.0));
			colors[vertexIndex * 5 + 2] = toGd(trail.color[p + 1u] * 0.1 +
				pixelpart::vec4d(glm::floor(trail.force[p + 1u] * packFactor) + pixelpart::vec3d(packFactor), 0.0));
			colors[vertexIndex * 5 + 3] = toGd(trail.color[p + 1u] * 0.1 +
				pixelpart::vec4d(glm::floor(trail.force[p + 1u] * packFactor) + pixelpart::vec3d(packFactor), 0.0));
			colors[vertexIndex * 5 + 4] = toGd((trail.color[p] + trail.color[p + 1u]) * 0.5 * 0.1 +
				pixelpart::vec4d(glm::floor((trail.force[p] + trail.force[p + 1u]) * 0.5 * packFactor) + pixelpart::vec3d(packFactor), 0.0));

			uvs[vertexIndex * 5 + 0] = toGd(vertexTextureCoords[0]);
			uvs[vertexIndex * 5 + 1] = toGd(vertexTextureCoords[1]);
			uvs[vertexIndex * 5 + 2] = toGd(vertexTextureCoords[2]);
			uvs[vertexIndex * 5 + 3] = toGd(vertexTextureCoords[3]);
			uvs[vertexIndex * 5 + 4] = toGd(vertexTextureCoords[4]);

			uvs2[vertexIndex * 5 + 0] = Vector2(static_cast<float>(trail.life[p]), static_cast<float>(entry.first));
			uvs2[vertexIndex * 5 + 1] = Vector2(static_cast<float>(trail.life[p]), static_cast<float>(entry.first));
			uvs2[vertexIndex * 5 + 2] = Vector2(static_cast<float>(trail.life[p + 1u]), static_cast<float>(entry.first));
			uvs2[vertexIndex * 5 + 3] = Vector2(static_cast<float>(trail.life[p + 1u]), static_cast<float>(entry.first));
			uvs2[vertexIndex * 5 + 4] = Vector2(static_cast<float>(trail.life[p] + trail.life[p + 1u]) * 0.5f, static_cast<float>(entry.first));

			vertexIndex++;
		}
	}

	Array meshArray;
	meshArray.resize(Mesh::ARRAY_MAX);
	meshArray[Mesh::ARRAY_VERTEX] = vertexArray;
	meshArray[Mesh::ARRAY_NORMAL] = normalArray;
	meshArray[Mesh::ARRAY_TANGENT] = tangentArray;
	meshArray[Mesh::ARRAY_COLOR] = colorArray;
	meshArray[Mesh::ARRAY_TEX_UV] = uvArray;
	meshArray[Mesh::ARRAY_TEX_UV2] = uv2Array;
	meshArray[Mesh::ARRAY_INDEX] = indexArray;

	mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, meshArray, Array(),
		Mesh::ARRAY_FORMAT_VERTEX | Mesh::ARRAY_FORMAT_NORMAL | Mesh::ARRAY_FORMAT_TANGENT |
		Mesh::ARRAY_FORMAT_COLOR | Mesh::ARRAY_FORMAT_TEX_UV | Mesh::ARRAY_FORMAT_TEX_UV2 |
		Mesh::ARRAY_FORMAT_INDEX);
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
	pixelpart::vec3d front = glm::normalize(direction);
	pixelpart::vec3d up = pixelpart::vec3d(0.0, 1.0, 0.0);
	if(glm::abs(glm::abs(glm::dot(front, up)) - 1.0) < 0.001) {
		up = pixelpart::vec3d(1.0, 0.0, 0.0);
	}

	pixelpart::vec3d right = glm::normalize(glm::cross(front, up));
	up = glm::normalize(glm::cross(right, front));

	return pixelpart::mat3d(right, up, front);
}
}