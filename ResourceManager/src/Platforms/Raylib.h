#pragma once

#include "../Namespace.h"

#include <string>

namespace RaylibPlatform 
{
	Expr FormatColor(const Expr& expr); 	
	std::string DefineColor(); 	

	Expr FormatVector2f(const Expr& expr); 	
	std::string DefineVector2f();
}
