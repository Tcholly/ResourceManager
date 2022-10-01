#include "NoPlatform.h"
#include <fmt/core.h>


namespace NoPlatform 
{
	Expr FormatColor(const Expr& colorExpr)
	{
		Expr result;
		result.name  = colorExpr.name;
		result.type  = colorExpr.type;
		result.value = "0x" + colorExpr.value.substr(1);

		return result;
	}
	
	std::string DefineColor()
	{
		std::string result;

		result += "#ifndef COLOR_TYPE\n";
		result += "\ttypedef unsigned int color;\n";
		result += "\t#define COLOR_TYPE\n";
		result += "#endif\n";

		return result;
	}

	Expr FormatVector2f(const Expr& expr)
	{
		Expr result;

		result.name = expr.name;
		result.type = expr.type; 
		result.value = expr.value;

		return result;
	}

	std::string DefineVector2f()
	{
		std::string result;
		
		result += "#ifndef VECTOR2F_TYPE\n";
		result += "\ttypedef struct vector2f\n";
		result += "\t{\n";
		result += "\t\tfloat x, y;\n";
		result += "\t} vector2f;\n";
		result += "\t#define VECTOR2F_TYPE\n";
		result += "#endif\n";

		return result;
	}
}
