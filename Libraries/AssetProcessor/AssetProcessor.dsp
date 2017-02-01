# Microsoft Developer Studio Project File - Name="AssetProcessor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=AssetProcessor - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AssetProcessor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AssetProcessor.mak" CFG="AssetProcessor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AssetProcessor - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AssetProcessor - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "Perforce Project"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AssetProcessor - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "Include" /I "..\..\Common\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1009 /d "NDEBUG"
# ADD RSC /l 0x1009 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386

!ELSEIF  "$(CFG)" == "AssetProcessor - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "Include" /I "..\..\Common\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1009 /d "_DEBUG"
# ADD RSC /l 0x1009 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 user32.lib gdi32.lib ..\CodecLib\Debug\CodecLib.lib ..\UtilLib\Debug\UtilLib.lib ..\3rdParty\JPEG6b\Debug\JPEG6b.lib ..\3rdParty\zlib\Win32_DLL_Debug\zlib1d.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libc" /def:"AssetProcessor.def" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "AssetProcessor - Win32 Release"
# Name "AssetProcessor - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ExAPBitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\ExAPBitmap16bpch.cpp
# End Source File
# Begin Source File

SOURCE=.\ExAPBitmap1BitBlackWhite.cpp
# End Source File
# Begin Source File

SOURCE=.\ExAPBitmap8BitPaletted.cpp
# End Source File
# Begin Source File

SOURCE=.\ExAPBitmap8bpch.cpp
# End Source File
# Begin Source File

SOURCE=.\ExAPBitmap8bpchNoAlpha.cpp
# End Source File
# Begin Source File

SOURCE=.\ExAPBitmap8bpchWithAlpha.cpp
# End Source File
# Begin Source File

SOURCE=.\ExAPBitmapCMYK32.cpp
# End Source File
# Begin Source File

SOURCE=.\ExAPBitmapDither.cpp
# End Source File
# Begin Source File

SOURCE=.\ExAPBitmapDitherBayer.cpp
# End Source File
# Begin Source File

SOURCE=.\ExAPBitmapDitherErrorDiffusion.cpp
# End Source File
# Begin Source File

SOURCE=.\ExAPBitmapDitherNone.cpp
# End Source File
# Begin Source File

SOURCE=.\ExAPBitmapDocument.cpp
# End Source File
# Begin Source File

SOURCE=.\ExAPBitmapGrey8.cpp
# End Source File
# Begin Source File

SOURCE=.\ExAPBitmapJPEGLibClient.cpp
# End Source File
# Begin Source File

SOURCE=.\ExAPBitmapNon8BitPaletted.cpp
# End Source File
# Begin Source File

SOURCE=.\ExAPBitmapPalette.cpp
# End Source File
# Begin Source File

SOURCE=.\ExAPBitmapPaletted.cpp
# End Source File
# Begin Source File

SOURCE=.\ExAPBitmapRGB24.cpp
# End Source File
# Begin Source File

SOURCE=.\ExAPBitmapRGBA32.cpp
# End Source File
# Begin Source File

SOURCE=.\ExAPBitmapRGBA64.cpp
# End Source File
# Begin Source File

SOURCE=.\ExAPBitmapWin.cpp
# End Source File
# Begin Source File

SOURCE=.\ExAPBitmapXXX24.cpp
# End Source File
# Begin Source File

SOURCE=.\ExAPBitmapXXXA32.cpp
# End Source File
# Begin Source File

SOURCE=.\ExAPColourModel.cpp
# End Source File
# Begin Source File

SOURCE=.\ExAPExport.cpp
# End Source File
# Begin Source File

SOURCE=.\ExAPExportGIF.cpp
# End Source File
# Begin Source File

SOURCE=.\ExAPExportTIFF.cpp
# End Source File
# Begin Source File

SOURCE=.\ExAPImportBMP.cpp
# End Source File
# Begin Source File

SOURCE=.\ExAPImportICO.cpp
# End Source File
# Begin Source File

SOURCE=.\ExAPImportJPEG.cpp
# End Source File
# Begin Source File

SOURCE=.\ExAPImportPNG.cpp
# End Source File
# Begin Source File

SOURCE=.\ExAPImportTIFF.cpp
# End Source File
# Begin Source File

SOURCE=.\ExAPMedianCut.cpp
# End Source File
# Begin Source File

SOURCE=.\ExAPReadGIF87a.cpp
# End Source File
# Begin Source File

SOURCE=.\ExAPReadTIFFTags.cpp
# End Source File
# Begin Source File

SOURCE=.\ExAPWinDLLMain.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Include\ExAPBitmap.h
# End Source File
# Begin Source File

SOURCE=.\Include\ExAPBitmap16bpch.h
# End Source File
# Begin Source File

SOURCE=.\Include\ExAPBitmap1BitBlackWhite.h
# End Source File
# Begin Source File

SOURCE=.\Include\ExAPBitmap8BitPaletted.h
# End Source File
# Begin Source File

SOURCE=.\Include\ExAPBitmap8bpch.h
# End Source File
# Begin Source File

SOURCE=.\Include\ExAPBitmap8bpchNoAlpha.h
# End Source File
# Begin Source File

SOURCE=.\Include\ExAPBitmap8bpchWithAlpha.h
# End Source File
# Begin Source File

SOURCE=.\Include\ExAPBitmapCMYK32.h
# End Source File
# Begin Source File

SOURCE=.\Include\ExAPBitmapDither.h
# End Source File
# Begin Source File

SOURCE=.\Include\ExAPBitmapDitherBayer.h
# End Source File
# Begin Source File

SOURCE=.\Include\ExAPBitmapDitherErrorDiffusion.h
# End Source File
# Begin Source File

SOURCE=.\Include\ExAPBitmapDitherNone.h
# End Source File
# Begin Source File

SOURCE=.\Include\ExAPBitmapDocument.h
# End Source File
# Begin Source File

SOURCE=.\Include\ExAPBitmapGrey8.h
# End Source File
# Begin Source File

SOURCE=.\Include\ExAPBitmapNon8BitPaletted.h
# End Source File
# Begin Source File

SOURCE=.\Include\ExAPBitmapPalette.h
# End Source File
# Begin Source File

SOURCE=.\Include\ExAPBitmapPaletted.h
# End Source File
# Begin Source File

SOURCE=.\Include\ExAPBitmapRGB24.h
# End Source File
# Begin Source File

SOURCE=.\Include\ExAPBitmapRGBA32.h
# End Source File
# Begin Source File

SOURCE=.\Include\ExAPBitmapRGBA64.h
# End Source File
# Begin Source File

SOURCE=.\Include\ExAPBitmapXXX24.h
# End Source File
# Begin Source File

SOURCE=.\Include\ExAPBitmapXXXA32.h
# End Source File
# Begin Source File

SOURCE=.\Include\ExAPColourModel.h
# End Source File
# Begin Source File

SOURCE=.\Include\ExAPConversionTemplates.h
# End Source File
# Begin Source File

SOURCE=.\Include\ExAPImportTIFF.h
# End Source File
# Begin Source File

SOURCE=.\Include\ExAPMedianCut.h
# End Source File
# Begin Source File

SOURCE=.\Include\ExAPReadTIFFTags.h
# End Source File
# Begin Source File

SOURCE=.\Include\ExAPRect.h
# End Source File
# Begin Source File

SOURCE=.\Include\ExAPTIFFTags.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\AssetProcessor.rc
# End Source File
# End Group
# End Target
# End Project
