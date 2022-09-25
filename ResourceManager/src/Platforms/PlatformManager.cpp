#include "PlatformManager.h"

#include "Raylib.h"
#include "NoPlatform.h"

#include "../Utils/Logger.h"


namespace PlatformManager
{
	Platform platform = Platform::NONE;


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
}
