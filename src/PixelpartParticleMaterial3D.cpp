#include "PixelpartParticleMaterial3D.h"

namespace godot {
void PixelpartParticleMaterial3D::_register_methods() {
	register_property<PixelpartParticleMaterial3D, bool>("unshaded",
		&PixelpartParticleMaterial3D::set_unshaded, &PixelpartParticleMaterial3D::get_unshaded,
		true,
		GODOT_METHOD_RPC_MODE_DISABLED,
		GODOT_PROPERTY_USAGE_DEFAULT,
		GODOT_PROPERTY_HINT_NONE);
	register_property<PixelpartParticleMaterial3D, bool>("vertex_lighting",
		&PixelpartParticleMaterial3D::set_vertex_lighting, &PixelpartParticleMaterial3D::get_vertex_lighting,
		false,
		GODOT_METHOD_RPC_MODE_DISABLED,
		GODOT_PROPERTY_USAGE_DEFAULT,
		GODOT_PROPERTY_HINT_NONE);
	register_property<PixelpartParticleMaterial3D, int>("diffuse_mode",
		&PixelpartParticleMaterial3D::set_diffuse_mode, &PixelpartParticleMaterial3D::get_diffuse_mode,
		SpatialMaterial::DIFFUSE_BURLEY,
		GODOT_METHOD_RPC_MODE_DISABLED,
		GODOT_PROPERTY_USAGE_DEFAULT,
		GODOT_PROPERTY_HINT_ENUM, "Burley,Lambert,Lambert Wrap,Oren-Nayar,Toon"); // fix
	register_property<PixelpartParticleMaterial3D, int>("specular_mode",
		&PixelpartParticleMaterial3D::set_specular_mode, &PixelpartParticleMaterial3D::get_specular_mode,
		SpatialMaterial::SPECULAR_SCHLICK_GGX,
		GODOT_METHOD_RPC_MODE_DISABLED,
		GODOT_PROPERTY_USAGE_DEFAULT,
		GODOT_PROPERTY_HINT_ENUM, "SchlickGGX,Blinn,Phong,Toon,Disabled");
	register_property<PixelpartParticleMaterial3D, int>("normal_mode",
		&PixelpartParticleMaterial3D::set_normal_mode, &PixelpartParticleMaterial3D::get_normal_mode,
		PARTICLE_NORMAL_MODE_MESH,
		GODOT_METHOD_RPC_MODE_DISABLED,
		GODOT_PROPERTY_USAGE_DEFAULT,
		GODOT_PROPERTY_HINT_ENUM, "Mesh,Static");
	register_property<PixelpartParticleMaterial3D, Vector3>("static_normal",
		&PixelpartParticleMaterial3D::set_static_normal, &PixelpartParticleMaterial3D::get_static_normal,
		Vector3(0.0f, 1.0f, 0.0f),
		GODOT_METHOD_RPC_MODE_DISABLED,
		GODOT_PROPERTY_USAGE_DEFAULT,
		GODOT_PROPERTY_HINT_NONE);
	register_property<PixelpartParticleMaterial3D, float>("metallic",
		&PixelpartParticleMaterial3D::set_metallic, &PixelpartParticleMaterial3D::get_metallic,
		0.0f,
		GODOT_METHOD_RPC_MODE_DISABLED,
		GODOT_PROPERTY_USAGE_DEFAULT,
		GODOT_PROPERTY_HINT_RANGE, "0.0,1.0,0.01");
	register_property<PixelpartParticleMaterial3D, float>("specular",
		&PixelpartParticleMaterial3D::set_specular, &PixelpartParticleMaterial3D::get_specular,
		0.5f,
		GODOT_METHOD_RPC_MODE_DISABLED,
		GODOT_PROPERTY_USAGE_DEFAULT,
		GODOT_PROPERTY_HINT_RANGE, "0.0,1.0,0.01");
	register_property<PixelpartParticleMaterial3D, float>("roughness",
		&PixelpartParticleMaterial3D::set_roughness, &PixelpartParticleMaterial3D::get_roughness,
		1.0f,
		GODOT_METHOD_RPC_MODE_DISABLED,
		GODOT_PROPERTY_USAGE_DEFAULT,
		GODOT_PROPERTY_HINT_RANGE, "0.0,1.0,0.01");

	register_method("_init", &PixelpartParticleMaterial3D::_init);
}

PixelpartParticleMaterial3D::PixelpartParticleMaterial3D() {

}

void PixelpartParticleMaterial3D::_init() {
	unshaded = true;
	vertexLighting = false;
	diffuseMode = SpatialMaterial::DIFFUSE_BURLEY;
	specularMode = SpatialMaterial::SPECULAR_SCHLICK_GGX;
	normalMode = PARTICLE_NORMAL_MODE_MESH;
	staticNormal = Vector3(0.0f, 1.0f, 0.0f);

	metallic = 0.0f;
	specular = 0.5f;
	roughness = 1.0f;

	shaderDirty = true;
}

void PixelpartParticleMaterial3D::set_unshaded(bool mode) {
	shaderDirty |= mode != unshaded;
	unshaded = mode;
}
bool PixelpartParticleMaterial3D::get_unshaded() const {
	return unshaded;
}

void PixelpartParticleMaterial3D::set_vertex_lighting(bool mode) {
	shaderDirty |= mode != vertexLighting;
	vertexLighting = mode;
}
bool PixelpartParticleMaterial3D::get_vertex_lighting() const {
	return vertexLighting;
}

void PixelpartParticleMaterial3D::set_diffuse_mode(int mode) {
	shaderDirty |= mode != static_cast<int>(diffuseMode);
	diffuseMode = static_cast<SpatialMaterial::DiffuseMode>(mode);
}
int PixelpartParticleMaterial3D::get_diffuse_mode() const {
	return static_cast<int>(diffuseMode);
}

void PixelpartParticleMaterial3D::set_specular_mode(int mode) {
	shaderDirty |= mode != static_cast<int>(specularMode);
	specularMode = static_cast<SpatialMaterial::SpecularMode>(mode);
}
int PixelpartParticleMaterial3D::get_specular_mode() const {
	return static_cast<int>(specularMode);
}

void PixelpartParticleMaterial3D::set_normal_mode(int mode) {
	shaderDirty |= mode != static_cast<int>(normalMode);
	normalMode = static_cast<ParticleNormalMode>(mode);
}
void PixelpartParticleMaterial3D::set_static_normal(Vector3 normal) {
	staticNormal = normal;
}
int PixelpartParticleMaterial3D::get_normal_mode() const {
	return static_cast<int>(normalMode);
}
Vector3 PixelpartParticleMaterial3D::get_static_normal() const {
	return staticNormal;
}

void PixelpartParticleMaterial3D::set_metallic(float value) {
	metallic = value;
}
void PixelpartParticleMaterial3D::set_specular(float value) {
	specular = value;
}
void PixelpartParticleMaterial3D::set_roughness(float value) {
	roughness = value;
}
float PixelpartParticleMaterial3D::get_metallic() const {
	return metallic;
}
float PixelpartParticleMaterial3D::get_specular() const {
	return specular;
}
float PixelpartParticleMaterial3D::get_roughness() const {
	return roughness;
}

void PixelpartParticleMaterial3D::set_shader_dirty(bool state) {
	shaderDirty = state;
}
bool PixelpartParticleMaterial3D::is_shader_dirty() const {
	return shaderDirty;
}
}