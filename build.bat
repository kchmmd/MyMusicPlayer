set project_name=MyMusicPlayer
set qt_path=C:\Qt\Qt5.14.2\5.14.2\msvc2017\lib\cmake\Qt5
set windeployqt_path=C:\Qt\Qt5.14.2\5.14.2\msvc2017\bin\windeployqt.exe
set vs_version="Visual Studio 15 2017"
set /p var=wille build %project_name% , Please choice buildType=Debug)(0/1)? 0.Debug 1.Release:
if %var% equ 1 (set buildType=Release) else (set buildType=Debug)
echo will build %project_name% %buildType%
pause
cd ..
mkdir %project_name%_Build_%buildType%
cd %project_name%_Build_%buildType%
cmake ../%project_name%/src -G %vs_version% -DCMAKE_BUILD_TYPE=%buildType% -DQt5_DIR=%qt_path%
cmake --build . --config %buildType%
cd ../%project_name%/%buildType%
%windeployqt_path% %project_name%.exe
pause