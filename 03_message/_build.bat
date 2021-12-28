@echo off
"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\14.30.30705\bin\Hostx86\x86\cl.exe" ^
/EHsc ^
Main.cpp ^
/I "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\14.30.30705\include" ^
/I "C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\ucrt" ^
/I "C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\um" ^
/I "C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\shared" ^
/I "C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\winrt" ^
/link ^
/LIBPATH:"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\14.30.30705\lib\x86" ^
/LIBPATH:"C:\Program Files (x86)\Windows Kits\10\lib\10.0.19041.0\ucrt\x86" ^
/LIBPATH:"C:\Program Files (x86)\Windows Kits\10\lib\10.0.19041.0\um\x86"
pause