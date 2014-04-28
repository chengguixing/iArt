//==========================================
// Matt Pietrek
// Microsoft Systems Journal, March 2000
// FILE: DelayLoadProfile.CPP
//==========================================
#include "stdafx.h"
#include <stdio.h>
#include <shlwapi.h>
#include <tchar.h>

#include "DebugInjector.H"

bool ProcessCommandLine( PCTSTR pszCmdLine, PTSTR pszTargetCmdLine );
bool GetInjectedDLLFullPath( PTSTR pszFullPath );
void __stdcall OutputDebugStringCallback( PTSTR psz );

char g_szHelp[] =
"Syntax: DelayLoadProfile <TargetCmdLine> [target args]\n\n"
"Matt Pietrek, Microsoft Systems Journal, March 2000\n\n"
"DelayLoadProfile starts a target process.  As the process exits, it\n"
"generates a report indicating which DLLs were imported by the EXE,\n"
"and how many times the DLL was called.\n\n"
"Please see the accompanying article for a full explanation of "
"DelayLoadProfile.\n";

//===========================================================================
int main( int argc, TCHAR * argv[] )
{
    //=====================================================
    // Get target cmd line
    TCHAR szTargetCmdLine[MAX_PATH*2] = { 0 };
    if ( !ProcessCommandLine( GetCommandLine(), szTargetCmdLine ) )
    {
        printf( g_szHelp );
        return 1;
    }

    //=====================================================
    // Create target in preparation for injection
    CDebugInjector injector;

    if ( !injector.LoadProcess( szTargetCmdLine ) )
    {
        printf( "Unable to start %s\n", szTargetCmdLine );
        return 1;
    }

    //=====================================================
    // Tell the injector which DLL to inject
    TCHAR szDllToInject[MAX_PATH];
    GetInjectedDLLFullPath( szDllToInject );

    injector.SetDLLToInject( szDllToInject );

    //=====================================================
    // Indicate where the OutputDebugString strings will go
    injector.SetOutputDebugStringCallback( OutputDebugStringCallback );

    //=====================================================
    // Let the target run.  This cause the injection
    injector.Run();

    return 0;
}

//===========================================================================
bool ProcessCommandLine(PCTSTR pszCmdLine,       // Cmd line passed to us
                        PTSTR pszTargetCmdLine   // What we'll start the target
                        )                       // process with
{
    // cmd line syntax is: ThisExeName [OurArgs] TargetExe [TargetArgs]

    PTSTR p = PathGetArgs( pszCmdLine );
    if ( p && *p )
    {
        _tcscpy( pszTargetCmdLine, p );
        return true;
    }

    return false;
}

//===========================================================================
bool GetInjectedDLLFullPath( PTSTR pszFullPath )
{
    // The DLL should be in the same directory as this EXE.  Get the EXE's
    // full path and replace the EXE name with the DLL name.
    TCHAR szExePath[MAX_PATH];

    GetModuleFileName( 0, szExePath, sizeof(szExePath) );
    PathRemoveFileSpec( szExePath );

    _tcscpy( pszFullPath, szExePath );
    _tcscat( pszFullPath, _T("\\DelayLoadProfileDll.DLL" ));

    return true;
}

void __stdcall OutputDebugStringCallback( PTSTR psz )
{
    // Print everything to the console.  If other OutputDebugString calls
    // in the debuggee annoy you, can can apply filtering here.
    printf( "%s\n", psz );
}
