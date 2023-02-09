workspace "ResourceManager"
    language "C++"
    cppdialect "C++17"
    
    architecture "x86_64"
    configurations { "Debug", "Release" }

	warnings "Extra"

    filter { "configurations:Debug" }
        defines { "_DEBUG" }
        symbols "On"

    filter { "configurations:Release" }
        optimize "On"

    filter { }

    targetdir ("bin/%{prj.name}/%{cfg.longname}")
    objdir ("obj/%{prj.name}/%{cfg.longname}")

project "ResourceManager"
    kind "ConsoleApp"
    files "ResourceManager/**"

    includedirs "Dependencies/fmt/%{cfg.system}/include"

    libdirs "Dependencies/fmt/%{cfg.system}/lib"

	links { "fmt" }
