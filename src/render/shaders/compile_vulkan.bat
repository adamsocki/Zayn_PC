@echo off
set GLSLC_PATH=D:\VulkanSDK\1.3.296.0\bin\glslc.exe
set INPUT_DIR=D:\dev_d\Zayn_PC\src\render\shaders
set OUTPUT_DIR=D:\dev_d\Zayn_PC\src\render\shaders\compiled

if not exist "%OUTPUT_DIR%" (
    mkdir "%OUTPUT_DIR%"
   
    echo Directory "%OUTPUT_DIR%" created.
)

for %%f in (%INPUT_DIR%\*.vert) do (
    "%GLSLC_PATH%" "%%f" -o "%OUTPUT_DIR%\%%~nf_vert.spv"
)

for %%f in (%INPUT_DIR%\*.frag) do (
    "%GLSLC_PATH%" "%%f" -o "%OUTPUT_DIR%\%%~nf_frag.spv"
)

echo Shader compilation complete.
pause

