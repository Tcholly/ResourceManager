// This file is auto generated
#pragma once

#include <string>

#ifndef RL_COLOR_TYPE
	// Color, 4 components, R8G8B8A8 (32bit)
	typedef struct Color
	{
		unsigned char r;        // Color red value
		unsigned char g;        // Color green value
		unsigned char b;        // Color blue value
		unsigned char a;        // Color alpha value
	} Color;
	#define RL_COLOR_TYPE
#endif

#ifndef RL_VECTOR2_TYPE
	typedef struct Vector2
	{
		float x;
		float y;
	} Vector2;
	#define RL_COLOR_TYPE
#endif

#ifndef PI
	#define PI 3.14159265358979323846f
#endif
#ifndef E
	#define E 2.71828182845904523536f
#endif

namespace Textures
{
	namespace Filepaths
	{
		const std::string DOG = "Assets/Dog.png";
		const Color CAT = {129, 162, 242, 255};
	} // namespace Filepaths

} // namespace Textures

namespace Globals
{
	namespace Vars
	{
		const int TARGET_FPS = 60;
	} // namespace Vars

	const Vector2 VEC = {0.0f,0.2f};
} // namespace Globals

