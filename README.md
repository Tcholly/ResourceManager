# Simple Globals exporter

Export to a header file simple constants.

## Getting Started

```cmd
$ ResourceManager <file>
$ ResourceManager --help
```

It will export a file to hpp.
The file must look like this:

```
Namespace_name
{
	Namespace_name
	{
		var_name : type : value;
	}
	
	var_name : type : value;
	var_name : type : value; = Comment
	
	= This is a line comment
}

var_name : type : value;

```

Take a look at [test.txt](/test.txt) for more.

## Supported types

Support for:
- basic types (int, float, ...)
- string
- color -> Must be written in #AARRGGBB format.
- vector2f

Other types will simply be copied, for example using `var_name : int : 60` works. For strings there is an automatic conversion from `string` to `std::string`.

# To support
- char (doesn't support single quote)

## TODO

- [x] Export file to hpp
- [ ] Add support for more complex types 
- [ ] Export textures and full files instead of the path only
- [ ] Export file to dll
