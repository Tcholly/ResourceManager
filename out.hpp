// This file is auto generated, DO NOT MODIFY (pwease uwu)
// PLATFORM: RAYLIB
// SOURCE: test.txt
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
	#define RL_VECTOR2_TYPE
#endif

#ifndef RL_VECTOR2_TYPE
	typedef struct Vector2
	{
		float x;
		float y;
	} Vector2;
	#define RL_VECTOR2_TYPE
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
		const char BAT = '=';
	} // namespace Filepaths

} // namespace Textures

namespace Included
{
	const int ASD = 1312;
} // namespace Included

namespace Globals
{
	namespace Vars
	{
		const int TARGET_FPS = 60;
	} // namespace Vars

	const Vector2 VEC = {0.0f,0.2f};
	const Vector2 VIC = {1,4};
} // namespace Globals

