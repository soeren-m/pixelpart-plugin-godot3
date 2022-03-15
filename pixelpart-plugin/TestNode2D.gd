extends Node2D

const Pixelpart = preload("res://addons/pixelpart/src/PixelpartUtil.gd")

func _ready():
	var emitter = $SampleEffect.get_particle_emitter_by_index(0)
	
	emitter.get_particle_opacity().clear();
	emitter.get_particle_opacity().add_point(0.0, 1.0)
	emitter.get_particle_opacity().add_point(1.0, 0.0)
