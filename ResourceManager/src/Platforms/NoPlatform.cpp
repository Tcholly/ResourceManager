#include "NoPlatform.h"
#include <fmt/core.h>


namespace NoPlatform 
{
	static Expr FormatColor(const Expr& colorExpr)
	{
		Expr result;
		result.name  = colorExpr.name;
		result.type  = colorExpr.type;
		result.value = "0x" + colorExpr.value.substr(1);

		return result;
	}
	
	static Expr ReturnSame(const Expr& expr)
	{
		Expr result;

		result.name = expr.name;
		result.type = expr.type; 
		result.value = expr.value;

		return result;
	}

	PlatformData GetPlatformData()
	{
		PlatformData result;

		TypeInfo colorInfo;
		colorInfo.name = "color";
		colorInfo.definition += "#ifndef COLOR_TYPE\n";
		colorInfo.definition += "\ttypedef unsigned int color;\n";
		colorInfo.definition += "\t#define COLOR_TYPE\n";
		colorInfo.definition += "#endif\n";
		colorInfo.FormatType = FormatColor;

		TypeInfo vector2fInfo;
		vector2fInfo.name = "vector2f";
		vector2fInfo.definition += "#ifndef VECTOR2F_TYPE\n";
		vector2fInfo.definition += "\ttypedef struct vector2f\n";
		vector2fInfo.definition += "\t{\n";
		vector2fInfo.definition += "\t\tfloat x, y;\n";
		vector2fInfo.definition += "\t} vector2f;\n";
		vector2fInfo.definition += "\t#define VECTOR2F_TYPE\n";
		vector2fInfo.definition += "#endif\n";
		vector2fInfo.FormatType = ReturnSame;

		TypeInfo vector2iInfo;
		vector2iInfo.name = "vector2i";
		vector2iInfo.definition += "#ifndef VECTOR2I_TYPE\n";
		vector2iInfo.definition += "\ttypedef struct vector2i\n";
		vector2iInfo.definition += "\t{\n";
		vector2iInfo.definition += "\t\tint x, y;\n";
		vector2iInfo.definition += "\t} vector2i;\n";
		vector2iInfo.definition += "\t#define VECTOR2I_TYPE\n";
		vector2iInfo.definition += "#endif\n";
		vector2iInfo.FormatType = ReturnSame;

		result.data[Type::COLOR] = colorInfo;
		result.data[Type::VECTOR2F] = vector2fInfo;
		result.data[Type::VECTOR2I] = vector2iInfo;

		return result;
	}
}
