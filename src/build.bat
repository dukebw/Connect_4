@echo off

REM Fix this messy build file

set CommonCompilerFlags=-MT -nologo -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -DHANDMADE_INTERNAL=1 -DHANDMADE_SLOW=1 -DHANDMADE_WIN32=1 -FC -Z7 
set CommonLinkerFlags=SDL2.lib SDL2main.lib

REM TODO - can we just build both with one exe?

IF NOT EXIST ..\build mkdir ..\..\build
pushd ..\build

REM fix this up for 32-bit compiling
REM 32-bit build
REM cl %CommonCompilerFlags% ..\handmade\code\win32_handmade.cpp /link -subsystem:windows,5.1 %CommonLinkerFlags%

REM 64-bit build
REM cl %CommonCompilerFlags% ..\src\connect4.cpp ..\src\sdl2_connect4.cpp ..\src\board.cpp ..\src\gameLogic.cpp ..\src\graphics.cpp ..\src\linkedList.cpp /link %CommonLinkerFlags%
cl /EHsc /MD  ..\src\connect4.cpp ..\src\sdl2_connect4.cpp ..\src\board.cpp ..\src\gameLogic.cpp ..\src\graphics.cpp ..\src\linkedList.cpp /Feconnect4.exe /I w:\vs_dev_lib\SDL2-2.0.3\include /link /LIBPATH:w:\vs_dev_lib\SDL2-2.0.3\lib\x64 SDL2.lib SDL2main.lib /SUBSYSTEM:CONSOLE
REM cl %CommonCompilerFlags% ..\handmade\code\win32_handmade.cpp /link %CommonLinkerFlags%
popd
