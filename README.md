# fractal-tree-opengl

Fundamental fractal tree rendered with OpenGL in C/C++ using recursive approach.

It's a demo for people that want to know the "low-level" implementation details, instead of just writing pre-made functions from a framework without much thought behind it.

## Instruction when building for Visual Studio

Execute `build_vs.bat` script:

```console
> build_vs.bat [VERSION]
```

and replace `[VERSION]` with your current Visual Studio version.
For more information: [Click here](https://premake.github.io/docs/Using-Premake)

## Build

Build for release:

```console
> build_release.bat
```

Build for debug:

```console
> build.bat
```

Remember to change `MSVC_PATH` variable inside the build scripts, otherwise it won't be able to execute `cl.exe`