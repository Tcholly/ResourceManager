#include "Raylib.h"
#include <fmt/core.h>
#include "../Utils/Logger.h"

static int ParseHexDigit(char hex)
{
	if (hex > 47 && hex < 58)
		return hex - 48;
	if (hex > 64 && hex < 71)
		return 10 + hex - 65;
	if (hex > 96 && hex < 103)
		return 10 + hex - 97;

	return -1;
}

static int ParseHexNumber(const std::string& hex)
{
	int result = 0;

	for (char ch : hex)
	{
		int digit = ParseHexDigit(ch);
		if (digit < 0)
			return -1;
		
		result = result * 0x10 + digit;
	}

	return result;
}


namespace RaylibPlatform
{
	static Expr FormatColor(const Expr& colorExpr)
	{
		Expr result;
		result.name  = colorExpr.name;
		result.type  = "Color";

		std::string hex = colorExpr.value.substr(1);
		if (hex.size() != 8)
		{
			Logger::Error("Unkown color format for {} : {} : {}: Hex number is too big", colorExpr.name, colorExpr.type, colorExpr.value);
			result.value = colorExpr.value;
			return result;
		}

		std::string aStr = hex.substr(0, 2);
		std::string rStr = hex.substr(2, 2);
		std::string gStr = hex.substr(4, 2);
		std::string bStr = hex.substr(6, 2);
		
		int a = ParseHexNumber(aStr);
		int r = ParseHexNumber(rStr);
		int g = ParseHexNumber(gStr);
		int b = ParseHexNumber(bStr);

		if (a < 0 || r < 0 || g < 0 || b < 0)
		{
			Logger::Error("Unkown color format for {} : {} : {}: Failed to convert hex", colorExpr.name, colorExpr.type, colorExpr.value);
			result.value = colorExpr.value;
			return result;
		}

		result.value = fmt::format("{{{}, {}, {}, {}}}", r, g, b, a);

		return result;
	}
	
	static Expr FormatVector2(const Expr& expr) 	
	{
		Expr result;

		result.name  = expr.name;
		result.type  = "Vector2";
		result.value = expr.value;

		return result;
	}

	PlatformData GetPlatformData()
	{
		PlatformData result;

		TypeInfo colorInfo;
		colorInfo.name = "Color";
		colorInfo.definition += "#ifndef RL_COLOR_TYPE\n";
		colorInfo.definition += "\t// Color, 4 components, R8G8B8A8 (32bit)\n";
		colorInfo.definition += "\ttypedef struct Color\n";
		colorInfo.definition += "\t{\n";
		colorInfo.definition += "\t\tunsigned char r;        // Color red value\n";
		colorInfo.definition += "\t\tunsigned char g;        // Color green value\n";
		colorInfo.definition += "\t\tunsigned char b;        // Color blue value\n";
		colorInfo.definition += "\t\tunsigned char a;        // Color alpha value\n";
		colorInfo.definition += "\t} Color;\n";
		colorInfo.definition += "\t#define RL_COLOR_TYPE\n";
		colorInfo.definition += "#endif\n";
		colorInfo.FormatType = FormatColor;


		TypeInfo vector2Info;
		vector2Info.name = "Vector2";
		vector2Info.definition += "#ifndef RL_VECTOR2_TYPE\n";
		vector2Info.definition += "\ttypedef struct Vector2\n";
		vector2Info.definition += "\t{\n";
		vector2Info.definition += "\t\tfloat x;\n";
		vector2Info.definition += "\t\tfloat y;\n";
		vector2Info.definition += "\t} Vector2;\n";
		vector2Info.definition += "\t#define RL_COLOR_TYPE\n";
		vector2Info.definition += "#endif\n";
		vector2Info.FormatType = FormatVector2;


		result.data[Type::COLOR] = colorInfo;
		result.data[Type::VECTOR2F] = vector2Info;
		result.data[Type::VECTOR2I] = vector2Info;

		return result;
	}
}
