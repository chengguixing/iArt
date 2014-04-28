//==========================================
// Matt Pietrek
// Microsoft Systems Journal, March 2000
// FILE: DelayLoadProfile.CPP
//==========================================
#if !defined(AFX_DEBUGINJECTOR_H__F6766652_BB61_4FE0_835C_56CD23FC63EE__INCLUDED_)
#define AFX_DEBUGINJECTOR_H__F6766652_BB61_4FE0_835C_56CD23FC63EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef void (__stdcall *PFNODSCALLBACK)(PTSTR pszString);

class CDebugInjector  
{
public:
    CDebugInjector();
    virtual ~CDebugInjector();

    bool    LoadProcess( PTSTR pszCmdLine );
    bool    SetDLLToInject( PTSTR pszDLL );
    bool    Run( void );
    bool    SetOutputDebugStringCallback( PFNODSCALLBACK pfn );

    bool    m_bInjected;    // Set to true after LoadLibrary has been called
                            // in the debuggee. LoadLibrary may have failed!

protected:
    DWORD   HandleDebugEvent( DEBUG_EVENT & dbgEvent );
    DWORD   HandleException( DEBUG_EVENT & dbgEvent );
    void    HandleOutputDebugString( DEBUG_EVENT & dbgEvent );

    bool    SetEntryPointBP( void );
    bool    RemoveEntryPointBP( void );

    bool    SaveEntryPointContext( DEBUG_EVENT & dbgEvent );
    bool    RestoreEntryPointContext( void );

    bool    PlaceInjectionStub( void );
    PVOID   GetMemoryForLoadLibraryStub( void );

    bool    ReadTargetMemory( PVOID pAddr, PVOID pBuffer, unsigned cb );
    bool    WriteTargetMemory( PVOID pAddr, PVOID pBuffer, unsigned cb );

    PROCESS_INFORMATION         m_ProcessInformation;
    CREATE_PROCESS_DEBUG_INFO   m_CreateProcessDebugInfo;
    PTSTR                        m_pszDLLToInject;
    PFNODSCALLBACK              m_pfnODSCallback;

    //=======================================================================
    // Stuff for the BP we'll set at the EXE's entry point
    PVOID   m_pExeEntryPoint;
    BYTE    m_originalExeEntryPointOpcode;
    CONTEXT m_originalThreadContext;

    //=======================================================================
    // Here lies the stub we use to call LoadLibrary in the target process
    #pragma pack( push, 1 )
    struct LOADLIBRARY_STUB
    {
        BYTE    instr_PUSH;
        DWORD   operand_PUSH_value;
        BYTE    instr_MOV_EAX;
        DWORD   operand_MOV_EAX;
        WORD    instr_CALL_EAX;
        BYTE    instr_INT_3;
        TCHAR    data_DllName[MAX_PATH];

        LOADLIBRARY_STUB() :    // Constructor builds known in advance fields
            instr_PUSH( 0x68 ), instr_MOV_EAX( 0xB8 ),
            instr_CALL_EAX( 0xD0FF ), instr_INT_3( 0xCC ){ }
    };
    #pragma pack( pop )

    LOADLIBRARY_STUB    m_stub;
    LOADLIBRARY_STUB *  m_pStubInTarget;
    PVOID               m_pStubInTargetBP;
};

#endif // !defined(AFX_DEBUGINJECTOR_H__F6766652_BB61_4FE0_835C_56CD23FC63EE__INCLUDED_)
