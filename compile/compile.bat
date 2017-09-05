@echo off
copy \programme\hammer\maps\hqtrilogy\%1.map \programme\hammer\compile\%1.map
hlcsg -estimate -texdata 10000 -wadautodetect %1
hlbsp -estimate -texdata 10000 %1
hlvis -estimate -texdata 10000 %1
hlrad -estimate -texdata 10000 %1

echo.
echo %1 finished.
echo.
