#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <filesystem>
#include <chrono>
#include <fmt/format.h>

#include "Utils/Logger.h"
#include "Utils/Timer.h"
#include "Platforms/PlatformManager.h"
#include "Namespace.h"

enum EndingType
{
	NONE,
	BRACES_OPEN,
	BRACES_CLOSE,
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
			case EndingType::NONE:         name = "NONE";        break;
			case EndingType::BRACES_OPEN:  name = "{";           break;
			case EndingType::BRACES_CLOSE: name = "}";           break;
			case EndingType::COLON:        name = ":";           break;
			case EndingType::SEMICOLON:    name = ";";           break;
			case EndingType::END_OF_FILE:  name = "EOF";         break;
			default:                       name = "UNREACHABLE"; break;
		}
		return formatter<string_view>::format(name, ctx);
	}
};

// TODO: Findo how to move in PlatformManager.cpp
template <> struct fmt::formatter<Platform> : formatter<string_view>
{
	template <typename FormatContext>
	auto format(Platform m, FormatContext& ctx) const -> decltype(ctx.out())
	{
		string_view name = "unknown";
		switch (m)
		{
			case Platform::NONE:   name = "NONE";        break;
			case Platform::RAYLIB: name = "RAYLIB";      break;
			default:               name = "UNREACHABLE"; break;
		}
		return formatter<string_view>::format(name, ctx);
	}
};


struct Args
{
	int argc;
	char** argv;
};

static std::string inputFile = "";
static std::vector<std::string> includedFiles;

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

std::string GetOneLineContent(std::stringstream& ss, const std::string& filename)
{
	std::string result;
	std::string line;
	while (std::getline(ss, line))
	{
		line = trim(line);

		std::stringstream finalStream(line);
		bool inString = false;
		bool inChar = false;
		bool inPreprocessor = false;
		std::string preprocessor = "";
		char ch;
		while (finalStream.get(ch))
		{
			if (ch == '\"' && !inChar)
				inString = !inString;
			else if (ch == '\'' && !inString)
				inChar = !inChar;
			else if ((ch == ' ' || ch == '\t') && !inString &&!inChar)
				continue;
			else if (ch == '=' && !inString && !inChar)
				break;
			else if (ch == '$' && !inString && !inChar)
				inPreprocessor = true;

			if (inPreprocessor)
			{
				preprocessor += ch;
				continue;
			}

			result += ch;
		}

		if (inPreprocessor)
		{
			// Handle preprocessor

			std::string includePreprocessor = "$include";
			if (preprocessor.find(includePreprocessor) == 0)
			{
				std::string rest = preprocessor.substr(includePreprocessor.size());

				if (rest.size() > 0)
				{
					if ((rest[0] == '\'' || rest[0] == '"') && (rest[rest.size() - 1] == '\'' || rest[rest.size() - 1] == '"'))
					{
						std::string includePath = rest.substr(1, rest.size() - 2);
						std::filesystem::path filepath(filename);
						filepath = filepath.remove_filename();
						filepath = filepath.append(includePath);
						std::ifstream includeFile(filepath);
						if (!includeFile)
						{
							Logger::Error("Failed to include {}: {}", includePath, std::strerror(errno));
							continue;
						}

						includedFiles.emplace_back(filepath.string());
						
						std::stringstream includess;
						includess << includeFile.rdbuf();
						includeFile.close();

						result += GetOneLineContent(includess, filepath.string());

						includess.clear();
					}
					else
						Logger::Error("Include preprocessor's path must be enclosed in quotes: {}", rest); 
				}
				else
					Logger::Error("Include preprocessor must be accompanied by a path");
			}
			else
				Logger::Error("Unknown preprocessor: {}", preprocessor); 
		}
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
			return {result, BRACES_OPEN};
		if (ch == '}')
			return {result, BRACES_CLOSE};
		if (ch == ':')
			return {result, COLON};
		if (ch == ';')
			return {result, SEMICOLON};

		result += ch;
	}

	return {result, END_OF_FILE};
}

