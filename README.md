# WinGfxDriverDetect
Windows operating system has a very complicated way of storing the location of graphics drivers. It is described by a series of registry keys with multiple levels of indirection. They are possible to follow, although that's very cumbersome and error prone. *WinGfxDriverDetect* is a utility tool able to detect the location by initializing graphics APIs and querying DLL modules loaded by the process. Module names are checked against a hardcoded database of driver names by different vendors. The list of known drivers is in [known_drivers.h](known_drivers.h) file. Multi-GPU systems are not supported. If there are multiple drivers of the same type active, the tool may show an arbitrary one.



### Usage

Administrator mode is not required. If called without arguments, the tool will check Vulkan driver location. Alternatively different APIs may be selected with a positional argument, see below. The driver locations will usually be the same, but it's not guaranteed.
```
.\GetDriverStore.exe opencl
.\GetDriverStore.exe opengl
.\GetDriverStore.exe vulkan
.\GetDriverStore.exe dx11
.\GetDriverStore.exe dx12
```

If the tool is successful in finding the driver, it will return 0 and print the directory to stdout, for example:
```
$ .\GetDriverStore.exe
C:\Windows\System32\DriverStore\FileRepository\nv_dispig.inf_amd64_7e5fd280efaa5445
```

If the tool fails due to some error, it will return non-zero value and print errors to stderr, for example:
```
$ .\GetDriverStore.exe
ERROR: vkCreateInstance failed
```

If the tool cannot find the driver among the loaded modules, it's possible the driver dll name is not in known drivers database. In that case the tool will show a helpful message allowing to manually select it.
```
$ .\GetDriverStore.exe vulkan
ERROR: No driver has been found. The drivers are detected based on a hardcoded name, so it's possible GetDriverStore doesn't have the name stored. A total of 20 new modules have been loaded after driver init. Maybe one of them is the driver?
  1. C:\WINDOWS\SYSTEM32\windows.staterepositorycore.dll
  2. C:\WINDOWS\SYSTEM32\directxdatabasehelper.dll
  3. C:\WINDOWS\SYSTEM32\kernel.appcore.dll
  4. C:\WINDOWS\System32\bcryptPrimitives.dll
  5. C:\WINDOWS\System32\DriverStore\FileRepository\u0418637.inf_amd64_4462a7afa694670e\B418564\amdvlk64.dll <--- This looks like a driver
  6. C:\WINDOWS\System32\SETUPAPI.dll
  7. C:\WINDOWS\SYSTEM32\WINMM.dll
  8. C:\WINDOWS\System32\SHELL32.dll
  9. C:\WINDOWS\System32\wintypes.dll
  10. C:\WINDOWS\SYSTEM32\windows.storage.dll
  11. C:\WINDOWS\System32\SHCORE.dll
  12. C:\WINDOWS\System32\shlwapi.dll
  13. C:\WINDOWS\SYSTEM32\dwmapi.dll
  14. C:\WINDOWS\SYSTEM32\DEVOBJ.dll
  15. C:\WINDOWS\System32\WINTRUST.dll
  16. C:\WINDOWS\System32\CRYPT32.dll
  17. C:\WINDOWS\SYSTEM32\MSASN1.dll
  18. C:\WINDOWS\SYSTEM32\BCrypt.dll
  19. C:\WINDOWS\System32\DriverStore\FileRepository\u0418637.inf_amd64_4462a7afa694670e\B418564\amdenc64.dll <--- This looks like a driver
  20. C:\WINDOWS\System32\DriverStore\FileRepository\u0418637.inf_amd64_4462a7afa694670e\B418564\amdihk64.dll <--- This looks like a driver
```

### Building
*WinGfxDriverDetect* requires CMake and MSVC compiler to build. It can be built by following commands:
```
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

Support for certain APIs can be disabled if needed. For example, to build with only DirectX12 support:
```
cmake .. -DSUPPORT_OPENCL=0 -DSUPPORT_OPENGL=0 -DSUPPORT_VULKAN=0 -DSUPPORT_DX11=0 -DSUPPORT_DX12=1
```
