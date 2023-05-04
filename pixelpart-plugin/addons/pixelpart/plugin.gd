tool extends EditorPlugin

const plugin_path = "res://addons/pixelpart"
var effect_resource_import_plugin

func _enter_tree():
	var iconCanvasItem = load(plugin_path + "/icon16_canvasitem.png") as Texture
	var iconSpatial = load(plugin_path + "/icon16_spatial.png") as Texture

	add_project_setting("pixelpart/particle_capacity", 10000, TYPE_INT, PROPERTY_HINT_RANGE, "100,100000")

	add_autoload_singleton("PixelpartShaders", plugin_path + "/src/PixelpartShaders.gdns")
	add_custom_type("PixelpartEffectResource", "Resource", load(plugin_path + "/src/PixelpartEffectResource.gdns"), iconCanvasItem)
	add_custom_type("PixelpartEffect2D", "Node2D", load(plugin_path + "/src/PixelpartEffect2D.gdns"), iconCanvasItem)
	add_custom_type("PixelpartEffect", "Spatial", load(plugin_path + "/src/PixelpartEffect.gdns"), iconSpatial)

	effect_resource_import_plugin = load(plugin_path + "/src/PixelpartEffectResourceImportPlugin.gd").new()
	add_import_plugin(effect_resource_import_plugin)

func _exit_tree():
	remove_import_plugin(effect_resource_import_plugin)

	remove_custom_type("PixelpartEffect")
	remove_custom_type("PixelpartEffect2D")
	remove_custom_type("PixelpartEffectResource")
	remove_autoload_singleton("PixelpartShaders")

	remove_project_setting("pixelpart/particle_capacity")

func add_project_setting(name: String, initial_value, type: int, hint: int, hint_string: String) -> void:
	if ProjectSettings.has_setting(name):
		return

	var property_info: Dictionary = {
		"name": name,
		"type": type,
		"hint": hint,
		"hint_string": hint_string
	}

	ProjectSettings.set_setting(name, initial_value)
	ProjectSettings.add_property_info(property_info)
	ProjectSettings.set_initial_value(name, initial_value)

func remove_project_setting(name: String):
	ProjectSettings.clear(name)
