for /R %%i in (**.vert **.frag **.comp **.tese **.tesc) do "D:\VulkanSDK\latest\Bin\glslangValidator.exe" -V "%%~i" -o "compiled\%%~ni%%~xi.spv"