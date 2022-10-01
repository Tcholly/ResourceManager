#pragma once

#include "../Namespace.h"

#include <string>

enum class Platform
{
	NONE,
	RAYLIB,
};

namespace PlatformManager
{
	void SetPlatform(Platform platform);

	Expr FormatColor(const Expr& colorExpr); // Format #AARRGGBB
	std::string DefineColor();

	Expr FormatVector2f(const Expr& vector2fExpr);
	std::string DefineVector2f();
}
