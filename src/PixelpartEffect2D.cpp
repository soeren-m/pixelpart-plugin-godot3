#include "PixelpartEffect2D.h"
#include "PixelpartShaders.h"
#include "PixelpartUtil.h"
#include <VisualServer.hpp>
#include <ProjectSettings.hpp>
#include <ImageTexture.hpp>

namespace godot {
void PixelpartEffect2D::_register_methods() {
	register_property<PixelpartEffect2D, Ref<PixelpartEffectResource>>("effect", &PixelpartEffect2D::set_effect, &PixelpartEffect2D::get_effect, nullptr);
	register_property<PixelpartEffect2D, bool>("playing", &PixelpartEffect2D::play, &PixelpartEffect2D::is_playing, true);
	register_property<PixelpartEffect2D, bool>("loop", &PixelpartEffect2D::set_loop, &PixelpartEffect2D::get_loop, false);
	register_property<PixelpartEffect2D, float>("loop_time", &PixelpartEffect2D::set_loop_time, &PixelpartEffect2D::get_loop_time, 1.0f,
		GODOT_METHOD_RPC_MODE_DISABLED,
		GODOT_PROPERTY_USAGE_DEFAULT,
		GODOT_PROPERTY_HINT_EXP_RANGE, "0.0,1000.0,0.01");
	register_property<PixelpartEffect2D, float>("speed", &PixelpartEffect2D::set_speed, &PixelpartEffect2D::get_speed, 1.0f,
		GODOT_METHOD_RPC_MODE_DISABLED,
		GODOT_PROPERTY_USAGE_DEFAULT,
		GODOT_PROPERTY_HINT_EXP_RANGE, "0.0,100.0,0.01");
	register_property<PixelpartEffect2D, float>("frame_rate", &PixelpartEffect2D::set_frame_rate, &PixelpartEffect2D::get_frame_rate, 60.0f,
		GODOT_METHOD_RPC_MODE_DISABLED,
		GODOT_PROPERTY_USAGE_DEFAULT,
		GODOT_PROPERTY_HINT_EXP_RANGE, "1.0,100.0,1.0");
	register_property<PixelpartEffect2D, bool>("flip_h", &PixelpartEffect2D::set_flip_h, &PixelpartEffect2D::get_flip_h, false);
	register_property<PixelpartEffect2D, bool>("flip_v", &PixelpartEffect2D::set_flip_v, &PixelpartEffect2D::get_flip_v, true);
	register_method("_init", &PixelpartEffect2D::_init);
	register_method("_enter_tree", &PixelpartEffect2D::_enter_tree);
	register_method("_exit_tree", &PixelpartEffect2D::_exit_tree);
	register_method("_process", &PixelpartEffect2D::_process);
	register_method("draw", &PixelpartEffect2D::draw);
	register_method("play", &PixelpartEffect2D::play);
	register_method("pause", &PixelpartEffect2D::pause);
	register_method("restart", &PixelpartEffect2D::restart);
	register_method("reset", &PixelpartEffect2D::reset);
	register_method("is_playing", &PixelpartEffect2D::is_playing);
	register_method("get_time", &PixelpartEffect2D::get_time);
	register_method("get_import_scale", &PixelpartEffect2D::get_import_scale);
	register_method("get_particle_emitter", &PixelpartEffect2D::get_particle_emitter);
	register_method("get_sprite", &PixelpartEffect2D::get_sprite);
	register_method("get_force_field", &PixelpartEffect2D::get_force_field);
	register_method("get_collider", &PixelpartEffect2D::get_collider);
	register_method("get_particle_emitter_by_id", &PixelpartEffect2D::get_particle_emitter_by_id);
	register_method("get_sprite_by_id", &PixelpartEffect2D::get_sprite_by_index);
	register_method("get_force_field_by_id", &PixelpartEffect2D::get_force_field_by_id);
	register_method("get_collider_by_id", &PixelpartEffect2D::get_collider_by_id);
	register_method("get_particle_emitter_by_index", &PixelpartEffect2D::get_particle_emitter_by_index);
	register_method("get_sprite_by_index", &PixelpartEffect2D::get_sprite_by_index);
	register_method("get_force_field_by_index", &PixelpartEffect2D::get_force_field_by_index);
	register_method("get_collider_by_index", &PixelpartEffect2D::get_collider_by_index);
}

PixelpartEffect2D::PixelpartEffect2D() {
	auto settings = ProjectSettings::get_singleton();

	if(settings->has_setting("pixelpart/max_num_particles")) {
		particleEngine.setParticleCapacity((int)settings->get_setting("pixelpart/max_num_particles"));
	}
	else {
		particleEngine.setParticleCapacity(10000);
	}
}
PixelpartEffect2D::~PixelpartEffect2D() {
	VisualServer* vs = VisualServer::get_singleton();

	for(EmitterInstance& inst : emitterInstances) {
		vs->free_rid(inst.canvasItem);
		vs->free_rid(inst.material);
	}

	for(SpriteInstance& inst : spriteInstances) {
		vs->free_rid(inst.canvasItem);
		vs->free_rid(inst.material);
	}

	for(auto& entry : textures) {
		vs->free_rid(entry.second);
	}
}

void PixelpartEffect2D::_init() {
	simulationTime = 0.0f;

	playing = true;
	loop = false;
	loopTime = 1.0f;
	speed = 1.0f;
	timeStep = 1.0f / 60.0f;

	flipH = false;
	flipV = true;
}
void PixelpartEffect2D::_enter_tree() {
	VisualServer::get_singleton()->connect("frame_pre_draw", this, "draw");
}
void PixelpartEffect2D::_exit_tree() {
	VisualServer::get_singleton()->disconnect("frame_pre_draw", this, "draw");
}

void PixelpartEffect2D::_process(float dt) {
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
void PixelpartEffect2D::draw() {
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

			draw_sprite2d(
				sprite,
				spriteInstances[spriteIndex].canvasItem,
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

			draw_emitter2d(
				emitter,
				emitterInstances[emitterIndex].meshBuilder,
				emitterInstances[emitterIndex].canvasItem,
				emitterInstances[emitterIndex].shader,
				emitterInstances[emitterIndex].material,
				textureRIDs);

			e++;
		}
	}
}

void PixelpartEffect2D::play(bool p) {
	playing = p;
}
void PixelpartEffect2D::pause() {
	playing = false;
}
void PixelpartEffect2D::restart() {
	particleEngine.restart();
}
void PixelpartEffect2D::reset() {
	particleEngine.reset();
}
bool PixelpartEffect2D::is_playing() const {
	return playing;
}
float PixelpartEffect2D::get_time() const {
	return static_cast<float>(particleEngine.getTime());
}

void PixelpartEffect2D::set_loop(bool l) {
	loop = l;
}
bool PixelpartEffect2D::get_loop() const {
	return loop;
}

void PixelpartEffect2D::set_loop_time(float l) {
	loopTime = std::max(l, 0.0f);
}
float PixelpartEffect2D::get_loop_time() const {
	return loopTime;
}

void PixelpartEffect2D::set_speed(float s) {
	speed = std::max(s, 0.0f);
}
float PixelpartEffect2D::get_speed() const {
	return speed;
}

void PixelpartEffect2D::set_frame_rate(float r) {
	timeStep = 1.0f / std::min(std::max(r, 1.0f), 100.0f);
}
float PixelpartEffect2D::get_frame_rate() const {
	return 1.0f / timeStep;
}

void PixelpartEffect2D::set_flip_h(bool flip) {
	flipH = flip;
}
void PixelpartEffect2D::set_flip_v(bool flip) {
	flipV = flip;
}
bool PixelpartEffect2D::get_flip_h() const {
	return flipH;
}
bool PixelpartEffect2D::get_flip_v() const {
	return flipV;
}

float PixelpartEffect2D::get_import_scale() const {
	if(!effectResource.is_valid()) {
		return 1.0f;
	}

	return effectResource->get_scale();
}

void PixelpartEffect2D::set_effect(Ref<PixelpartEffectResource> effectRes) {
	VisualServer* vs = VisualServer::get_singleton();
	PixelpartShaders* shaders = PixelpartShaders::get_instance();

	for(EmitterInstance& inst : emitterInstances) {
		vs->free_rid(inst.canvasItem);
		vs->free_rid(inst.material);
	}

	for(SpriteInstance& inst : spriteInstances) {
		vs->free_rid(inst.canvasItem);
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
					vs->canvas_item_create(),
					vs->material_create(),
					shaders->get_shader(buildResult.code,
						"canvas_item",
						(particleEmitter.blendMode == pixelpart::BlendMode::additive) ? "blend_add" :
						(particleEmitter.blendMode == pixelpart::BlendMode::subtractive) ? "blend_sub" :
						"blend_mix"),
					buildResult.textureIds
				});
			}

			for(const pixelpart::Sprite& sprite : nativeEffect.sprites) {
				pixelpart::ShaderGraph::BuildResult buildResult;
				sprite.shader.build(buildResult);

				spriteInstances.push_back(SpriteInstance{
					vs->canvas_item_create(),
					vs->material_create(),
					shaders->get_shader(buildResult.code,
						"canvas_item",
						(sprite.blendMode == pixelpart::BlendMode::additive) ? "blend_add" :
						(sprite.blendMode == pixelpart::BlendMode::subtractive) ? "blend_sub" :
						"blend_mix"),
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

			Godot::print_error(String(e.what()), __FUNCTION__, "PixelpartEffect2D.cpp", __LINE__);
		}
	}
	else {
		particleEngine.setEffect(nullptr);
	}
}
Ref<PixelpartEffectResource> PixelpartEffect2D::get_effect() const {
	return effectResource;
}

