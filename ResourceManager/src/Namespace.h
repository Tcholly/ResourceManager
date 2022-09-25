#pragma once

#include <string>
#include <vector>

struct Expr
{
	std::string name;
	std::string type;
	std::string value;
};

struct Namespace
{
	std::string name;
	std::vector<Expr> exprs;
	std::vector<Namespace*> namespaces;

	~Namespace()
	{
		for (auto ns : namespaces)
			delete ns;

		exprs.clear();
		namespaces.clear();
		name = "";
	}
};
