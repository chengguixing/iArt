//==========================================
// Matt Pietrek
// Microsoft Systems Journal, March 2000
// FILE: DelayLoadProfile.CPP
//==========================================
#include "stdafx.h"
#include <stdio.h>
#include <malloc.h>
#include <stddef.h>
#include <tchar.h>
#include "DebugInjector.h"

static PSTR s_arszDebugEventTypes[] = 
{
    "",
    "EXCEPTION",
    "CREATE_THREAD",
    "CREATE_PROCESS",
    "EXIT_THREAD",
    "EXIT_PROCESS",
    "LOAD_DLL",
    "UNLOAD_DLL",
    "OUTPUT_DEBUG_STRING",
    "RIP",
};  

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDebugInjector::CDebugInjector() :
    m_bInjected( false ),
    m_pszDLLToInject( 0 ),
    m_pExeEntryPoint( 0 ),
    m_pStubInTarget( 0 ),
    m_pStubInTargetBP( 0 ),
    m_originalExeEntryPointOpcode( 0 )
{
    memset( &m_CreateProcessDebugInfo,  0, sizeof(m_CreateProcessDebugInfo) );
    memset( &m_originalThreadContext, 0, sizeof(m_originalThreadContext) );
    memset( &m_ProcessInformation, 0, sizeof(m_ProcessInformation) );
}

//===================================================================

CDebugInjector::~CDebugInjector()
{
    CloseHandle( m_ProcessInformation.hThread );
    CloseHandle( m_ProcessInformation.hProcess );
    
    CloseHandle( m_CreateProcessDebugInfo.hProcess );
    CloseHandle( m_CreateProcessDebugInfo.hThread );

    delete []m_pszDLLToInject;
}

//===================================================================

bool CDebugInjector::LoadProcess( PTSTR pszCmdLine )
{
    STARTUPINFO startupInfo;
    
    memset(&startupInfo, 0, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);

    BOOL bCreateProcessRetValue;

    bCreateProcessRetValue = 
        CreateProcess(
                    0,                          // lpszImageName
                    pszCmdLine,                 // lpszCommandLine
                    0,                          // lpsaProcess
                    0,                          // lpsaThread
                    FALSE,                      // fInheritHandles
                    DEBUG_ONLY_THIS_PROCESS,    // fdwCreate
                    0,                          // lpvEnvironment
                    0,                          // lpszCurDir
                    &startupInfo,               // lpsiStartupInfo
                    &m_ProcessInformation );    // lppiProcInfo

    return bCreateProcessRetValue != FALSE;
}

//===================================================================

bool CDebugInjector::SetDLLToInject(PTSTR pszDLL)
{
    m_pszDLLToInject = new TCHAR[ lstrlen(pszDLL) + 1 ];
    lstrcpy( m_pszDLLToInject, pszDLL );
    
    return true;
}

//===================================================================

bool CDebugInjector::Run( void )
{
    DEBUG_EVENT dbgEvent;
    DWORD dwContinueStatus;
    
    // The debug loop.  Runs until the debuggee terminats
    while ( 1 )
    {
        WaitForDebugEvent(&dbgEvent, INFINITE);
                  
        dwContinueStatus = HandleDebugEvent( dbgEvent );
        
        if ( dbgEvent.dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT )
            break;
       
        ContinueDebugEvent( dbgEvent.dwProcessId,
                            dbgEvent.dwThreadId,
                            dwContinueStatus );
    }

    return true;
}

//===================================================================

DWORD CDebugInjector::HandleDebugEvent(DEBUG_EVENT &dbgEvent)
{
    DWORD dwContinueStatus = DBG_CONTINUE;

    switch ( dbgEvent.dwDebugEventCode )
    {
        case CREATE_PROCESS_DEBUG_EVENT:
            m_CreateProcessDebugInfo = dbgEvent.u.CreateProcessInfo;
            CloseHandle( m_CreateProcessDebugInfo.hFile ); 
            break;

        case EXCEPTION_DEBUG_EVENT:
            dwContinueStatus = HandleException( dbgEvent );
            break;

        case CREATE_THREAD_DEBUG_EVENT:
            CloseHandle( dbgEvent.u.CreateThread.hThread );
            break;

        case LOAD_DLL_DEBUG_EVENT:
            CloseHandle( dbgEvent.u.LoadDll.hFile );
            break;

        case OUTPUT_DEBUG_STRING_EVENT:
            HandleOutputDebugString( dbgEvent );
            break;
    }
    
    return dwContinueStatus;
}

//===================================================================

