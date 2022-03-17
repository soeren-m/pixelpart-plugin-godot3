#include "PixelpartEffect.h"
#include "PixelpartShaders.h"
#include "PixelpartUtil.h"
#include "RenderUtil.h"
#include <ProjectSettings.hpp>
#include <World.hpp>
#include <Mesh.hpp>

namespace godot {
void PixelpartEffect::_register_methods() {
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
	register_property<PixelpartEffect, Ref<PixelpartEffectResource>>("effect", &PixelpartEffect::set_effect, &PixelpartEffect::get_effect, nullptr);
	register_method("_init", &PixelpartEffect::_init);
	register_method("_enter_tree", &PixelpartEffect::_enter_tree);
	register_method("_exit_tree", &PixelpartEffect::_exit_tree);
	register_method("_process", &PixelpartEffect::_process);
	register_method("_update_draw", &PixelpartEffect::_update_draw);
	register_method("play", &PixelpartEffect::play);
	register_method("pause", &PixelpartEffect::pause);
	register_method("restart", &PixelpartEffect::restart);
	register_method("reset", &PixelpartEffect::reset);
	register_method("is_playing", &PixelpartEffect::is_playing);
	register_method("get_time", &PixelpartEffect::get_time);
	register_method("get_import_scale", &PixelpartEffect::get_import_scale);
	register_method("get_particle_emitter", &PixelpartEffect::get_particle_emitter);
	register_method("get_force_field", &PixelpartEffect::get_force_field);
	register_method("get_collider", &PixelpartEffect::get_collider);
	register_method("get_sprite", &PixelpartEffect::get_sprite);
	register_method("get_particle_emitter_by_id", &PixelpartEffect::get_particle_emitter_by_id);
	register_method("get_particle_emitter_by_index", &PixelpartEffect::get_particle_emitter_by_index);
	register_method("get_force_field_by_index", &PixelpartEffect::get_force_field_by_index);
	register_method("get_collider_by_index", &PixelpartEffect::get_collider_by_index);
	register_method("get_sprite_by_index", &PixelpartEffect::get_sprite_by_index);
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

	for(InstanceData& data : emitterInstances) {
		vs->free_rid(data.immediate);
		vs->free_rid(data.instance);
		vs->free_rid(data.material);
		vs->free_rid(data.texture);
	}

	for(InstanceData& data : spriteInstances) {
		vs->free_rid(data.immediate);
		vs->free_rid(data.instance);
		vs->free_rid(data.material);
		vs->free_rid(data.texture);
	}
}

void PixelpartEffect::_init() {
	simulationTime = 0.0f;

	playing = true;
	flipH = false;
	flipV = false;
	loop = false;
	loopTime = 1.0f;
	speed = 1.0f;
	timeStep = 1.0f / 60.0f;
}
void PixelpartEffect::_enter_tree() {
	VisualServer::get_singleton()->connect("frame_pre_draw", this, "_update_draw");
}
void PixelpartEffect::_exit_tree() {
	VisualServer::get_singleton()->disconnect("frame_pre_draw", this, "_update_draw");
}

void PixelpartEffect::_process(float dt) {
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
void PixelpartEffect::_update_draw() {
	Viewport* viewport = get_viewport();
	if(!viewport) {
		Godot::print_error(String("No viewport available"), __FUNCTION__, "", __LINE__);
		return;
	}

	const pixelpart::Effect* effect = particleEngine.getEffect();
	const std::vector<pixelpart::ParticleEmitter>& emitters = effect->getParticleEmitters();
	const std::vector<pixelpart::Sprite> sprites = effect->getSprites();
	const std::vector<uint32_t> emitterOrder = effect->getParticleEmittersSortedByLayer();
	const std::vector<uint32_t> spriteOrder = effect->getSpritesSortedByLayer();
	
	uint32_t maxLayer = 0;
	if(!emitters.empty()) {
		maxLayer = std::max(maxLayer, emitters[emitterOrder.back()].layer);
	}
	if(!sprites.empty()) {
		maxLayer = std::max(maxLayer, sprites[spriteOrder.back()].layer);
	}

	for(uint32_t l = 0, emitterIndex = 0, spriteIndex = 0; l <= maxLayer; l++) {
		while(spriteIndex < spriteOrder.size()) {
			const pixelpart::Sprite& sprite = sprites[spriteOrder[spriteIndex]];
			if(sprite.layer != l || spriteIndex >= spriteInstances.size()) {
				break;
			}

			draw_sprite3d(
				sprite,
				spriteInstances[spriteIndex].instance,
				spriteInstances[spriteIndex].immediate,
				spriteInstances[spriteIndex].material,
				spriteInstances[spriteIndex].texture);

			spriteIndex++;
		}

		while(emitterIndex < emitterOrder.size()) {
			const pixelpart::ParticleEmitter& emitter = emitters[emitterOrder[emitterIndex]];
			if(emitter.layer != l || emitterIndex >= emitterInstances.size()) {
				break;
			}

			draw_emitter3d(
				emitter,
				emitterInstances[emitterIndex].instance,
				emitterInstances[emitterIndex].immediate,
				emitterInstances[emitterIndex].material,
				emitterInstances[emitterIndex].texture);

			emitterIndex++;
		}
	}
}

void PixelpartEffect::play() {
	playing = true;
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

void PixelpartEffect::set_flip_h(bool flip_h) {
	flipH =  flip_h;
}
void PixelpartEffect::set_flip_v(bool flip_v) {
	flipV = flip_v;
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

	for(InstanceData& data : emitterInstances) {
		vs->free_rid(data.immediate);
		vs->free_rid(data.instance);
		vs->free_rid(data.material);
		vs->free_rid(data.texture);
	}

	for(InstanceData& data : spriteInstances) {
		vs->free_rid(data.immediate);
		vs->free_rid(data.instance);
		vs->free_rid(data.material);
		vs->free_rid(data.texture);
	}

	emitterInstances.clear();
	spriteInstances.clear();

	effectResource = effectRes;

	particleEmitters.clear();
	forceFields.clear();
	colliders.clear();
	sprites.clear();

	if(effectResource.is_valid()) {
		effectResource->load();
		nativeEffect = effectResource->get_project().effect;

		const pixelpart::ResourceDatabase& projectResources = effectResource->get_project_resources();

		particleEngine.setEffect(&nativeEffect);

		for(uint32_t i = 0; i < nativeEffect.getNumParticleEmitters(); i++) {
			Ref<PixelpartParticleEmitter> emitterRef;
			emitterRef.instance();
			emitterRef->init(effectResource, &nativeEffect.getParticleEmitterByIndex(i), &particleEngine);
			particleEmitters[nativeEffect.getParticleEmitterByIndex(i).name] = emitterRef;
		}

		for(uint32_t i = 0; i < nativeEffect.getNumForceFields(); i++) {
			Ref<PixelpartForceField> forceFieldRef;
			forceFieldRef.instance();
			forceFieldRef->init(effectResource, &nativeEffect.getForceField(i), &particleEngine);
			forceFields[nativeEffect.getForceField(i).name] = forceFieldRef;
		}

		for(uint32_t i = 0; i < nativeEffect.getNumColliders(); i++) {
			Ref<PixelpartCollider> colliderRef;
			colliderRef.instance();
			colliderRef->init(effectResource, &nativeEffect.getCollider(i), &particleEngine);
			colliders[nativeEffect.getCollider(i).name] = colliderRef;
		}

		for(uint32_t i = 0; i < nativeEffect.getNumSprites(); i++) {
			Ref<PixelpartSprite> spriteRef;
			spriteRef.instance();
			spriteRef->init(effectResource, &nativeEffect.getSprite(i), &particleEngine);
			sprites[nativeEffect.getSprite(i).name] = spriteRef;	
		}

		for(uint32_t i = 0; i < nativeEffect.getNumParticleEmitters(); i++) {
			const pixelpart::ParticleEmitter& emitter = nativeEffect.getParticleEmitterByIndex(i);
			const pixelpart::ImageResource& imageResource = effectResource->get_project_resources().images.at(emitter.particleSprite.id);

			PoolByteArray imageData;
			imageData.resize(imageResource.data.size());
			memcpy(imageData.write().ptr(), imageResource.data.data(), imageResource.data.size());

			Ref<Image> image;
			image.instance();
			image->create_from_data(imageResource.width, imageResource.height, false, Image::FORMAT_RGBA8, imageData);

			RID immediate = vs->immediate_create();
			RID instance = vs->instance_create();
			RID material = vs->material_create();
			RID texture = vs->texture_create_from_image(image, Texture::FLAG_FILTER | Texture::FLAG_REPEAT);

			emitterInstances.push_back(InstanceData{
				immediate,
				instance,
				material,
				texture
			});
		}

		for(uint32_t i = 0; i < nativeEffect.getNumSprites(); i++) {
			const pixelpart::Sprite& sprite = nativeEffect.getSprite(i);
			const pixelpart::ImageResource& imageResource = projectResources.images.at(sprite.image.id);

			PoolByteArray imageData;
			imageData.resize(imageResource.data.size());
			memcpy(imageData.write().ptr(), imageResource.data.data(), imageResource.data.size());

			Ref<Image> image;
			image.instance();
			image->create_from_data(imageResource.width, imageResource.height, false, Image::FORMAT_RGBA8, imageData);

			RID immediate = vs->immediate_create();
			RID instance = vs->instance_create();
			RID material = vs->material_create();
			RID texture = vs->texture_create_from_image(image, Texture::FLAG_FILTER);

			spriteInstances.push_back(InstanceData{
				immediate,
				instance,
				material,
				texture
			});
		}
	}
	else {
		Godot::print_error(String("Effect resource not valid"), __FUNCTION__, "", __LINE__);
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
Ref<PixelpartSprite> PixelpartEffect::get_sprite(String name) const {
	CharString nameCharString = name.utf8();
	std::string nameStdString = std::string(nameCharString.get_data(), nameCharString.length());
	
	if(sprites.count(nameStdString)) {
		return sprites.at(nameStdString);
	}

	return Ref<PixelpartSprite>();
}
Ref<PixelpartParticleEmitter> PixelpartEffect::get_particle_emitter_by_id(int id) const {
	if(id >= 0 && nativeEffect.hasParticleEmitter(static_cast<uint32_t>(id))) {
		std::string name = nativeEffect.getParticleEmitter(static_cast<uint32_t>(id)).name;

		if(particleEmitters.count(name)) {
			return particleEmitters.at(name);
		}
	}

	return Ref<PixelpartParticleEmitter>();
}
Ref<PixelpartParticleEmitter> PixelpartEffect::get_particle_emitter_by_index(int index) const {
	if(index >= 0 && static_cast<uint32_t>(index) < nativeEffect.getNumParticleEmitters()) {
		std::string name = nativeEffect.getParticleEmitterByIndex(static_cast<uint32_t>(index)).name;

		if(particleEmitters.count(name)) {
			return particleEmitters.at(name);
		}
	}

	return Ref<PixelpartParticleEmitter>();
}
Ref<PixelpartForceField> PixelpartEffect::get_force_field_by_index(int index) const {
	if(index >= 0 && nativeEffect.hasForceField(static_cast<uint32_t>(index))) {
		std::string name = nativeEffect.getForceField(static_cast<uint32_t>(index)).name;
		
		if(forceFields.count(name)) {
			return forceFields.at(name);
		}
	}

	return Ref<PixelpartForceField>();
}
Ref<PixelpartCollider> PixelpartEffect::get_collider_by_index(int index) const {
	if(index >= 0 && nativeEffect.hasCollider(static_cast<uint32_t>(index))) {
		std::string name = nativeEffect.getCollider(static_cast<uint32_t>(index)).name;
		
		if(colliders.count(name)) {
			return colliders.at(name);
		}
	}

	return Ref<PixelpartCollider>();
}
Ref<PixelpartSprite> PixelpartEffect::get_sprite_by_index(int index) const {
	if(index >= 0 && nativeEffect.hasSprite(static_cast<uint32_t>(index))) {
		std::string name = nativeEffect.getSprite(static_cast<uint32_t>(index)).name;
		
		if(sprites.count(name)) {
			return sprites.at(name);
		}
	}

	return Ref<PixelpartSprite>();
}

void PixelpartEffect::draw_emitter3d(const pixelpart::ParticleEmitter& emitter, RID instance, RID immediate, RID material, RID spriteTexture) {
	VisualServer* vs = VisualServer::get_singleton();

	vs->immediate_clear(immediate);

	if(is_visible() && emitter.visible) {
		const pixelpart::Effect* effect = particleEngine.getEffect();
		const uint32_t emitterIndex = effect->findParticleEmitterById(emitter.id);
		const uint32_t numParticles = particleEngine.getNumParticles(emitterIndex);
		const pixelpart::ParticleData& particles = particleEngine.getParticles(emitterIndex);
		const pixelpart::floatd scaleX = static_cast<pixelpart::floatd>(get_import_scale()) * (flipH ? -1.0 : +1.0);
		const pixelpart::floatd scaleY = static_cast<pixelpart::floatd>(get_import_scale()) * (flipV ? -1.0 : +1.0);

		if(emitter.renderer == pixelpart::ParticleEmitter::RendererType::trail && numParticles > 1) {
			std::vector<pixelpart::vec2d> points(pixelpart::getNumParticleVertices(emitter, numParticles));
			std::vector<pixelpart::vec2d> uvs(pixelpart::getNumParticleVertices(emitter, numParticles));
			std::vector<pixelpart::vec4d> colors(pixelpart::getNumParticleVertices(emitter, numParticles));

			pixelpart::generateParticleTrailTriangleStrip(
				reinterpret_cast<pixelpart::floatd*>(points.data()),
				reinterpret_cast<pixelpart::floatd*>(uvs.data()),
				reinterpret_cast<pixelpart::floatd*>(colors.data()),
				emitter,
				particles,
				numParticles,
				scaleX,
				scaleY);

			vs->immediate_begin(immediate, Mesh::PRIMITIVE_TRIANGLE_STRIP);

			for(uint32_t i = 0; i + 1 < points.size(); i++) {
				vs->immediate_vertex(immediate, pp2gd(pixelpart::vec3d(points[i].x, points[i].y, 0.0)));
				vs->immediate_uv(immediate, pp2gd(uvs[i]));
				vs->immediate_color(immediate, pp2gd(colors[i]));
			}

			vs->immediate_end(immediate);
		}
		else {
			PoolIntArray indexArray;
			PoolVector2Array pointArray;
			PoolVector2Array uvArray;
			PoolColorArray colorArray;
			indexArray.resize(static_cast<int>(numParticles * 6));
			pointArray.resize(static_cast<int>(numParticles * 4));
			uvArray.resize(static_cast<int>(numParticles * 4));
			colorArray.resize(static_cast<int>(numParticles * 4));

			pixelpart::generateParticleSpriteTriangles(
				indexArray.write().ptr(),
				reinterpret_cast<float*>(pointArray.write().ptr()),
				reinterpret_cast<float*>(uvArray.write().ptr()),
				reinterpret_cast<float*>(colorArray.write().ptr()),
				emitter,
				particles,
				numParticles,
				scaleX,
				scaleY);

			vs->immediate_begin(immediate, Mesh::PRIMITIVE_TRIANGLES);

			for(uint32_t p = 0; p < numParticles; p++) {
				vs->immediate_vertex(immediate, Vector3(pointArray[p * 4 + 0].x, pointArray[p * 4 + 0].y, 0.0));
				vs->immediate_uv(immediate, uvArray[p * 4 + 0]);
				vs->immediate_color(immediate, colorArray[p * 4 + 0]);
					
				vs->immediate_vertex(immediate, Vector3(pointArray[p * 4 + 2].x, pointArray[p * 4 + 2].y, 0.0));
				vs->immediate_uv(immediate, uvArray[p * 4 + 2]);
				vs->immediate_color(immediate, colorArray[p * 4 + 1]);
			
				vs->immediate_vertex(immediate,Vector3(pointArray[p * 4 + 1].x, pointArray[p * 4 + 1].y, 0.0));
				vs->immediate_uv(immediate, uvArray[p * 4 + 1]);
				vs->immediate_color(immediate, colorArray[p * 4 + 1]);	
				
				vs->immediate_vertex(immediate, Vector3(pointArray[p * 4 + 0].x, pointArray[p * 4 + 0].y, 0.0));
				vs->immediate_uv(immediate, uvArray[p * 4 + 0]);
				vs->immediate_color(immediate, colorArray[p * 4 + 0]);

				vs->immediate_vertex(immediate, Vector3(pointArray[p * 4 + 3].x, pointArray[p * 4 + 3].y, 0.0));
				vs->immediate_uv(immediate, uvArray[p * 4 + 3]);
				vs->immediate_color(immediate, colorArray[p * 4 + 3]);
					
				vs->immediate_vertex(immediate, Vector3(pointArray[p * 4 + 2].x, pointArray[p * 4 + 2].y, 0.0));
				vs->immediate_uv(immediate, uvArray[p * 4 + 2]);
				vs->immediate_color(immediate, colorArray[p * 4 + 2]);
			}

			vs->immediate_end(immediate);
		}

		vs->material_set_shader(material, PixelpartShaders::get_instance()->get_shader_spatial(emitter.blendMode));
		vs->material_set_param(material, "u_Texture0", spriteTexture);
		vs->material_set_param(material, "u_ColorMode", static_cast<int>(emitter.colorMode));
		vs->material_set_param(material, "u_AlphaThreshold", emitter.alphaThreshold);

		vs->instance_set_base(instance, immediate);
		vs->instance_set_scenario(instance, get_world()->get_scenario());
		vs->instance_set_transform(instance, get_global_transform());
		vs->instance_geometry_set_material_override(instance, material);
	}
}
void PixelpartEffect::draw_sprite3d(const pixelpart::Sprite& sprite, RID instance, RID immediate, RID material, RID spriteTexture) {
	VisualServer* vs = VisualServer::get_singleton();

	vs->immediate_clear(immediate);

	if(is_visible() && sprite.visible &&
	particleEngine.getTime() >= sprite.lifetimeStart &&
	(particleEngine.getTime() <= sprite.lifetimeStart + sprite.lifetimeDuration || sprite.repeat)) {
		const pixelpart::ImageResource& imageResource = effectResource->get_project_resources().images.at(sprite.image.id);
		const pixelpart::floatd scaleX = static_cast<pixelpart::floatd>(get_import_scale()) * (flipH ? -1.0 : +1.0);
		const pixelpart::floatd scaleY = static_cast<pixelpart::floatd>(get_import_scale()) * (flipV ? -1.0 : +1.0);
		
		Vector2 points[4];
		Vector2 uvs[4];
		Color colors[4];
		pixelpart::generateSpriteTriangles<int, float>(
			nullptr,
			reinterpret_cast<float*>(points),
			reinterpret_cast<float*>(uvs),
			reinterpret_cast<float*>(colors),
			sprite,
			static_cast<pixelpart::floatd>(imageResource.width) / static_cast<pixelpart::floatd>(imageResource.height),
			particleEngine.getTime(),
			scaleX,
			scaleY);
		
		vs->immediate_begin(immediate, Mesh::PRIMITIVE_TRIANGLES);

		vs->immediate_vertex(immediate, Vector3(points[0].x, points[0].y, 0.0f));
		vs->immediate_uv(immediate, uvs[0]);
		vs->immediate_color(immediate, colors[0]);

		vs->immediate_vertex(immediate, Vector3(points[1].x, points[1].y, 0.0f));
		vs->immediate_uv(immediate, uvs[1]);
		vs->immediate_color(immediate, colors[1]);

		vs->immediate_vertex(immediate, Vector3(points[3].x, points[3].y, 0.0f));
		vs->immediate_uv(immediate, uvs[3]);
		vs->immediate_color(immediate, colors[3]);

		vs->immediate_vertex(immediate, Vector3(points[1].x, points[1].y, 0.0f));
		vs->immediate_uv(immediate, uvs[1]);
		vs->immediate_color(immediate, colors[1]);

		vs->immediate_vertex(immediate, Vector3(points[2].x, points[2].y, 0.0f));
		vs->immediate_uv(immediate, uvs[2]);
		vs->immediate_color(immediate, colors[2]);

		vs->immediate_vertex(immediate, Vector3(points[3].x, points[3].y, 0.0f));
		vs->immediate_uv(immediate, uvs[3]);
		vs->immediate_color(immediate, colors[3]);
	
		vs->immediate_end(immediate);

		vs->material_set_shader(material, PixelpartShaders::get_instance()->get_shader_spatial(sprite.blendMode));
		vs->material_set_param(material, "u_Texture0", spriteTexture);
		vs->material_set_param(material, "u_ColorMode", static_cast<int>(sprite.colorMode));
		vs->material_set_param(material, "u_AlphaThreshold", 0.0f);

		vs->instance_set_base(instance, immediate);
		vs->instance_set_scenario(instance, get_world()->get_scenario());
		vs->instance_set_transform(instance, get_global_transform());
		vs->instance_geometry_set_material_override(instance, material);
	}
}
}