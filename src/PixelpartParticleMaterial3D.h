#ifndef PIXELPART_PARTICLE_MATERIAL_3D_H
#define PIXELPART_PARTICLE_MATERIAL_3D_H

#include "PixelpartShaders.h"
#include "ParticleType.h"
#include <Resource.hpp>

namespace godot {
class PixelpartParticleMaterial3D : public Resource {
	GODOT_CLASS(PixelpartParticleMaterial3D, Resource)

public:
	static void _register_methods();

	PixelpartParticleMaterial3D();

	void _init();

	void set_unshaded(bool mode);
	bool get_unshaded() const;

	void set_vertex_lighting(bool mode);
	bool get_vertex_lighting() const;

	void set_diffuse_mode(int mode);
	int get_diffuse_mode() const;

	void set_specular_mode(int mode);
	int get_specular_mode() const;

	void set_normal_mode(int mode);
	void set_static_normal(Vector3 normal);
	int get_normal_mode() const;
	Vector3 get_static_normal() const;

	void set_metallic(float value);
	void set_specular(float value);
	void set_roughness(float value);
	float get_metallic() const;
	float get_specular() const;
	float get_roughness() const;

	void set_shader_dirty(bool state);
	bool is_shader_dirty() const;

private:
	bool unshaded = true;
	bool vertexLighting = false;
	SpatialMaterial::DiffuseMode diffuseMode = SpatialMaterial::DIFFUSE_BURLEY;
	SpatialMaterial::SpecularMode specularMode = SpatialMaterial::SPECULAR_SCHLICK_GGX;
	ParticleNormalMode normalMode = PARTICLE_NORMAL_MODE_MESH;
	Vector3 staticNormal = Vector3(0.0f, 1.0f, 0.0f);

	float metallic = 0.0f;
	float specular = 0.5f;
	float roughness = 1.0f;

	bool shaderDirty = true;
};
}

#endif