DWORD CDebugInjector::HandleException(DEBUG_EVENT &dbgEvent )
{
    EXCEPTION_RECORD  & exceptRec = dbgEvent.u.Exception.ExceptionRecord;

    // If this is a second chance exception, the debuggee is going to
    // die.  Spit out the exception code and address
    if ( dbgEvent.u.Exception.dwFirstChance == FALSE )
    {
        printf( "Exception code: %X  Addr: %08X\r\n",
                exceptRec.ExceptionCode, exceptRec.ExceptionAddress );
    }

    // If we've gone through the mechanics of injection already, just
    // pass the exception on to the debuggee
    if ( m_bInjected )
        return DBG_EXCEPTION_NOT_HANDLED;
    
    // If it isn't a breakpoint, we don't want to know about it.
    if ( exceptRec.ExceptionCode != EXCEPTION_BREAKPOINT )
        return DBG_EXCEPTION_NOT_HANDLED;

    static bool s_bFirstBP = FALSE;
    DWORD dwContinueStatus = DBG_CONTINUE;
    
    // Is this the DebugBreak breakpoint?
    if ( s_bFirstBP == false )
    {
        SetEntryPointBP();
        s_bFirstBP = true;
    }
    // Is this the breakpoint we set at the EXE's entry point?
    else if ( exceptRec.ExceptionAddress == m_pExeEntryPoint )
    {
        RemoveEntryPointBP();       
        SaveEntryPointContext( dbgEvent );
        PlaceInjectionStub();
    }
    // Is this the BP immediately after our LoadLibrary call?
    else if ( exceptRec.ExceptionAddress == m_pStubInTargetBP )
    {
        RestoreEntryPointContext();     
        m_bInjected = true;
    }
        
    return dwContinueStatus;
}

//===================================================================

void CDebugInjector::HandleOutputDebugString( DEBUG_EVENT & dbgEvent )
{
    if ( !m_pfnODSCallback )    // If no callback registered, just bail
        return;

    // An OutputDebugString debug event contains the address and length
    // of a string in the debuggee.  Prepare to copy that memory into
    // our process space.
    WORD nLength = dbgEvent.u.DebugString.nDebugStringLength;
    PVOID pStr = dbgEvent.u.DebugString.lpDebugStringData;

    TCHAR szBuffer[ 1024 ];
    nLength = min( sizeof(szBuffer), nLength );

    bool retValue = ReadTargetMemory( pStr, szBuffer, nLength );

    szBuffer[nLength] = 0;  // Null terminate this for Win9X ;-)

    // If we were able to read the string, invoke the registered callback
    if ( retValue )
        m_pfnODSCallback( szBuffer );
}

//===================================================================

bool CDebugInjector::SaveEntryPointContext( DEBUG_EVENT & dbgEvent )
{
    // Make sure that the thread we have the handle for is
    // the same thread that hit the BP
    if ( m_ProcessInformation.dwThreadId != dbgEvent.dwThreadId )
        DebugBreak();
    
    m_originalThreadContext.ContextFlags = CONTEXT_FULL;
    
    if ( !GetThreadContext( m_CreateProcessDebugInfo.hThread,
                            &m_originalThreadContext) )
        return false;

    // The EIP in the context structure points past the BP, so
    // decrement EIP to point at the original instruction
    m_originalThreadContext.Eip = m_originalThreadContext.Eip -1;

    return true;
}

//===================================================================

bool CDebugInjector::RestoreEntryPointContext( void )
{
    // Set the register back to what they were before we redirected them to
    // the LoadLibrary stub. If we were really fastidious, we'd also delete
    // the memory allocated for the stub here.

    return SetThreadContext(    m_CreateProcessDebugInfo.hThread,
                                &m_originalThreadContext) != FALSE;
}

//===================================================================

bool CDebugInjector::SetEntryPointBP( void )
{
    m_pExeEntryPoint = m_CreateProcessDebugInfo.lpStartAddress;

    bool retValue = ReadTargetMemory( m_pExeEntryPoint,
                                      &m_originalExeEntryPointOpcode,
                                      sizeof(m_originalExeEntryPointOpcode));

    if ( !retValue )
        return false;

    BYTE bpOpcode = 0xCC;

    retValue = WriteTargetMemory(   m_pExeEntryPoint,
                                    &bpOpcode,
                                    sizeof(bpOpcode) );
    return retValue ? true : false;
}

//===================================================================

bool CDebugInjector::RemoveEntryPointBP()
{
    bool retValue = WriteTargetMemory(m_pExeEntryPoint,
                                      &m_originalExeEntryPointOpcode,
                                      sizeof(m_originalExeEntryPointOpcode));
    if ( !retValue )
        return false;

    return true;
}

