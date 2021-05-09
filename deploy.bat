C:\Qt\5.15.1\mingw81_32\bin\windeployqt.exe build\release\AnalyzerViewer.exe --compiler-runtime
::xcopy "resources" "build\release\resources\" /E /C /I /Y
rmdir build\release\.ui /s /q
rmdir build\release\.moc /s /q
rmdir build\release\.obj /s /q
rmdir build\release\.rcc /s /q
pause