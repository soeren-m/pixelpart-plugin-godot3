extends EditorSpatialGizmoPlugin

const plugin_path = "res://addons/pixelpart"
const effect_class_name = "PixelpartEffect"

func _init():
	var gizmo_icon = load(plugin_path + "/pixelpart_gizmo_icon.png") as Texture
	create_icon_material("pixelpart_icon", gizmo_icon)

func get_name():
	return "PixelpartEffectGizmo"

func get_priority():
	return 0

func has_gizmo(spatial):
	if not (spatial is Object):
		return false

	var script = spatial.get_script()
	if not (script is NativeScript):
		return false

	return script.get_class_name() == effect_class_name

func is_selectable_when_hidden():
	return false

func redraw(gizmo):
	gizmo.clear()
	gizmo.add_unscaled_billboard(get_material("pixelpart_icon", gizmo), 0.05)
