#include "PixelpartEffect.h"
#include "PixelpartShaders.h"
#include "PixelpartUtil.h"
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
	register_property<PixelpartEffect, int>("billboard_mode", &PixelpartEffect::set_billboard_mode, &PixelpartEffect::get_billboard_mode, SpatialMaterial::BILLBOARD_DISABLED,
		GODOT_METHOD_RPC_MODE_DISABLED,
		GODOT_PROPERTY_USAGE_DEFAULT,
		GODOT_PROPERTY_HINT_ENUM, "Disabled,Enabled,Y-Billboard");
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
	register_method("get_particle_emitter", &PixelpartEffect::get_particle_emitter);
	register_method("get_sprite", &PixelpartEffect::get_sprite);
	register_method("get_force_field", &PixelpartEffect::get_force_field);
	register_method("get_collider", &PixelpartEffect::get_collider);
	register_method("get_particle_emitter_by_id", &PixelpartEffect::get_particle_emitter_by_id);
	register_method("get_sprite_by_id", &PixelpartEffect::get_sprite_by_index);
	register_method("get_force_field_by_id", &PixelpartEffect::get_force_field_by_id);
	register_method("get_collider_by_id", &PixelpartEffect::get_collider_by_id);
	register_method("get_particle_emitter_by_index", &PixelpartEffect::get_particle_emitter_by_index);
	register_method("get_sprite_by_index", &PixelpartEffect::get_sprite_by_index);
	register_method("get_force_field_by_index", &PixelpartEffect::get_force_field_by_index);
	register_method("get_collider_by_index", &PixelpartEffect::get_collider_by_index);
}

