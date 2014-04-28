//==========================================
// Matt Pietrek
// Microsoft Systems Journal, March 2000
// FILE: DelayLoadProfileDLL.H
//==========================================
bool PrepareToProfile( void );
bool ReportProfileResults( void );
bool RedirectIAT( PIMAGE_IMPORT_DESCRIPTOR pImportDesc, PVOID pBaseLoadAddr );
bool IsModuleOKToHook( PTSTR pszModule );

PIMAGE_NT_HEADERS PEHeaderFromHModule(HMODULE hModule);

// This function must be __cdecl!!!
void DelayLoadProfileDLL_UpdateCount( PVOID dummy );

// MakePtr is a macro that allows you to easily add to values (including
// pointers) together without dealing with C's pointer arithmetic.  It
// essentially treats the last two parameters as DWORDs.  The first
// parameter is used to typecast the result to the appropriate pointer type.
#define MakePtr(cast, ptr, addValue ) (cast)( (DWORD)(ptr)+(DWORD)(addValue))

#pragma pack( push, 1 )
struct DLPD_IAT_STUB
{
    BYTE    instr_CALL;
    DWORD   data_call;
    BYTE    instr_JMP;
    DWORD   data_JMP;
    DWORD   count;
    DWORD   pszNameOrOrdinal;

    DLPD_IAT_STUB() : instr_CALL( 0xE8 ), instr_JMP( 0xE9 ), count( 0 ) {}
};
#pragma pack( pop )

PIMAGE_IMPORT_DESCRIPTOR    g_pFirstImportDesc;
