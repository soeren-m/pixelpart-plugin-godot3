#include "PixelpartUtil.h"

namespace godot {
Vector2 pp2gd(const pixelpart::vec2d& v) {
	return Vector2(v.x, v.y);
}
Vector3 pp2gd(const pixelpart::vec3d& v) {
	return Vector3(v.x, v.y, v.z);
}
Color pp2gd(const pixelpart::vec4d& v) {
	return Color(v.r, v.g, v.b, v.a);
}

pixelpart::vec2d gd2pp(const Vector2& v) {
	return pixelpart::vec2d(v.x, v.y);
}
pixelpart::vec3d gd2pp(const Vector3& v) {
	return pixelpart::vec3d(v.x, v.y, v.z);
}
pixelpart::vec4d gd2pp(const Color& v) {
	return pixelpart::vec4d(v.r, v.g, v.b, v.a);
}

std::string replace(std::string str, const std::string& to, const std::string& from) {
	if(from.empty()) {
		return str;
	}

	std::size_t pos = 0;
	while((pos = str.find(from, pos)) != std::string::npos) {
		str.replace(pos, from.length(), to);
		pos += to.length();
	}

	return str;
}
}