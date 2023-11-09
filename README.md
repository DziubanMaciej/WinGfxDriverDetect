# WinGfxDriverDetect
Windows operating system has a very complicated way of storing the location of graphics drivers. It is described by a series of registry keys with multiple levels of indirection. They are possible to follow, although that's very cumbersome and error prone. *WinGfxDriverDetect* is a utility tool able to detect the location by initializing graphics context (namely OpenGL) and querying DLL modules loaded by the process. It's able to detect graphics drivers from AMD and NVIDIA by names of DLL modules.


### Usage

The tool should be called without any arguments. Administrator mode is not required. If the tool is successful in finding the driver, it will return 0 and print the directory to stdout, for example:
```
$ .\GetDriverStore.exe
C:\Windows\System32\DriverStore\FileRepository\nv_dispig.inf_amd64_7e5fd280efaa5445
```

If the tool fails, it will return non-zero value and print errors to stderr, for example:
```
.\GetDriverStore.exe
ERROR: No OpenGL driver found.
```

### Building
*WinGfxDriverDetect* requires CMake and  MSVC compiler to build. It can be built by following commands:
```
mkdir build
cd build
cmake ..
cmake --build . --config Release
```
