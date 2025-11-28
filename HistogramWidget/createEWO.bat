@echo off
cd .\build

if exist .\release\HistogramWidget.ewo (
    del .\release\HistogramWidget.ewo
) else (
    ECHO Plik .ewo nie istnieje, pomijam usuwanie.
)
nmake 

ren release\HistogramWidget.dll HistogramWidget.ewo

copy release\HistogramWidget.ewo C:\Siemens\Automation\WinCC_OA\3.19\bin\widgets\windows-64

cd ..

ECHO.
