@echo off

set BUILD_DIR=vs2022-x64

cmake -S .. -B %BUILD_DIR% -G "Visual Studio 17 2022" -A x64 -DIMCMP_TESTING=OFF

@REM build
@REM cmake --build %BUILD_DIR% --config Debug
@REM cmake --build %BUILD_DIR% --config Release

@REM install
@REM --prefix xxx is optional
@REM cmake --install %BUILD_DIR% --config Debug --prefix d:/lib/xxx
@REM cmake --install %BUILD_DIR% --config Release --prefix d:/lib/xxx

pause