Ref<PixelpartParticleEmitter> PixelpartEffect2D::get_particle_emitter(String name) const {
	CharString nameCharString = name.utf8();
	std::string nameStdString = std::string(nameCharString.get_data(), nameCharString.length());

	if(particleEmitters.count(nameStdString)) {
		return particleEmitters.at(nameStdString);
	}

	return Ref<PixelpartParticleEmitter>();
}
Ref<PixelpartSprite> PixelpartEffect2D::get_sprite(String name) const {
	CharString nameCharString = name.utf8();
	std::string nameStdString = std::string(nameCharString.get_data(), nameCharString.length());

	if(sprites.count(nameStdString)) {
		return sprites.at(nameStdString);
	}

	return Ref<PixelpartSprite>();
}
Ref<PixelpartForceField> PixelpartEffect2D::get_force_field(String name) const {
	CharString nameCharString = name.utf8();
	std::string nameStdString = std::string(nameCharString.get_data(), nameCharString.length());

	if(forceFields.count(nameStdString)) {
		return forceFields.at(nameStdString);
	}

	return Ref<PixelpartForceField>();
}
Ref<PixelpartCollider> PixelpartEffect2D::get_collider(String name) const {
	CharString nameCharString = name.utf8();
	std::string nameStdString = std::string(nameCharString.get_data(), nameCharString.length());

	if(colliders.count(nameStdString)) {
		return colliders.at(nameStdString);
	}

	return Ref<PixelpartCollider>();
}
Ref<PixelpartParticleEmitter> PixelpartEffect2D::get_particle_emitter_by_id(int id) const {
	if(id >= 0 && nativeEffect.particleEmitters.contains(static_cast<uint32_t>(id))) {
		std::string name = nativeEffect.particleEmitters.get(static_cast<uint32_t>(id)).name;

		if(particleEmitters.count(name)) {
			return particleEmitters.at(name);
		}
	}

	return Ref<PixelpartParticleEmitter>();
}
Ref<PixelpartSprite> PixelpartEffect2D::get_sprite_by_id(int id) const {
	if(id >= 0 && nativeEffect.sprites.contains(static_cast<uint32_t>(id))) {
		std::string name = nativeEffect.sprites.get(static_cast<uint32_t>(id)).name;

		if(sprites.count(name)) {
			return sprites.at(name);
		}
	}

	return Ref<PixelpartSprite>();
}
Ref<PixelpartForceField> PixelpartEffect2D::get_force_field_by_id(int id) const {
	if(id >= 0 && nativeEffect.forceFields.contains(static_cast<uint32_t>(id))) {
		std::string name = nativeEffect.forceFields.get(static_cast<uint32_t>(id)).name;

		if(forceFields.count(name)) {
			return forceFields.at(name);
		}
	}

	return Ref<PixelpartForceField>();
}
Ref<PixelpartCollider> PixelpartEffect2D::get_collider_by_id(int id) const {
	if(id >= 0 && nativeEffect.colliders.contains(static_cast<uint32_t>(id))) {
		std::string name = nativeEffect.colliders.get(static_cast<uint32_t>(id)).name;

		if(colliders.count(name)) {
			return colliders.at(name);
		}
	}

	return Ref<PixelpartCollider>();
}
Ref<PixelpartParticleEmitter> PixelpartEffect2D::get_particle_emitter_by_index(int index) const {
	if(index >= 0 && nativeEffect.particleEmitters.containsIndex(static_cast<uint32_t>(index))) {
		std::string name = nativeEffect.particleEmitters.getByIndex(static_cast<uint32_t>(index)).name;

		if(particleEmitters.count(name)) {
			return particleEmitters.at(name);
		}
	}

	return Ref<PixelpartParticleEmitter>();
}
Ref<PixelpartSprite> PixelpartEffect2D::get_sprite_by_index(int index) const {
	if(index >= 0 && nativeEffect.sprites.containsIndex(static_cast<uint32_t>(index))) {
		std::string name = nativeEffect.sprites.getByIndex(static_cast<uint32_t>(index)).name;

		if(sprites.count(name)) {
			return sprites.at(name);
		}
	}

	return Ref<PixelpartSprite>();
}
Ref<PixelpartForceField> PixelpartEffect2D::get_force_field_by_index(int index) const {
	if(index >= 0 && nativeEffect.forceFields.containsIndex(static_cast<uint32_t>(index))) {
		std::string name = nativeEffect.forceFields.getByIndex(static_cast<uint32_t>(index)).name;

		if(forceFields.count(name)) {
			return forceFields.at(name);
		}
	}

	return Ref<PixelpartForceField>();
}
Ref<PixelpartCollider> PixelpartEffect2D::get_collider_by_index(int index) const {
	if(index >= 0 && nativeEffect.colliders.containsIndex(static_cast<uint32_t>(index))) {
		std::string name = nativeEffect.colliders.getByIndex(static_cast<uint32_t>(index)).name;

		if(colliders.count(name)) {
			return colliders.at(name);
		}
	}

	return Ref<PixelpartCollider>();
}

