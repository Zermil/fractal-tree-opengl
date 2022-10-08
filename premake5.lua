workspace "fractal-tree-opengl"
  startproject "fractal-tree-opengl"
  architecture "x64"
  
  configurations 
  {
    "Debug",
    "Release"
  }
  
externals = {}
externals["glew"] = "deps/GLEW"
externals["glfw"] = "deps/GLFW"

project "fractal-tree-opengl"
  location "fractal-tree-opengl"
  kind "ConsoleApp"
  language "C++"
  cppdialect "C++latest"
  targetdir "./build"
  debugdir "./build"
  
  files
  {
    "./code/**.hpp",
    "./code/**.cpp",
    "./code/**.h",
    "./code/**.c"
  }

  includedirs
  {
    "%{externals.glew}/include",
    "%{externals.glfw}/include"
  }

  libdirs
  {
    "%{externals.glew}/lib",
    "%{externals.glfw}/lib"
  }

  links
  {
    "glew32s.lib",
    "glfw3.lib",
    "Shell32.lib",
    "Gdi32.lib",
    "opengl32.lib"
  }

  filter { "configurations:Debug" }
    defines { "DEBUG" }
    runtime "Debug"
    symbols "on"

  filter { "configurations:Release" }
    defines { "NDEBUG" }
    entrypoint "mainCRTStartup"
    kind "WindowedApp"
    runtime "Release"
    symbols "off"
    optimize "on"