//===================================================================

bool CDebugInjector::PlaceInjectionStub( void )
{
    //=====================================================
    // Locate where the stub will be in the target process
    m_pStubInTarget = (LOADLIBRARY_STUB*)GetMemoryForLoadLibraryStub();
    if ( !m_pStubInTarget )
        return false;

    m_pStubInTargetBP = (PBYTE)m_pStubInTarget +
                        offsetof(LOADLIBRARY_STUB, instr_INT_3);
    
    //=====================================================
    // Complete the stub fields that can't be preinitialized
    _tcscpy( m_stub.data_DllName, m_pszDLLToInject );

    m_stub.operand_PUSH_value = (DWORD)m_pStubInTarget
                                + offsetof( LOADLIBRARY_STUB, data_DllName);

    m_stub.operand_MOV_EAX =
        (DWORD)GetProcAddress(GetModuleHandle(_T("KERNEL32.DLL")),"LoadLibraryW");

    //=====================================================
    // Copy the stub into the target process
    bool retValue;
    retValue = WriteTargetMemory(m_pStubInTarget, &m_stub,sizeof(m_stub));
    if ( !retValue )
        return false;

    //=====================================================
    // Change the EIP register in the target thread to point
    // at the stub we just copied in.
    CONTEXT stubContext = m_originalThreadContext;
    stubContext.Eip = (DWORD)m_pStubInTarget;

    SetThreadContext( m_CreateProcessDebugInfo.hThread, &stubContext );

    return true;
}

//===================================================================

bool CDebugInjector::ReadTargetMemory(  PVOID pAddr,
                                        PVOID pBuffer,
                                        unsigned cb )
{
    DWORD cbRead;

    BOOL retVal = ReadProcessMemory(    m_CreateProcessDebugInfo.hProcess,
                                        pAddr, pBuffer, cb, &cbRead );

    return ( retVal && (cbRead == cb) );
}

//===================================================================

bool CDebugInjector::WriteTargetMemory( PVOID pAddr,
                                        PVOID pBuffer,
                                        unsigned cb )
{
    DWORD cbWrite;

    BOOL retVal = WriteProcessMemory(   m_CreateProcessDebugInfo.hProcess,
                                        pAddr, pBuffer, cb, &cbWrite );

    return ( retVal && (cbWrite == cb) );
}

//===================================================================

bool CDebugInjector::SetOutputDebugStringCallback( PFNODSCALLBACK pfn )
{
    m_pfnODSCallback = pfn;
    return true;
}

//===================================================================

typedef
LPVOID (__stdcall * PFNVIRTALLEX)(HANDLE, LPVOID, SIZE_T, DWORD,DWORD);

PVOID CDebugInjector::GetMemoryForLoadLibraryStub(void)
{
    OSVERSIONINFO osvi = { sizeof(osvi) };

    GetVersionEx( &osvi );

    if ( osvi.dwPlatformId == VER_PLATFORM_WIN32_NT )
    {
        // We're on NT, so use VirtualAllocEx to allocate memory in the other
        // process address space.  Alas, we can't just call VirtualAllocEx
        // since it's not defined in the Windows 95 KERNEL32.DLL.

        PFNVIRTALLEX pfnVirtualAllocEx = (PFNVIRTALLEX)
            GetProcAddress(GetModuleHandle(_T("KERNEL32.DLL")),"VirtualAllocEx");

        LPVOID pStubMemory = pfnVirtualAllocEx(
                                m_CreateProcessDebugInfo.hProcess,
                                0, sizeof(LOADLIBRARY_STUB),
                                MEM_COMMIT, PAGE_READWRITE );
        
        return (LOADLIBRARY_STUB *)pStubMemory;
    }
    else if ( osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS )
    {
        // In Windows 9X, create a small memory mapped file.  On this
        // platform, memory mapped files are above 2GB, and thus are
        // accessible by all processes.

        HANDLE hFileMapping = CreateFileMapping(
                                    INVALID_HANDLE_VALUE, 0,
                                    PAGE_READWRITE | SEC_COMMIT,
                                    0,
                                    sizeof(LOADLIBRARY_STUB),
                                    0 );
        if ( hFileMapping )
        {
            LPVOID pStubMemory = MapViewOfFile( hFileMapping,
                                                FILE_MAP_WRITE,
                                                0, 0,
                                                sizeof(LOADLIBRARY_STUB) );
            return (LOADLIBRARY_STUB *)pStubMemory;
        }
        else
            CloseHandle( hFileMapping );
    }

    return 0;
}
