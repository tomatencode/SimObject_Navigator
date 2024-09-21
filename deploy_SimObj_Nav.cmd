set Target_Folder="\\le.grp\dept\ITP_Public\SMILE\Tools\EXPERIMENTAL_SimObject_Navigator"
rem set Target_Folder="C:\Dev\simobjnav"
rem set Qt_Executable="C:\Dev\SimObject_Navigator\out\build\x64-Release\SimObject_Navigator.exe"
rem Works only when build with QT Creator - I have no idea why...
set Qt_Executable="C:\Dev\SimObject_Navigator\build-SimObject_Navigator-Desktop_Qt_6_5_1_MSVC_64_bit-Release\SimObject_Navigator.exe"

copy /y %Qt_Executable%  %Target_Folder%\SimObject_Navigator.exe
set PATH=%PATH%;C:\Dev\Qt\6.4.0\msvc2019_64\bin
C:\Dev\Qt\6.3.1\msvc2019_64\bin\windeployqt.exe %Target_Folder%\SimObject_Navigator.exe --dir %Target_Folder%

set Target_Folder="C:\Dev\SimObject_Navigator\out\local_deployment"

copy /y %Qt_Executable%  %Target_Folder%\SimObject_Navigator.exe
C:\Dev\Qt\6.3.1\msvc2019_64\bin\windeployqt.exe %Target_Folder%\SimObject_Navigator.exe --dir C:\Dev\SimObject_Navigator\out\local_deployment