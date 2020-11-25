@echo off
cls
title Subduction Submision
echo Removing old directories
echo.
rm -rf SOURCE
rm -rf TOOLS
rm -f subduction_setup.exe
mkdir SOURCE
mkdir TOOLS
echo.
robocopy ..\src\ SOURCE\ /S /MT /XF *.user /XF *.vcxproj /XF *.orig /XF *.filters /XF *.suo /XF *.sln /XD Test
echo.
echo COPYING HTML
echo.
robocopy ..\res_ui\ SOURCE\hud\ /MT
robocopy ..\res_ui\css\ SOURCE\hud\css\ /MT
echo.
echo Python Scripts
echo.
robocopy ..\res\scripts SOURCE\pyscripts\ /MT
echo.
echo Install Script
echo.
xcopy ..\installer\install.iss SOURCE
echo.
echo Installer
echo.
xcopy ..\installer\subduction_setup.exe .\
echo.
echo Submission Sys
echo.
xcopy submission.bat TOOLS\
cls
echo Completed!
pause
@echo on
