#ifndef PIXELPART_UTIL_H
#define PIXELPART_UTIL_H

#include "Types.h"
#include <Vector2.hpp>
#include <Vector3.hpp>
#include <Color.hpp>

namespace godot {
Vector2 pp2gd(const pixelpart::vec2d& v);
Vector3 pp2gd(const pixelpart::vec3d& v);
Color pp2gd(const pixelpart::vec4d& v);

pixelpart::vec2d gd2pp(const Vector2& v);
pixelpart::vec3d gd2pp(const Vector3& v);
pixelpart::vec4d gd2pp(const Color& v);
}

#endif