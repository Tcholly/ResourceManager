# Simple Globals exporter

Export to a header file simple constants.

## Getting Started

```
ResourceManager <file>
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
	var_name : type : value;
}

var_name : type : value;

```

## TODO

- [x] Export file to hpp
- [ ] Export textures and full files instead of the path only
- [ ] Export file to dll
