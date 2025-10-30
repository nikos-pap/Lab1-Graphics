@echo off
echo Copying DLLs...
xcopy /Y /D "%~dp0Dependencies\Binaries\slang\*.dll" "%1"
echo Done!