@echo off
copy \programme\hammer\maps\hqtrilogy\%1.map \programme\hammer\compile\%1.map
old_zhlt\hlcsg -texdata 10000 %1
old_zhlt\hlbsp -texdata 10000 %1
old_zhlt\hlvis -texdata 10000 %1
old_zhlt\hlrad -texdata 10000 %1

echo.
echo %1 finished.
echo.
