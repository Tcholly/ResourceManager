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
}
