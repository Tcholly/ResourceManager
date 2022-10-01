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

	Expr FormatColor(const Expr& colorExpr)
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
	
	std::string DefineColor()
	{
		std::string result;

		result += "#ifndef RL_COLOR_TYPE\n";
		result += "\t// Color, 4 components, R8G8B8A8 (32bit)\n";
		result += "\ttypedef struct Color\n";
		result += "\t{\n";
		result += "\t\tunsigned char r;        // Color red value\n";
		result += "\t\tunsigned char g;        // Color green value\n";
		result += "\t\tunsigned char b;        // Color blue value\n";
		result += "\t\tunsigned char a;        // Color alpha value\n";
		result += "\t} Color;\n";
		result += "\t#define RL_COLOR_TYPE\n";
		result += "#endif\n";

		return result;
	}

	Expr FormatVector2f(const Expr& expr) 	
	{
		Expr result;

		result.name  = expr.name;
		result.type  = "Vector2";
		result.value = expr.value;

		return result;
	}

	std::string DefineVector2f()
	{
		std::string result;

		result += "#ifndef RL_VECTOR2_TYPE\n";
		result += "\ttypedef struct Vector2\n";
		result += "\t{\n";
		result += "\t\tfloat x;\n";
		result += "\t\tfloat y;\n";
		result += "\t} Vector2;\n";
		result += "\t#define RL_COLOR_TYPE\n";
		result += "#endif\n";

		return result;
	}
}
