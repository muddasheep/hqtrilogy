@echo off
copy \programme\hammer\maps\hqtrilogy\%1.map \programme\hammer\compile\%1.map
hlcsg_x64 -hullfile default.hull %1
hlbsp_x64 %1
hlvis_x64 %1
hlrad_x64 %1

echo.
echo %1 finished.
echo.
