#include "PlatformManager.h"

#include "Raylib.h"
#include "NoPlatform.h"

#include "../Utils/Logger.h"

namespace PlatformManager
{
	static Platform platform = Platform::NONE;
	static std::map<Platform, PlatformData> platformsInfo;

	void SetPlatform(Platform _platform)
	{
		platform = _platform;

		if (platformsInfo.find(platform) == platformsInfo.end())
		{
			switch (platform)
			{
				case Platform::NONE:
					platformsInfo[platform] = NoPlatform::GetPlatformData();
					break;
				case Platform::RAYLIB:
					platformsInfo[platform] = RaylibPlatform::GetPlatformData();
					break;
				default:
					Logger::Error("UNREACHABLE SetPlatform default");
					exit(1);
					break;
			}
		}
	}

	Platform GetPlatform()
	{
		return platform;
	}

	Expr FormatColor(const Expr& colorExpr)
	{
		return platformsInfo[platform].data[Type::COLOR].FormatType(colorExpr);
	}

	std::string DefineColor()
	{
		return platformsInfo[platform].data[Type::COLOR].definition;
	}


	Expr FormatVector2f(const Expr& vector2fExpr)
	{
		return platformsInfo[platform].data[Type::VECTOR2F].FormatType(vector2fExpr);
	}


	std::string DefineVector2f()
	{
		return platformsInfo[platform].data[Type::VECTOR2F].definition;
	}
}
