@echo off
setlocal

:: Set the configuration options
set CONFIGURATION_RELEASE=Release
set CONFIGURATION_DEBUG=Debug
set PLATFORM=Win32
set SOLUTION_FILE=project_files/CoopAndreas.sln

:: Main script
if "%1"=="" (
    echo Please specify the build configuration: Release or Debug
    goto :eof
)

if "%2"=="" (
    echo Please specify the target: CoopAndreas, CoopAndreas:clean, CoopAndreas:rebuild, server, server:clean, server:rebuild, or all
    goto :eof
)

if "%2"=="all" (
    call :build_all %1
    goto :eof
)

:: Echo Command
echo msbuild %SOLUTION_FILE% /p:Configuration=%CONFIGURATION_RELEASE% /p:Platform=%PLATFORM% /t:%2 

call :build_project %1 %2
goto :eof

:build_all
if "%1"=="Release" (
    echo Building All Release Win32...
    msbuild %SOLUTION_FILE% /p:Configuration=%CONFIGURATION_RELEASE% /p:Platform=%PLATFORM%
    goto :eof
) else if "%1"=="Debug" (
    echo Building All Debug Win32...
    msbuild %SOLUTION_FILE% /p:Configuration=%CONFIGURATION_DEBUG% /p:Platform=%PLATFORM% /p:DefineConstants="_DEV"
    goto :eof
) else (
    echo Invalid build configuration. Use "Release" or "Debug".
    goto :eof
)

:: Function to build the project
:build_project
if "%1"=="Release" (
    echo Building Release Win32...
    msbuild %SOLUTION_FILE% /p:Configuration=%CONFIGURATION_RELEASE% /p:Platform=%PLATFORM% /t:%2
) else if "%1"=="Debug" (
    echo Building Debug Win32...
    msbuild %SOLUTION_FILE% /p:Configuration=%CONFIGURATION_DEBUG% /p:Platform=%PLATFORM% /t:%2
) else (
    echo Invalid build configuration. Use "Release" or "Debug".
    goto :eof
)

goto :eof

endlocal