PixelpartEffect::PixelpartEffect() {
	auto settings = ProjectSettings::get_singleton();

	if(settings->has_setting("pixelpart/max_num_particles")) {
		particleEngine.setParticleCapacity((int)settings->get_setting("pixelpart/max_num_particles"));
	}
	else {
		particleEngine.setParticleCapacity(10000);
	}
}
PixelpartEffect::~PixelpartEffect() {
	VisualServer* vs = VisualServer::get_singleton();

	for(EmitterInstance& inst : emitterInstances) {
		vs->free_rid(inst.immediate);
		vs->free_rid(inst.instance);
		vs->free_rid(inst.material);
	}

	for(SpriteInstance& inst : spriteInstances) {
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
	billboardMode = SpatialMaterial::BILLBOARD_DISABLED;
}
void PixelpartEffect::_enter_tree() {
	VisualServer::get_singleton()->connect("frame_pre_draw", this, "draw");
}
void PixelpartEffect::_exit_tree() {
	VisualServer::get_singleton()->disconnect("frame_pre_draw", this, "draw");
}

void PixelpartEffect::_process(float dt) {
	if(!particleEngine.getEffect()) {
		return;
	}

	if(playing) {
		simulationTime += std::min(dt, 1.0f) * speed;

		while(simulationTime > timeStep) {
			simulationTime -= timeStep;
			particleEngine.step(timeStep);

			if(loop) {
				if(particleEngine.getTime() > loopTime) {
					particleEngine.reset();
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

	const pixelpart::Effect* effect = particleEngine.getEffect();
	if(!effect) {
		return;
	}

	const std::vector<pixelpart::ParticleEmitter>& emitters = effect->particleEmitters.get();
	const std::vector<pixelpart::Sprite> sprites = effect->sprites.get();
	const std::vector<uint32_t> emitterIndicesSortedByLayer = effect->particleEmitters.getSortedIndices([](const pixelpart::ParticleEmitter& emitter1, const pixelpart::ParticleEmitter& emitter2) {
		return emitter1.layer < emitter2.layer;
	});
	const std::vector<uint32_t> spriteIndicesSortedByLayer = effect->sprites.getSortedIndices([](const pixelpart::Sprite& sprite1, const pixelpart::Sprite& sprite2) {
		return sprite1.layer < sprite2.layer;
	});

	uint32_t maxLayer = 0;
	if(!emitters.empty()) {
		maxLayer = std::max(maxLayer, emitters[emitterIndicesSortedByLayer.back()].layer);
	}
	if(!sprites.empty()) {
		maxLayer = std::max(maxLayer, sprites[spriteIndicesSortedByLayer.back()].layer);
	}

	for(uint32_t l = 0, e = 0, s = 0; l <= maxLayer; l++) {
		while(s < spriteIndicesSortedByLayer.size()) {
			const uint32_t spriteIndex = spriteIndicesSortedByLayer[s];
			const pixelpart::Sprite& sprite = sprites[spriteIndex];
			if(sprite.layer != l || spriteIndex >= spriteInstances.size()) {
				break;
			}

			std::vector<RID> textureRIDs(spriteInstances[spriteIndex].textures.size());
			for(uint32_t t = 0; t < textureRIDs.size(); t++) {
				textureRIDs[t] = textures.at(spriteInstances[spriteIndex].textures[t]);
			}

			draw_sprite(
				sprite,
				spriteInstances[spriteIndex].instance,
				spriteInstances[spriteIndex].immediate,
				spriteInstances[spriteIndex].shader,
				spriteInstances[spriteIndex].material,
				textureRIDs);

			s++;
		}

		while(e < emitterIndicesSortedByLayer.size()) {
			const uint32_t emitterIndex = emitterIndicesSortedByLayer[e];
			const pixelpart::ParticleEmitter& emitter = emitters[emitterIndex];
			if(emitter.layer != l || emitterIndex >= emitterInstances.size()) {
				break;
			}

			std::vector<RID> textureRIDs(emitterInstances[emitterIndex].textures.size());
			for(uint32_t t = 0; t < textureRIDs.size(); t++) {
				textureRIDs[t] = textures.at(emitterInstances[emitterIndex].textures[t]);
			}

			draw_emitter(
				emitter,
				emitterInstances[emitterIndex].meshBuilder,
				emitterInstances[emitterIndex].instance,
				emitterInstances[emitterIndex].immediate,
				emitterInstances[emitterIndex].shader,
				emitterInstances[emitterIndex].material,
				textureRIDs);

			e++;
		}
	}
}

void PixelpartEffect::play(bool p) {
	playing = p;
}
void PixelpartEffect::pause() {
	playing = false;
}
void PixelpartEffect::restart() {
	particleEngine.restart();
}
void PixelpartEffect::reset() {
	particleEngine.reset();
}
bool PixelpartEffect::is_playing() const {
	return playing;
}
float PixelpartEffect::get_time() const {
	return static_cast<float>(particleEngine.getTime());
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
void PixelpartEffect::set_billboard_mode(int mode) {
	billboardMode = mode;
	if(billboardMode != SpatialMaterial::BILLBOARD_ENABLED && billboardMode != SpatialMaterial::BILLBOARD_FIXED_Y) {
		billboardMode = SpatialMaterial::BILLBOARD_DISABLED;
	}
}
bool PixelpartEffect::get_flip_h() const {
	return flipH;
}
bool PixelpartEffect::get_flip_v() const {
	return flipV;
}
int PixelpartEffect::get_billboard_mode() const {
	return billboardMode;
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

	for(EmitterInstance& inst : emitterInstances) {
		vs->free_rid(inst.immediate);
		vs->free_rid(inst.instance);
		vs->free_rid(inst.material);
	}

	for(SpriteInstance& inst : spriteInstances) {
		vs->free_rid(inst.immediate);
		vs->free_rid(inst.instance);
		vs->free_rid(inst.material);
	}

	for(auto& entry : textures) {
		vs->free_rid(entry.second);
	}

	emitterInstances.clear();
	spriteInstances.clear();
	particleEmitters.clear();
	sprites.clear();
	forceFields.clear();
	colliders.clear();
	textures.clear();

	effectResource = effectRes;

	if(effectResource.is_valid()) {
		effectResource->load();

		nativeEffect = effectResource->get_project().effect;
		particleEngine.setEffect(&nativeEffect);

		try {
			for(pixelpart::ParticleEmitter& particleEmitter : nativeEffect.particleEmitters) {
				Ref<PixelpartParticleEmitter> emitterRef;
				emitterRef.instance();
				emitterRef->init(effectResource, &particleEmitter, &particleEngine);
				particleEmitters[particleEmitter.name] = emitterRef;
			}

			for(pixelpart::Sprite& sprite : nativeEffect.sprites) {
				Ref<PixelpartSprite> spriteRef;
				spriteRef.instance();
				spriteRef->init(effectResource, &sprite, &particleEngine);
				sprites[sprite.name] = spriteRef;
			}

			for(pixelpart::ForceField& forceField : nativeEffect.forceFields) {
				Ref<PixelpartForceField> forceFieldRef;
				forceFieldRef.instance();
				forceFieldRef->init(effectResource, &forceField, &particleEngine);
				forceFields[forceField.name] = forceFieldRef;
			}

			for(pixelpart::Collider& collider : nativeEffect.colliders) {
				Ref<PixelpartCollider> colliderRef;
				colliderRef.instance();
				colliderRef->init(effectResource, &collider, &particleEngine);
				colliders[collider.name] = colliderRef;
			}

			for(const pixelpart::ParticleEmitter& particleEmitter : nativeEffect.particleEmitters) {
				pixelpart::ShaderGraph::BuildResult buildResult;
				particleEmitter.particleShader.build(buildResult);

				emitterInstances.push_back(EmitterInstance{
					pixelpart::ParticleMeshBuilder(),
					vs->immediate_create(),
					vs->instance_create(),
					vs->material_create(),
					shaders->get_shader(buildResult.code,
						"spatial",
						(particleEmitter.blendMode == pixelpart::BlendMode::additive) ? "cull_disabled,unshaded,blend_add" :
						(particleEmitter.blendMode == pixelpart::BlendMode::subtractive) ? "cull_disabled,unshaded,blend_sub" :
						"cull_disabled,unshaded,blend_mix"),
					buildResult.textureIds
				});
			}

			for(const pixelpart::Sprite& sprite : nativeEffect.sprites) {
				pixelpart::ShaderGraph::BuildResult buildResult;
				sprite.shader.build(buildResult);

				spriteInstances.push_back(SpriteInstance{
					vs->immediate_create(),
					vs->instance_create(),
					vs->material_create(),
					shaders->get_shader(buildResult.code,
						"spatial",
						(sprite.blendMode == pixelpart::BlendMode::additive) ? "cull_disabled,unshaded,blend_add" :
						(sprite.blendMode == pixelpart::BlendMode::subtractive) ? "cull_disabled,unshaded,blend_sub" :
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
			particleEngine.setEffect(nullptr);

			Godot::print_error(String(e.what()), __FUNCTION__, "PixelpartEffect.cpp", __LINE__);
		}
	}
	else {
		particleEngine.setEffect(nullptr);
	}
}
Ref<PixelpartEffectResource> PixelpartEffect::get_effect() const {
	return effectResource;
}

Ref<PixelpartParticleEmitter> PixelpartEffect::get_particle_emitter(String name) const {
	CharString nameCharString = name.utf8();
	std::string nameStdString = std::string(nameCharString.get_data(), nameCharString.length());

	if(particleEmitters.count(nameStdString)) {
		return particleEmitters.at(nameStdString);
	}

	return Ref<PixelpartParticleEmitter>();
}
Ref<PixelpartSprite> PixelpartEffect::get_sprite(String name) const {
	CharString nameCharString = name.utf8();
	std::string nameStdString = std::string(nameCharString.get_data(), nameCharString.length());

	if(sprites.count(nameStdString)) {
		return sprites.at(nameStdString);
	}

	return Ref<PixelpartSprite>();
}
Ref<PixelpartForceField> PixelpartEffect::get_force_field(String name) const {
	CharString nameCharString = name.utf8();
	std::string nameStdString = std::string(nameCharString.get_data(), nameCharString.length());

	if(forceFields.count(nameStdString)) {
		return forceFields.at(nameStdString);
	}

	return Ref<PixelpartForceField>();
}
Ref<PixelpartCollider> PixelpartEffect::get_collider(String name) const {
	CharString nameCharString = name.utf8();
	std::string nameStdString = std::string(nameCharString.get_data(), nameCharString.length());

	if(colliders.count(nameStdString)) {
		return colliders.at(nameStdString);
	}

	return Ref<PixelpartCollider>();
}
Ref<PixelpartParticleEmitter> PixelpartEffect::get_particle_emitter_by_id(int id) const {
	if(id >= 0 && nativeEffect.particleEmitters.contains(static_cast<uint32_t>(id))) {
		std::string name = nativeEffect.particleEmitters.get(static_cast<uint32_t>(id)).name;

		if(particleEmitters.count(name)) {
			return particleEmitters.at(name);
		}
	}

	return Ref<PixelpartParticleEmitter>();
}
Ref<PixelpartSprite> PixelpartEffect::get_sprite_by_id(int id) const {
	if(id >= 0 && nativeEffect.sprites.contains(static_cast<uint32_t>(id))) {
		std::string name = nativeEffect.sprites.get(static_cast<uint32_t>(id)).name;

		if(sprites.count(name)) {
			return sprites.at(name);
		}
	}

	return Ref<PixelpartSprite>();
}
Ref<PixelpartForceField> PixelpartEffect::get_force_field_by_id(int id) const {
	if(id >= 0 && nativeEffect.forceFields.contains(static_cast<uint32_t>(id))) {
		std::string name = nativeEffect.forceFields.get(static_cast<uint32_t>(id)).name;

		if(forceFields.count(name)) {
			return forceFields.at(name);
		}
	}

	return Ref<PixelpartForceField>();
}
Ref<PixelpartCollider> PixelpartEffect::get_collider_by_id(int id) const {
	if(id >= 0 && nativeEffect.colliders.contains(static_cast<uint32_t>(id))) {
		std::string name = nativeEffect.colliders.get(static_cast<uint32_t>(id)).name;

		if(colliders.count(name)) {
			return colliders.at(name);
		}
	}

	return Ref<PixelpartCollider>();
}
Ref<PixelpartParticleEmitter> PixelpartEffect::get_particle_emitter_by_index(int index) const {
	if(index >= 0 && nativeEffect.particleEmitters.containsIndex(static_cast<uint32_t>(index))) {
		std::string name = nativeEffect.particleEmitters.getByIndex(static_cast<uint32_t>(index)).name;

		if(particleEmitters.count(name)) {
			return particleEmitters.at(name);
		}
	}

	return Ref<PixelpartParticleEmitter>();
}
Ref<PixelpartSprite> PixelpartEffect::get_sprite_by_index(int index) const {
	if(index >= 0 && nativeEffect.sprites.containsIndex(static_cast<uint32_t>(index))) {
		std::string name = nativeEffect.sprites.getByIndex(static_cast<uint32_t>(index)).name;

		if(sprites.count(name)) {
			return sprites.at(name);
		}
	}

	return Ref<PixelpartSprite>();
}
Ref<PixelpartForceField> PixelpartEffect::get_force_field_by_index(int index) const {
	if(index >= 0 && nativeEffect.forceFields.containsIndex(static_cast<uint32_t>(index))) {
		std::string name = nativeEffect.forceFields.getByIndex(static_cast<uint32_t>(index)).name;

		if(forceFields.count(name)) {
			return forceFields.at(name);
		}
	}

	return Ref<PixelpartForceField>();
}
Ref<PixelpartCollider> PixelpartEffect::get_collider_by_index(int index) const {
	if(index >= 0 && nativeEffect.colliders.containsIndex(static_cast<uint32_t>(index))) {
		std::string name = nativeEffect.colliders.getByIndex(static_cast<uint32_t>(index)).name;

		if(colliders.count(name)) {
			return colliders.at(name);
		}
	}

	return Ref<PixelpartCollider>();
}

void PixelpartEffect::draw_emitter(const pixelpart::ParticleEmitter& emitter, pixelpart::ParticleMeshBuilder& meshBuilder, RID instance, RID immediate, RID shader, RID material, const std::vector<RID>& textures) {
	VisualServer* vs = VisualServer::get_singleton();

	vs->immediate_clear(immediate);

	if(is_visible() && emitter.visible) {
		const pixelpart::Effect* effect = particleEngine.getEffect();
		const uint32_t emitterIndex = effect->particleEmitters.findById(emitter.id);
		const uint32_t numParticles = particleEngine.getNumParticles(emitterIndex);
		const pixelpart::ParticleData& particles = particleEngine.getParticles(emitterIndex);
		const pixelpart::floatd scaleX = static_cast<pixelpart::floatd>(get_import_scale()) * (flipH ? -1.0 : +1.0);
		const pixelpart::floatd scaleY = static_cast<pixelpart::floatd>(get_import_scale()) * (flipV ? -1.0 : +1.0);

		meshBuilder.update(emitter, particles, numParticles, particleEngine.getTime());

		PoolIntArray indexArray;
		PoolVector2Array pointArray;
		PoolVector2Array textureCoordArray;
		PoolColorArray colorArray;
		PoolVector2Array velocityArray;
		PoolVector2Array forceArray;
		PoolRealArray lifeArray;
		PoolIntArray idArray;
		indexArray.resize(meshBuilder.getNumIndices());
		pointArray.resize(meshBuilder.getNumVertices());
		textureCoordArray.resize(meshBuilder.getNumVertices());
		colorArray.resize(meshBuilder.getNumVertices());
		velocityArray.resize(meshBuilder.getNumVertices());
		forceArray.resize(meshBuilder.getNumVertices());
		lifeArray.resize(meshBuilder.getNumVertices());
		idArray.resize(meshBuilder.getNumVertices());

		meshBuilder.build(
			indexArray.write().ptr(),
			reinterpret_cast<float*>(pointArray.write().ptr()),
			reinterpret_cast<float*>(textureCoordArray.write().ptr()),
			reinterpret_cast<float*>(colorArray.write().ptr()),
			reinterpret_cast<float*>(velocityArray.write().ptr()),
			reinterpret_cast<float*>(forceArray.write().ptr()),
			lifeArray.write().ptr(),
			idArray.write().ptr(),
			scaleX,
			scaleY);

		vs->immediate_begin(immediate, Mesh::PRIMITIVE_TRIANGLES);

		for(int i = 0; i < indexArray.size(); i++) {
			vs->immediate_color(immediate, colorArray[indexArray[i]]);
			vs->immediate_uv(immediate, textureCoordArray[indexArray[i]]);
			vs->immediate_uv2(immediate, Vector2(lifeArray[indexArray[i]], idArray[indexArray[i]]));
			vs->immediate_normal(immediate, Vector3(velocityArray[indexArray[i]].x, velocityArray[indexArray[i]].y, 0.0f));
			vs->immediate_tangent(immediate, Plane(forceArray[indexArray[i]].x, forceArray[indexArray[i]].y, 0.0f, 0.0f));
			vs->immediate_vertex(immediate, Vector3(pointArray[indexArray[i]].x, pointArray[indexArray[i]].y, 0.0f));
		}

		vs->immediate_end(immediate);

		vs->material_set_shader(material, shader);
		vs->material_set_param(material, "EFFECT_TIME", static_cast<float>(particleEngine.getTime()));
		vs->material_set_param(material, "OBJECT_TIME", static_cast<float>(particleEngine.getTime() - emitter.lifetimeStart));
		for(std::size_t t = 0; t < textures.size(); t++) {
			vs->material_set_param(material, String("TEXTURE") + String::num_int64(t), textures[t]);
		}

		vs->instance_set_base(instance, immediate);
		vs->instance_geometry_set_material_override(instance, material);
		vs->instance_set_scenario(instance, get_world()->get_scenario());
		vs->instance_set_transform(instance, get_final_transform());
	}
}
void PixelpartEffect::draw_sprite(const pixelpart::Sprite& sprite, RID instance, RID immediate, RID shader, RID material, const std::vector<RID>& textures) {
	VisualServer* vs = VisualServer::get_singleton();

	vs->immediate_clear(immediate);

	if(is_visible() && sprite.visible &&
	particleEngine.getTime() >= sprite.lifetimeStart &&
	(particleEngine.getTime() <= sprite.lifetimeStart + sprite.lifetimeDuration || sprite.repeat)) {
		const pixelpart::floatd scaleX = static_cast<pixelpart::floatd>(get_import_scale()) * (flipH ? -1.0 : +1.0);
		const pixelpart::floatd scaleY = static_cast<pixelpart::floatd>(get_import_scale()) * (flipV ? -1.0 : +1.0);

		pixelpart::SpriteMeshBuilder meshBuilder(sprite, particleEngine.getTime());

		Vector2 points[4];
		Vector2 uvs[4];
		Color colors[4];
		float lives[4];
		meshBuilder.build<int, float>(
			nullptr,
			reinterpret_cast<float*>(points),
			reinterpret_cast<float*>(uvs),
			reinterpret_cast<float*>(colors),
			lives,
			scaleX,
			scaleY);

		vs->immediate_begin(immediate, Mesh::PRIMITIVE_TRIANGLES);

		vs->immediate_uv(immediate, uvs[0]);
		vs->immediate_uv2(immediate, Vector2(lives[0], 0.0f));
		vs->immediate_color(immediate, colors[0]);
		vs->immediate_vertex(immediate, Vector3(points[0].x, points[0].y, 0.0f));
		vs->immediate_uv(immediate, uvs[1]);
		vs->immediate_uv2(immediate, Vector2(lives[1], 0.0f));
		vs->immediate_color(immediate, colors[1]);
		vs->immediate_vertex(immediate, Vector3(points[1].x, points[1].y, 0.0f));
		vs->immediate_uv(immediate, uvs[3]);
		vs->immediate_uv2(immediate, Vector2(lives[3], 0.0f));
		vs->immediate_color(immediate, colors[3]);
		vs->immediate_vertex(immediate, Vector3(points[3].x, points[3].y, 0.0f));
		vs->immediate_uv(immediate, uvs[1]);
		vs->immediate_uv2(immediate, Vector2(lives[1], 0.0f));
		vs->immediate_color(immediate, colors[1]);
		vs->immediate_vertex(immediate, Vector3(points[1].x, points[1].y, 0.0f));
		vs->immediate_uv(immediate, uvs[2]);
		vs->immediate_uv2(immediate, Vector2(lives[2], 0.0f));
		vs->immediate_color(immediate, colors[2]);
		vs->immediate_vertex(immediate, Vector3(points[2].x, points[2].y, 0.0f));
		vs->immediate_uv(immediate, uvs[3]);
		vs->immediate_uv2(immediate, Vector2(lives[3], 0.0f));
		vs->immediate_color(immediate, colors[3]);
		vs->immediate_vertex(immediate, Vector3(points[3].x, points[3].y, 0.0f));

		vs->immediate_end(immediate);

		vs->material_set_shader(material, shader);
		vs->material_set_param(material, "EFFECT_TIME", static_cast<float>(particleEngine.getTime()));
		vs->material_set_param(material, "OBJECT_TIME", static_cast<float>(particleEngine.getTime() - sprite.lifetimeStart));
		for(std::size_t t = 0; t < textures.size(); t++) {
			vs->material_set_param(material, String("TEXTURE") + String::num_int64(t), textures[t]);
		}

		vs->instance_set_base(instance, immediate);
		vs->instance_geometry_set_material_override(instance, material);
		vs->instance_set_scenario(instance, get_world()->get_scenario());
		vs->instance_set_transform(instance, get_final_transform());
	}
}

Transform PixelpartEffect::get_final_transform() {
	Viewport* viewport = get_viewport();
	Transform finalTransform = get_global_transform();

	if(viewport && viewport->get_camera()) {
		if(billboardMode == SpatialMaterial::BILLBOARD_ENABLED) {
			Transform cameraTransform = viewport->get_camera()->get_global_transform();

			finalTransform.set_look_at(
				finalTransform.get_origin(),
				finalTransform.get_origin() * 2.0f - cameraTransform.origin,
				Vector3::UP);
		}
		else if(billboardMode == SpatialMaterial::BILLBOARD_FIXED_Y) {
			Transform cameraTransform = viewport->get_camera()->get_global_transform();
			cameraTransform.origin.y = 0.0f;

			finalTransform.set_look_at(
				finalTransform.get_origin(),
				finalTransform.get_origin() * 2.0f - cameraTransform.origin,
				Vector3::UP);
		}
	}

	return finalTransform;
}
}