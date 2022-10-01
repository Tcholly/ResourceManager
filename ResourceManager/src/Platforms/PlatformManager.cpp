#include "PlatformManager.h"

#include "Raylib.h"
#include "NoPlatform.h"

#include "../Utils/Logger.h"


namespace PlatformManager
{
	static Platform platform = Platform::NONE;


	void SetPlatform(Platform _platform)
	{
		platform = _platform;
	}

	Expr FormatColor(const Expr& colorExpr)
	{
		switch (platform)
		{
			case Platform::RAYLIB:
				return RaylibPlatform::FormatColor(colorExpr);

			case Platform::NONE:
				return NoPlatform::FormatColor(colorExpr);

			default:
				Logger::Error("UNREACHABLE FormatColor default");
				break;
		}

		return {colorExpr.name, colorExpr.type, colorExpr.value};
	}

	std::string DefineColor()
	{
		switch (platform)
		{
			case Platform::RAYLIB:
				return RaylibPlatform::DefineColor();

			case Platform::NONE:
				return NoPlatform::DefineColor();

			default:
				Logger::Error("UNREACHABLE DefineColor default");
				break;
		}

		return "";
	}


	Expr FormatVector2f(const Expr& vector2fExpr)
	{
		switch (platform)
		{
			case Platform::RAYLIB:
				return RaylibPlatform::FormatVector2f(vector2fExpr);

			case Platform::NONE:
				return NoPlatform::FormatVector2f(vector2fExpr);

			default:
				Logger::Error("UNREACHABLE FormatVector2f default");
				break;
		}

		return {vector2fExpr.name, vector2fExpr.type, vector2fExpr.value};

	}


	std::string DefineVector2f()
	{
		switch (platform)
		{
			case Platform::RAYLIB:
				return RaylibPlatform::DefineVector2f();

			case Platform::NONE:
				return NoPlatform::DefineVector2f();

			default:
				Logger::Error("UNREACHABLE DefineVector2f default");
				break;
		}

		return "";

	}
}
