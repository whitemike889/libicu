# Microsoft Developer Studio Project File - Name="ustdio" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ustdio - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ustdio.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ustdio.mak" CFG="ustdio - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ustdio - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ustdio - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ustdio - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "USTDIO_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /I "\icu\include\internal" /I "\icu\include" /I "..\..\common" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "USTDIO_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 \icu\lib\release\icuuc.lib \icu\lib\release\icui18n.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"..\..\..\bin\Release\ustdio.dll"

!ELSEIF  "$(CFG)" == "ustdio - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\lib\Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "USTDIO_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\common" /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "USTDIO_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ..\..\..\lib\debug\icuuc.lib ..\..\..\lib\debug\icui18n.lib /nologo /dll /debug /machine:I386 /out:"..\..\..\bin\Debug\ustdio.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "ustdio - Win32 Release"
# Name "ustdio - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\locbund.c
# End Source File
# Begin Source File

SOURCE=.\loccache.c
# End Source File
# Begin Source File

SOURCE=.\ufile.c
# End Source File
# Begin Source File

SOURCE=.\ufmt_cmn.c
# End Source File
# Begin Source File

SOURCE=.\uprintf.c
# End Source File
# Begin Source File

SOURCE=.\uprntf_p.c
# End Source File
# Begin Source File

SOURCE=.\uscanf.c
# End Source File
# Begin Source File

SOURCE=.\uscanf_p.c
# End Source File
# Begin Source File

SOURCE=.\uscanset.c
# End Source File
# Begin Source File

SOURCE=.\ustdio.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\locbund.h
# End Source File
# Begin Source File

SOURCE=.\loccache.h
# End Source File
# Begin Source File

SOURCE=.\ufile.h
# End Source File
# Begin Source File

SOURCE=.\ufmt_cmn.h
# End Source File
# Begin Source File

SOURCE=.\uprintf.h
# End Source File
# Begin Source File

SOURCE=.\uprntf_p.h
# End Source File
# Begin Source File

SOURCE=.\uscanf.h
# End Source File
# Begin Source File

SOURCE=.\uscanf_p.h
# End Source File
# Begin Source File

SOURCE=.\uscanset.h
# End Source File
# Begin Source File

SOURCE=.\ustdio.h

!IF  "$(CFG)" == "ustdio - Win32 Release"

# Begin Custom Build
InputPath=.\ustdio.h

"..\..\..\include\ustdio.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy                    ustdio.h                   ..\..\..\include

# End Custom Build

!ELSEIF  "$(CFG)" == "ustdio - Win32 Debug"

# Begin Custom Build
InputPath=.\ustdio.h

"..\..\..\include\unicode\ustdio.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy                    ustdio.h                   ..\..\..\include\unicode

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
