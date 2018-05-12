@echo off
copy "C:\Users\muddasheep\Documents\hqtrilogy\SpiritSource18\dlls\Releasehl\spirit.dll" "C:\Program Files (x86)\Steam\steamapps\common\Half-Life\hqtrilogy\dlls\spirit.dll"
copy "C:\Users\muddasheep\Documents\hqtrilogy\SpiritSource18\cl_dll\Release\client.dll" "C:\Program Files (x86)\Steam\steamapps\common\Half-Life\hqtrilogy\cl_dlls\client.dll"

copy "C:\Users\muddasheep\Documents\hqtrilogy\SpiritSource18\dlls\Releasehl\spirit.dll" "C:\Users\muddasheep\Documents\hqtrilogy\build\hqtrilogy\dlls\spirit.dll"
copy "C:\Users\muddasheep\Documents\hqtrilogy\SpiritSource18\cl_dll\Release\client.dll" "C:\Users\muddasheep\Documents\hqtrilogy\build\hqtrilogy\cl_dlls\client.dll"

echo Starting %1.bsp ...
"C:\Program Files (x86)\Steam\steamapps\common\Half-Life\hl.exe" -console -game hqtrilogy -dev +map %1