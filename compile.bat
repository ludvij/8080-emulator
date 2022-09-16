@REM to lazy to write a makefile or use vs
g++ .\src\main.cpp .\src\Dissasembler.cpp ^
-std=c++17 -Wall -Wextra -Werror ^
-DDEBUG -DNOP_ON_UNSUPPORTED_OPCODE ^
-o dissasembler.exe