void PixelpartEffect2D::draw_emitter2d(const pixelpart::ParticleEmitter& emitter, pixelpart::ParticleMeshBuilder& meshBuilder, RID canvasItem, RID shader, RID material, const std::vector<RID>& textures) {
	VisualServer* vs = VisualServer::get_singleton();

	vs->canvas_item_clear(canvasItem);

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

		Vector2* textureCoords = textureCoordArray.write().ptr();
		for(int i = 0; i < textureCoordArray.size(); i++) {
			textureCoords[i].x = std::floor(lifeArray[i] * 10000.0f) + textureCoords[i].x * 0.5f;
			textureCoords[i].y = static_cast<float>(idArray[i]) + textureCoords[i].y * 0.5f;
		}

		Color* colors = colorArray.write().ptr(); 
		for(int i = 0; i < colorArray.size(); i++) {
			colors[i].r = std::floor(velocityArray[i].x * 10000.0f + 10000.0f) + colors[i].r * 0.5f;
			colors[i].g = std::floor(velocityArray[i].y * 10000.0f + 10000.0f) + colors[i].g * 0.5f;
			colors[i].b = std::floor(forceArray[i].x * 10000.0f + 10000.0f) + colors[i].b * 0.5f;
			colors[i].a = std::floor(forceArray[i].y * 10000.0f + 10000.0f) + colors[i].a * 0.5f;
		}

		vs->material_set_shader(material, shader);
		vs->material_set_param(material, "EFFECT_TIME", static_cast<float>(particleEngine.getTime()));
		vs->material_set_param(material, "OBJECT_TIME", static_cast<float>(particleEngine.getTime() - emitter.lifetimeStart));
		for(std::size_t t = 0; t < textures.size(); t++) {
			vs->material_set_param(material, String("TEXTURE") + String::num_int64(t), textures[t]);
		}

		vs->canvas_item_set_parent(canvasItem, get_canvas_item());
		vs->canvas_item_set_transform(canvasItem, Transform2D());
		vs->canvas_item_set_material(canvasItem, material);

		if(meshBuilder.getNumIndices() > 0) {
			vs->canvas_item_add_triangle_array(canvasItem,
				indexArray,
				pointArray,
				colorArray,
				textureCoordArray);
		}
	}
}

