REM git clone https://github.com/KhronosGroup/Vulkan-Loader.git
pause

set CMAKE="C:\Program Files\CMake\bin\cmake.exe"

REM CMake generator for Windows:
REM set CM_GEN=-G"Visual Studio 14 2015"
REM set CM_GEN=-G"Visual Studio 14 2015 Win64"
set CM_GEN=-Ax64

REM VisualStudio build configuration:
REM set CM_CONFIG=MinSizeRel
REM set CM_CONFIG=Release
REM set CM_CONFIG=RelWithDebInfo
set CM_CONFIG=Debug

REM CMake build directory
mkdir BUILD
cd BUILD

REM CMake installation directory
REM set CM_INSTALL=%CD%\install
set CM_INSTALL=c:\mew\install\%CM_CONFIG%

%CMAKE% %CM_GEN% -DCMAKE_INSTALL_PREFIX=%CM_INSTALL% -DVULKAN_HEADERS_INSTALL_DIR=%CM_INSTALL% ..
%CMAKE% --build . -j --target install --config %CM_CONFIG%

pause
