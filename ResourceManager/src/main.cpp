#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <fmt/format.h>

#include "Utils/Logger.h"

enum EndingType
{
	NONE,
	NAMESPACE_BEGIN,
	NAMESPACE_END,
	COLON,
	SEMICOLON,
	END_OF_FILE
};

template <> struct fmt::formatter<EndingType> : formatter<string_view> 
{
	template <typename FormatContext>
	auto format(EndingType m, FormatContext& ctx) const -> decltype(ctx.out()) 
	{
		string_view name = "unknown";
		switch (m) 
		{
			case EndingType::NONE:              name = "NONE"; break;
			case EndingType::NAMESPACE_BEGIN:   name = "{";    break;
			case EndingType::NAMESPACE_END:     name = "}";    break;
			case EndingType::COLON:             name = ":";    break;
			case EndingType::SEMICOLON:         name = ";";    break;
			case EndingType::END_OF_FILE:       name = "EOF";  break;
			defautl: break;
		}
		return formatter<string_view>::format(name, ctx);
	}
};

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


struct Args
{
	int argc;
	char** argv;
};

std::string GetNextArg(Args& args)
{
	if (args.argc < 1)
		return "";
	auto arg = *args.argv++;
	args.argc--;

	return arg;
}


std::string trim(const std::string& str)
{
    size_t first = str.find_first_not_of(" \t");
    if (std::string::npos == first)
    {
        return str;
    }
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, (last - first + 1));
}

std::string GetOneLineContent(std::stringstream& ss)
{
	std::string temp;
	std::string line;
	while (std::getline(ss, line))
	{
		line = trim(line);
		temp += line;
	}

	std::stringstream finalStream(temp);

	std::string result;
	bool inString = false;
	char ch;
	while (finalStream.get(ch))
	{
		if (ch == '\"')
			inString = !inString;
		else if (ch == ' ' && !inString)
			continue;

		result += ch;
	}

	return result;
}

std::pair<std::string, EndingType> ParseNext(std::stringstream& ss)
{
	std::string result; 
	char ch;
	while (ss.get(ch))
	{
		if (ch == '{')
			return {result, NAMESPACE_BEGIN};
		if (ch == '}')
			return {result, NAMESPACE_END};
		if (ch == ':')
			return {result, COLON};
		if (ch == ';')
			return {result, SEMICOLON};

		result += ch;
	}

	return {result, END_OF_FILE};
}

Expr GetExpr(std::stringstream& ss, std::string name)
{
	auto type = ParseNext(ss);
	if (type.second != EndingType::COLON)
	{
		Logger::Error("Unknown expression: {} : {} {}", name, type.first, type.second);
		exit(1);
	}

	auto value = ParseNext(ss);
	if (value.second != EndingType::SEMICOLON)
	{
		Logger::Error("Unknown expression: {} : {} {}", name, type.first, type.second);
		exit(1);
	}	

	return {name, type.first, value.first};
}

Namespace* GetNamespace(std::stringstream& ss, std::string name)
{
	Namespace* result = new Namespace;
	result->name = name;

	while (!ss.eof())
	{
		auto token = ParseNext(ss);
		switch (token.second)
		{
			case NAMESPACE_BEGIN:
			{
				Namespace* ns = GetNamespace(ss, token.first);
				result->namespaces.emplace_back(ns);
				break;
			}
				
			case NAMESPACE_END:
				return result;
				break;

			case COLON:
			{
				Expr expr = GetExpr(ss, token.first);
				result->exprs.emplace_back(expr);
				break;
			}

			case SEMICOLON:
				Logger::Warn("Found useless semicolon");
				break;

			default:
				break;
		}	
	}
	
	return result;
}


void ExportNamespaceToHpp(std::ofstream& out, Namespace* root, int level)
{
	if (level == 0)
	{
		out << "// This file is auto generated\n";
		out << "#pragma once\n\n";
		out << "#include <string>\n\n";
	}
	else
	{
		out << std::string(level - 1, '\t') << "namespace " << root->name << '\n';
		out << std::string(level - 1, '\t') << '{' << '\n';  
	}

	for (auto ns : root->namespaces)
		ExportNamespaceToHpp(out, ns, level + 1);

	for (auto expr : root->exprs)
	{
		out << std::string(level, '\t');
		out << "const ";
		if (expr.type == "string")
			out << "std::string ";
		else
			out << expr.type << ' ';
		out << expr.name << " = " << expr.value << ";\n";  
	}

	if (level > 0)
		out << std::string(level - 1, '\t') << "} // " << root->name << "\n\n";

}



void PrintNamespace(Namespace* ns, int level)
{
	if (!ns->name.empty())
	{
		std::cout << fmt::format("{:>{}}\n", ns->name, (level - 1) * 4 + ns->name.size());
		std::cout << fmt::format("{:>{}}\n", "{", (level - 1) * 4 + 1);
	}

	for (auto child_ns : ns->namespaces)
		PrintNamespace(child_ns, level + 1);

	for (auto expr : ns->exprs)
		std::cout << fmt::format("{:>{}} : {} : {};\n", expr.name, (level) * 4 + expr.name.size(), expr.type, expr.value);

	if (!ns->name.empty())
	{
		std::cout << fmt::format("{:>{}}\n", "}", (level - 1) * 4 + 1);
	}
}


void Usage(std::string program_name)
{
	std::cout << "USAGE:" << '\n';
	std::cout << "       " << program_name << " <FILE> [OPTIONS]\n";
	std::cout << "OPTIONS:\n";
	std::cout << "       -o   --output  <file>    Uses the given path to output the exported hpp code\n";
}


int main(int argc, char** argv)
{
	Args args = {argc, argv};
	std::string program = GetNextArg(args);
	std::string file = GetNextArg(args);
	std::string outputFile = "out.hpp";

	std::string nextArg = GetNextArg(args);
	if (!nextArg.empty())
	{
		if (nextArg == "-o" || nextArg == "--output")
		{
			nextArg = GetNextArg(args);
			if (nextArg.empty())
			{
				Logger::Error("Missing argument for --output");
				Usage(program);
				exit(1);
			}
			outputFile = nextArg;
		}
		else
		{
			Logger::Error("Unknown Argument {}", nextArg);
			Usage(program);
			exit(1);
		}
	}

	if (file.empty())
	{
		Logger::Error("Missing argument");
		Usage(program);
		exit(1);
	}

	std::ifstream in(file);
	if (!in)
	{
		Logger::Error("Failed to open {}: {}", file, strerror(errno));	
		exit(1);
	}

	std::stringstream ss;
	ss << in.rdbuf();
	in.close();

	std::string content = GetOneLineContent(ss);

	ss.str(std::string());
	ss.clear();
	ss.str(content);

	std::cout << ss.str() << '\n';

	Namespace* root = GetNamespace(ss, "");

	ss.str(std::string());
	ss.clear();

	// PrintNamespace(root, 0);

	std::ofstream out(outputFile);
	if (!out)
	{
		Logger::Error("Failed to open {}: {}", outputFile, strerror(errno));	
		exit(1);
	}

	ExportNamespaceToHpp(out, root, 0);
	out.close();

	std::cout << "Successfully exported " << file << " to " << outputFile << '\n';

	delete root;
}