void PixelpartEffect2D::draw_sprite2d(const pixelpart::Sprite& sprite, RID canvasItem, RID shader, RID material, const std::vector<RID>& textures) {
	VisualServer* vs = VisualServer::get_singleton();

	vs->canvas_item_clear(canvasItem);

	if(is_visible() && sprite.visible &&
	particleEngine.getTime() >= sprite.lifetimeStart &&
	(particleEngine.getTime() <= sprite.lifetimeStart + sprite.lifetimeDuration || sprite.repeat)) {
		const pixelpart::floatd scaleX = static_cast<pixelpart::floatd>(get_import_scale()) * (flipH ? -1.0 : +1.0);
		const pixelpart::floatd scaleY = static_cast<pixelpart::floatd>(get_import_scale()) * (flipV ? -1.0 : +1.0);

		pixelpart::SpriteMeshBuilder meshBuilder(sprite, particleEngine.getTime());

		PoolIntArray indexArray;
		PoolVector2Array pointArray;
		PoolVector2Array textureCoordArray;
		PoolColorArray colorArray;
		PoolRealArray lifeArray;
		indexArray.resize(meshBuilder.getNumIndices());
		pointArray.resize(meshBuilder.getNumVertices());
		textureCoordArray.resize(meshBuilder.getNumVertices());
		colorArray.resize(meshBuilder.getNumVertices());
		lifeArray.resize(meshBuilder.getNumVertices());

		meshBuilder.build(
			indexArray.write().ptr(),
			reinterpret_cast<float*>(pointArray.write().ptr()),
			reinterpret_cast<float*>(textureCoordArray.write().ptr()),
			reinterpret_cast<float*>(colorArray.write().ptr()),
			lifeArray.write().ptr(),
			scaleX,
			scaleY);

		Vector2* textureCoords = textureCoordArray.write().ptr();
		for(std::size_t i = 0; i < textureCoordArray.size(); i++) {
			textureCoords[i].x = std::floor(lifeArray[i] * 10000.0f) + textureCoords[i].x * 0.5f;
			textureCoords[i].y = textureCoords[i].y * 0.5;
		}

		Color* colors = colorArray.write().ptr(); 
		for(std::size_t i = 0; i < colorArray.size(); i++) {
			colors[i].r = 10000.0f + colors[i].r * 0.5f;
			colors[i].g = 10000.0f + colors[i].g * 0.5f;
			colors[i].b = 10000.0f + colors[i].b * 0.5f;
			colors[i].a = 10000.0f + colors[i].a * 0.5f;
		}

		vs->material_set_shader(material, shader);
		vs->material_set_param(material, "EFFECT_TIME", static_cast<float>(particleEngine.getTime()));
		vs->material_set_param(material, "OBJECT_TIME", static_cast<float>(particleEngine.getTime() - sprite.lifetimeStart));
		for(std::size_t t = 0; t < textures.size(); t++) {
			vs->material_set_param(material, String("TEXTURE") + String::num_int64(t), textures[t]);
		}

		vs->canvas_item_set_parent(canvasItem, get_canvas_item());
		vs->canvas_item_set_transform(canvasItem, Transform2D());
		vs->canvas_item_set_material(canvasItem, material);

		vs->canvas_item_add_triangle_array(canvasItem,
			indexArray,
			pointArray,
			colorArray,
			textureCoordArray);
	}
}
}