std::string GetValue(std::stringstream& ss, const std::string& name, const std::string& type)
{
	auto value = ParseNext(ss);

	if (value.second == EndingType::BRACES_OPEN)
	{
		value = ParseNext(ss);
		if (value.second == EndingType::BRACES_CLOSE)
		{
			auto end = ParseNext(ss);
			if (end.second != EndingType::SEMICOLON)
			{
				Logger::Error("Unknown expression: {} : {} : {{ {} }} {} {}", name, type, value.first, end.first, end.second);
				Logger::Error("Missing semicolon?");
				exit(1);
			}
			return '{' + value.first + '}';
		}

		return "";
	}

	if (value.second == EndingType::SEMICOLON)
	{
		return value.first;
	}
	else
	{
		Logger::Error("Unknown expression: {} : {} : {} {}", name, type, value.first, value.second);
		Logger::Error("Missing semicolon?");
		exit(1);
	}
	
	Logger::Error("UNREACHABLE: Unknown expression: {} : {} : {} {}", name, type, value.first, value.second);
	exit(1);

	return "";
}

Expr GetExpr(std::stringstream& ss, std::string name)
{
	auto type = ParseNext(ss);
	if (type.second != EndingType::COLON)
	{
		Logger::Error("Unknown expression: {} : {} {}", name, type.first, type.second);
		exit(1);
	}

	std::string value = GetValue(ss, name, type.first);
	if (value.empty())
	{
		Logger::Error("Unvalid value: {} : {} : {}", name, type.first, value);
		exit(1);
	}

	return {name, type.first, value};
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
			case BRACES_OPEN:
			{
				Namespace* ns = GetNamespace(ss, token.first);
				result->namespaces.emplace_back(ns);
				break;
			}
				
			case BRACES_CLOSE:
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

bool DoesNamespaceContainsType(Namespace* root, const std::string& type)
{
	for (auto ns : root->namespaces)
		if (DoesNamespaceContainsType(ns, type))
			return true;

	for (auto expr : root->exprs)
		if (expr.type == type)
			return true;

	return false;
}

void DefineConstantHpp(std::ofstream& out, std::string name, std::string value)
{
	out << "#ifndef " << name << '\n';
	out << "\t#define " << name << ' ' << value << '\n';
	out << "#endif\n";
}

void InitHppFile(std::ofstream& out, Namespace* root)
{
	out << "// This file is auto generated, DO NOT MODIFY (pwease uwu)\n";
	out << "// PLATFORM: " << fmt::format("{}\n", PlatformManager::GetPlatform());
	out << "// SOURCE: " << inputFile << "\n";
	out << "#pragma once\n\n";

	if (DoesNamespaceContainsType(root, "string"))
		out << "#include <string>\n\n";

	if (DoesNamespaceContainsType(root, "color"))
		out << PlatformManager::DefineColor() << '\n';
	
	if (DoesNamespaceContainsType(root, "vector2f"))
		out << PlatformManager::DefineVector2f() << '\n';

	// WARNING: In case of platform such as raylib it repeats the definition
	if (DoesNamespaceContainsType(root, "vector2i"))
		out << PlatformManager::DefineVector2i() << '\n';


	DefineConstantHpp(out, "PI", "3.14159265358979323846f");
	DefineConstantHpp(out, "E",  "2.71828182845904523536f");

	out << '\n';
}

void ExportNamespaceToHpp(std::ofstream& out, Namespace* root, int level)
{
	if (level == 0)
		InitHppFile(out, root);
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
			expr.type = "std::string";
		else if (expr.type == "color")
			expr = PlatformManager::FormatColor(expr);	
		else if (expr.type == "vector2f")
			expr = PlatformManager::FormatVector2f(expr);
		else if (expr.type == "vector2i")
			expr = PlatformManager::FormatVector2i(expr);

		out << expr.type << ' ' << expr.name << " = " << expr.value << ";\n";  
	}

	if (level > 0)
		out << std::string(level - 1, '\t') << "} // namespace " << root->name << "\n\n";
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
	std::cout << "       -o   --output   <file>      Uses the given path to output the exported hpp code\n";
	std::cout << "       -p   --platform <platform>  Uses the given platform for types definition, available platforms:\n";
	std::cout << "                                   rl, raylib\n"; 
}

std::string GetVariable(const std::string& checkString, std::string line)
{
	if (line.find(checkString) != 0)
		return "";

	return line.substr(checkString.size());
}

