@ECHO OFF
IF NOT EXIST bin MKDIR bin
cl /MP /W4 /WX /Gy /O2 /incremental /D_CONSOLE /nologo /ISource Source\_TU\_TU.cpp /c /GS- || EXIT /B
LINK /nologo *.obj user32.lib kernel32.lib gdi32.lib dsound.lib d3d9.lib /incremental /SUBSYSTEM:CONSOLE /NODEFAULTLIB /out:bin\Meong.exe || EXIT /B
bin\Meong.exe