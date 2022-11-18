extends EditorImportPlugin

func get_importer_name():
	return "pixelpart.effect"

func get_visible_name():
	return "Pixelpart Effect";

func get_recognized_extensions():
	return ["ppfx", "json"]

func get_import_order():
	return 5

func get_save_extension():
	return "res";

func get_resource_type():
	return "Resource";

enum Presets { DEFAULT }

func get_preset_count():
	return Presets.size()

func get_preset_name(preset):
	match preset:
		Presets.DEFAULT:
			return "Default"
		_:
			return "Unknown"

func get_import_options(preset):
	match preset:
		Presets.DEFAULT:
			return [
				{ "name": "scale", "default_value": 100.0 }
			]
		_:
			return []

func get_option_visibility(option, options):
	return true

func import(source_file, save_path, options, r_platform_variants, r_gen_files):
	var effect = preload("res://addons/pixelpart/src/PixelpartEffectResource.gdns").new()
	effect.import(source_file)
	effect.scale = options.scale

	return ResourceSaver.save("%s.%s" % [save_path, get_save_extension()], effect)