int main(int argc, char** argv)
{
	Args args = {argc, argv};
	std::string program = GetNextArg(args);
	inputFile= GetNextArg(args);

	ScopeTimer mainTimer("The program took " CONSOLE_YELLOW "{}" CONSOLE_GREEN " seconds");

	if (inputFile == "-h" || inputFile == "--help")
	{
		Usage(program);
		exit(1);
	}


	std::string outputFile = "out.hpp";

	PlatformManager::SetPlatform(Platform::NONE);
	
	std::string nextArg = GetNextArg(args);
	while (!nextArg.empty())
	{
		if (nextArg == "-o" || nextArg == "--output")
		{
			nextArg = GetNextArg(args);
			if (nextArg.empty() || nextArg.find("-") == 0)
			{
				Logger::Error("Missing argument for --output");
				Usage(program);
				exit(1);
			}
			outputFile = nextArg;
		}
		else if (nextArg == "-p" || nextArg == "--platform")
		{
			nextArg = GetNextArg(args);
			if (nextArg.empty() || nextArg.find("-") == 0)
			{
				Logger::Error("Missing argument for --platform");
				Usage(program);
				exit(1);
			}

			if (nextArg == "rl" || nextArg == "raylib")
			{
				PlatformManager::SetPlatform(Platform::RAYLIB);
			}
			else
			{
				Logger::Error("Unknown argument for --platform: {}", nextArg);
				Usage(program);
				exit(1);
			}
		}
		else
		{
			Logger::Error("Unknown flag: {}", nextArg);
			Usage(program);
			exit(1);
		}

		nextArg = GetNextArg(args);
	}

	if (inputFile.empty())
	{
		Logger::Error("Missing argument <FILE>");
		Usage(program);
		exit(1);
	}

	std::ifstream in(inputFile);
	if (!in)
	{
		Logger::Error("Failed to open {}: {}", inputFile, strerror(errno));	
		exit(1);
	}

	includedFiles.emplace_back(inputFile);

	std::stringstream ss;
	ss << in.rdbuf();
	in.close();

	std::string content = GetOneLineContent(ss, inputFile);

	ss.str(std::string());
	ss.clear();

	// WARNING: Does not work if included file is changed
	std::filesystem::path outPath(outputFile);
	if (std::filesystem::exists(outPath))
	{
		bool isUpToDate = true;

		std::ifstream prevOutFile(outputFile);
		if (!prevOutFile)
		{
			Logger::Warn("Failed to scan {}: {}", outputFile, std::strerror(errno));
			Logger::Warn("Rebuilding {}", outputFile);
			prevOutFile.close();
			isUpToDate = false;
		}
		else
		{
			std::string line;
			std::getline(prevOutFile, line);
			std::getline(prevOutFile, line);

			std::string scannedPlatform = GetVariable("// PLATFORM: ", line);
			std::getline(prevOutFile, line);
			std::string scannedSource = GetVariable("// SOURCE: ", line);

			prevOutFile.close();

			if (scannedPlatform == "" || scannedSource == "")
			{
				Logger::Warn("File {} has been modified, please do not modify", outputFile);
				Logger::Warn("Rebuilding {}", outputFile);
				isUpToDate = false;
			}
			else
			{
				if (scannedPlatform != fmt::format("{}", PlatformManager::GetPlatform()) || scannedSource != inputFile)
					isUpToDate = false;
			}
		}

		auto outModifiedTime = std::filesystem::last_write_time(outPath);

		for (auto inFile : includedFiles)
		{
			std::filesystem::path inPath(inFile);
			if (!std::filesystem::exists(inPath))
				isUpToDate = false;

			auto inModifiedTime = std::filesystem::last_write_time(inPath);
			if (outModifiedTime < inModifiedTime)
				isUpToDate = false;
		}

		if (isUpToDate)
		{
			Logger::Info("{} is Up-To-Date", inputFile); 
			exit(0);
		}
	}



	ss.str(content);

	std::cout << ss.str() << '\n';

	Namespace* root = GetNamespace(ss, "");

	ss.str(std::string());
	ss.clear();

	// PrintNamespace(root, 0);

	std::ofstream out(outputFile);
	if (!out)
	{
		Logger::Error("Failed to open {}: {}", outputFile, std::strerror(errno));	
		exit(1);
	}

	ExportNamespaceToHpp(out, root, 0);
	out.close();

	Logger::Info("Successfully exported {} to {}", inputFile, outputFile);

	delete root;
}
