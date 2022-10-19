#pragma once

#include "../Namespace.h"

#include <string>
#include <map>

enum class Platform
{
	NONE,
	RAYLIB,
};

enum class Type
{
	COLOR,
	VECTOR2F
};

struct TypeInfo
{
	std::string name;
	std::string definition;
	Expr(*FormatType)(const Expr&);
};

struct PlatformData
{
	std::map<Type, TypeInfo> data;
};

namespace PlatformManager
{
	void SetPlatform(Platform platform);
	Platform GetPlatform();

	Expr FormatColor(const Expr& colorExpr); // Format #AARRGGBB
	std::string DefineColor();

	Expr FormatVector2f(const Expr& vector2fExpr);
	std::string DefineVector2f();
}
