@echo off
cd .\build

if exist .\release\TimeChargeHistograms.ewo (
    del .\release\TimeChargeHistograms.ewo
) else (
    ECHO Plik .ewo nie istnieje, pomijam usuwanie.
)
nmake 

ren release\TimeChargeHistograms.dll TimeChargeHistograms.ewo

copy release\TimeChargeHistograms.ewo C:\Siemens\Automation\WinCC_OA\3.19\bin\widgets\windows-64

cd ..

ECHO.
