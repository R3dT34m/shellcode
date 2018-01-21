/**
  Copyright © 2017 Odzhan. All Rights Reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are
  met:

  1. Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

  3. The name of the author may not be used to endorse or promote products
  derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY AUTHORS "AS IS" AND ANY EXPRESS OR
  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE. */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include <windows.h>

//#pragma comment (lib, "crypt32.lib")

PBYTE     img=NULL;
ULONGLONG img_base;
LPBYTE    lpAddress;
PCHAR     file;
DWORD     found, api_cnt;
DWORD     low, high;
    
// return pointer to DOS header
PIMAGE_DOS_HEADER DosHdr (void)
{
    return (PIMAGE_DOS_HEADER)lpAddress;
}

// return pointer to NT header
PIMAGE_NT_HEADERS NtHdr (void)
{
  DWORD v = DosHdr()->e_lfanew;
  if (v > 512) {
    //printf("%s is wrong\n", file);
    return NULL;
  }
  return (PIMAGE_NT_HEADERS) (lpAddress + DosHdr()->e_lfanew);
}

// return pointer to File header
PIMAGE_FILE_HEADER FileHdr (void)
{
  PIMAGE_NT_HEADERS nt = NtHdr();
  if (nt == NULL) return NULL;
  
  return &NtHdr()->FileHeader;
}

// determines CPU architecture of binary
BOOL is32 (void)
{
    PIMAGE_FILE_HEADER hdr = FileHdr();
    if (hdr == NULL) return FALSE;
    
    return FileHdr()->Machine==IMAGE_FILE_MACHINE_I386;
}

// determines CPU architecture of binary
BOOL is64 (void)
{
    PIMAGE_FILE_HEADER hdr = FileHdr();
    if (hdr == NULL) return FALSE;
    
    return FileHdr()->Machine==IMAGE_FILE_MACHINE_AMD64;
}

// return pointer to Optional header
LPVOID OptHdr (void)
{
    return (LPVOID)&NtHdr()->OptionalHeader;
}

// return pointer to first section header
PIMAGE_SECTION_HEADER SecHdr (void)
{
    PIMAGE_NT_HEADERS nt=NtHdr();
    if (nt == NULL) return NULL;
    
    return (PIMAGE_SECTION_HEADER)((LPBYTE)&nt->OptionalHeader + 
    nt->FileHeader.SizeOfOptionalHeader);
}

DWORD DirSize (void)
{
    if (is32()) {
      return ((PIMAGE_OPTIONAL_HEADER32)OptHdr())->NumberOfRvaAndSizes;
    } else if (is64()) {
      return ((PIMAGE_OPTIONAL_HEADER64)OptHdr())->NumberOfRvaAndSizes;
    }
    return 0;
}

DWORD SecSize (void)
{
    return NtHdr()->FileHeader.NumberOfSections;
}

PIMAGE_DATA_DIRECTORY Dirs (void)
{
    if (DirSize == 0) return NULL;
    
    if (is32()) {
      return ((PIMAGE_OPTIONAL_HEADER32)OptHdr())->DataDirectory;
    } else if (is64()) {
      return ((PIMAGE_OPTIONAL_HEADER64)OptHdr())->DataDirectory;
    }
    return NULL;
}

ULONGLONG ImgBase (void)
{
    if (is32()) {
      return ((PIMAGE_OPTIONAL_HEADER32)OptHdr())->ImageBase;
    } else if (is64()) {
      return ((PIMAGE_OPTIONAL_HEADER64)OptHdr())->ImageBase;
    }
    return 0;
}

// valid dos header?
int valid_dos_hdr (void)
{
    PIMAGE_DOS_HEADER dos=DosHdr();
    if (dos->e_magic!=IMAGE_DOS_SIGNATURE) return 0;
    return (dos->e_lfanew != 0);
}

// valid nt headers
int valid_nt_hdr (void)
{
  PIMAGE_NT_HEADERS nt = NtHdr();
  if (nt == NULL) return 0;
  return NtHdr()->Signature==IMAGE_NT_SIGNATURE;
}

int isObj (void) {
    PIMAGE_DOS_HEADER dos=DosHdr();
    
    return ((dos->e_magic==IMAGE_FILE_MACHINE_AMD64 ||
    dos->e_magic==IMAGE_FILE_MACHINE_I386) && 
    dos->e_sp==0);
}

DWORD rva2ofs(DWORD rva)
{
    PIMAGE_SECTION_HEADER	sec;
    int				i;

    if (rva == 0) return -1;

    sec = SecHdr();
    
    if (sec==0) return -1;
    
    for (i = SecSize() - 1; i >= 0; i--)
    {
        if (sec[i].VirtualAddress <= rva &&
            rva <= (DWORD)sec[i].VirtualAddress + sec[i].SizeOfRawData)
        {
            return sec[i].PointerToRawData + rva - sec[i].VirtualAddress;
        }
    }
    return -1;
}

char *api_exp[]=
{
  "DmpRemoteDumpRequest",
  NULL
};

char *api_imp[]=
{
  "LogonUserA",
  "LogonUserW",
  "ImpersonateLoggedOnUser",
  "CreateRemoteThread",
  "CreateRemoteThreadEx",
  "CreateProcessAsUserA",
  "CreateProcessAsUserW",
  "CreateProcessWithTokenA",
  "CreateProcessWithTokenW",
  "CreateProcessWithLogonA",
  "CreateProcessWithLogonW",
  "NtCreateThreadEx",
  "QueueUserAPC",
  "OpenProcess",
  "ReadProcessMemory",
  "WriteProcessMemory",
  "NtReadVirtualMemory",
  "NtWriteVirtualMemory",
  "NtWaitForSingleObject",
  "NtGetContextThread",
  "GetContextThread",
  "NtSetContextThread",
  "SetContextThread",
  "NtUnmapViewOfSection",
  "SetWindowsHookExA",
  "SetWindowsHookExW",
  "RtlCreateUserThread",
  "VirtualAllocEx",
  "OpenThread",
  "CreateProcessA",
  "CreateProcessW",
  "WinExec",
  "ShellExecuteExW",
  "ShellExecuteExA",
  "NtOpenProcess",
  
  "InternetConnect",
  "InternetOpenA",
  "InternetOpenW",
  "InternetWriteFile",
  "InternetWriteFileEx",
  "InternetReadFile",
  "InternetReadFileExA",
  "InternetReadFileExW",
  
  "InternetOpenUrlW",
  "InternetOpenUrlA",
  "InternetSetStatusCallback",
  "InternetSetStatusCallbackA",
  "InternetSetStatusCallbackW",
  
  "HttpOpenRequestA",
  "HttpOpenRequestW",
  "HttpSendRequestExA",
  "HttpSendRequestExW",
  
  "InternetCloseHandle",
  "URLDownloadToFileA",
  "URLDownloadToFileW",
  "HttpOpenRequest",
  "WinHttpOpen",
  "WinHttpConnect",
  "WinHttpOpenRequest",
  "WinHttpCloseHandle",
  "WinHttpSendRequest",
  "WinHttpReceiveResponse",
  "WinHttpReadData",
  "WinHttpWriteData",
  "HttpQueryInfoA",
  "HttpQueryInfoW",
  
  "FtpOpenFileA",
  "FtpOpenFileW",
  "FtpGetFileA",
  "FtpGetFileW",
  "FtpFindFirstFileA",
  "FtpFindFirstFileW",
  "InternetFindNextFileA",
  "InternetFindNextFileW",
  //"LoadLibraryA",
  //"LoadLibraryW",
  //"GetProcAddress",
  NULL
};

void check_api(PCHAR name) {
  int i;
  
  if (name == NULL) return;
  
  for (i=0; api_imp[i] != NULL; i++) {
    if (lstrcmp(api_imp[i], name)==0) {
      found++;
      printf ("Found %-30s in %s\n", api_imp[i], file);
    }
  }
}

void exp_dir (PIMAGE_DATA_DIRECTORY dir)
{
    DWORD                   cnt=0, ofs=0, idx;
    PIMAGE_EXPORT_DIRECTORY exp;
    PDWORD                  adr;
    PDWORD                  sym;
    PWORD                   ord;
    
    if (dir[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress!=0) 
    {
      ofs = rva2ofs(dir[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
      if (ofs != -1) 
      {
        exp=(IMAGE_EXPORT_DIRECTORY*) (ofs + lpAddress);
        
        if ((LPBYTE)exp >= (lpAddress + low)) return;
        
        cnt=exp->NumberOfNames;
        if (cnt == 0) return;
        
        ofs = rva2ofs(exp->AddressOfNames);
        
        if (ofs == -1) return;
        
        sym = (PDWORD) (ofs + lpAddress);
        
        if ((LPBYTE)sym >= (lpAddress + low)) return;
        
        //adr=(DWORD*) (rva2ofs(exp->AddressOfFunctions)    + lpAddress);
        //sym=(DWORD*) (rva2ofs(exp->AddressOfNames)        + lpAddress);
        //ord=(WORD*)  (rva2ofs(exp->AddressOfNameOrdinals) + lpAddress);
        //ofs=(DWORD)  (rva2ofs(exp->Name)                  + lpAddress);

        //if (ofs != -1) {
          for (idx=0; idx<cnt; idx++) {
            
            ofs=rva2ofs ((DWORD)sym[idx]);
            if (ofs != -1) {
              if ((LPBYTE)(ofs + lpAddress) > (lpAddress + low)) break;
              check_api((PCHAR)(ofs + lpAddress));
              
              /*printf ("\n  %04X %08X %s (%016llX)", 
              ord[idx], adr[ord[idx]], 
              ofs + lpAddress, adr[ord[idx]] + img_base);*/
            }
          }
        //}
      }
    }
}
       
typedef DWORD RVA;

typedef struct ImgDelayDescr {  
    DWORD        grAttrs;        // attributes  
    RVA          rvaDLLName;     // RVA to dll name  
    RVA          rvaHmod;        // RVA of module handle  
    RVA          rvaIAT;         // RVA of the IAT  
    RVA          rvaINT;         // RVA of the INT  
    RVA          rvaBoundIAT;    // RVA of the optional bound IAT  
    RVA          rvaUnloadIAT;   // RVA of optional copy of original IAT  
    DWORD        dwTimeStamp;    // 0 if not bound,  
} ImgDelayDescr, * PImgDelayDescr;  

void dump_imports32(PIMAGE_THUNK_DATA32 itd, DWORD ofs)
{
    PIMAGE_IMPORT_BY_NAME ibn;
    
    for (;;itd++) {
      // no more entries? bail out
      if (itd->u1.AddressOfData == 0) {
        break;
      }
      // skip ordinals
      if (IMAGE_SNAP_BY_ORDINAL32(itd->u1.Ordinal)) {
        continue;
      }
      ibn = (PIMAGE_IMPORT_BY_NAME)(rva2ofs(itd->u1.AddressOfData - ofs) + lpAddress);
      
      check_api(ibn->Name);           
    }
}

void dump_imports64(PIMAGE_THUNK_DATA64 itd)
{
    PIMAGE_IMPORT_BY_NAME ibn;
    
    for (;;itd++) {
      // no more entries? bail out
      if (itd->u1.AddressOfData == 0) {
        break;
      }
      // skip ordinals
      if (IMAGE_SNAP_BY_ORDINAL32(itd->u1.Ordinal)) {
        continue;
      }
      ibn = (PIMAGE_IMPORT_BY_NAME)(rva2ofs(itd->u1.AddressOfData) + lpAddress);
      
      check_api(ibn->Name);           
    }  
}

void imp_dir (PIMAGE_DATA_DIRECTORY dir)
{
    PIMAGE_IMPORT_DESCRIPTOR imp;
    DWORD                    ofs, va, vs;
    DWORD                    x32;
  
    va = dir[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    vs = dir[IMAGE_DIRECTORY_ENTRY_IMPORT].Size;
    
    if (va != 0 && vs != 0) 
    {      
      ofs = rva2ofs(va);
      
      if (ofs != -1)
      {
        imp = (PIMAGE_IMPORT_DESCRIPTOR)(ofs + lpAddress);
        x32 = is32();
        
        for (;;)
        {
          // if we're past address + size, the file is likely compressed or corrupt
          if ((LPBYTE)imp > (lpAddress + low)) break;
          
          // last entry? bail out
          if (imp->Name       == 0 ||
              imp->FirstThunk == 0) {        
            break;
          }

          if (imp->OriginalFirstThunk != 0) {
            ofs = rva2ofs(imp->OriginalFirstThunk);
          } else {
            ofs = rva2ofs(imp->FirstThunk);
          }
          
          if (ofs != -1)
          {
            if (x32) {
              dump_imports32((PIMAGE_THUNK_DATA32)(ofs + lpAddress), 0);
            } else {
              dump_imports64((PIMAGE_THUNK_DATA64)(ofs + lpAddress));
            }
          }
          // advance to next entry
          imp++;
        }
      }
    }
}

void delay_dir (IMAGE_DATA_DIRECTORY *dir)
{
    PImgDelayDescr           idd;
    DWORD                    ofs, va, vs, x;
    PIMAGE_THUNK_DATA32      itd;
    DWORD                    x32;
  
    va = dir[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT].VirtualAddress;
    vs = dir[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT].Size;
    
    if (va != 0 && vs != 0) 
    {
      ofs = rva2ofs(va);
      
      if (ofs != -1)
      {
        idd = (PImgDelayDescr)(ofs + lpAddress);
        x32 = is32();
       
        for (;;)
        {
          // if we're past address + size, the file is likely compressed or corrupt         
          if ((LPBYTE)idd > (lpAddress + low)) break;
          
          // last entry? bail out
          if (idd->rvaDLLName == 0 ||
              idd->rvaIAT     == 0 ||
              idd->rvaINT     == 0) {        
            break;
          }
          
          // old style imports?          
          x = (idd->grAttrs & 1) ? 0 : ImgBase();
          ofs = rva2ofs(idd->rvaINT - x);

          if (ofs != -1)
          {
            if (x32) {
              dump_imports32((PIMAGE_THUNK_DATA32)(ofs + lpAddress), x);
            } else {
              dump_imports64((PIMAGE_THUNK_DATA64)(ofs + lpAddress));
            }
          }
          // advance to next entry
          idd++;
        }
      }
    }
}

typedef struct com_info_t {
  char *s;
  REFIID iid;
} com_info;

/**
// firewall components
#include <netfw.h>
com_info fw_com[]={
  {"IID_INetFwRemoteAdminSettings", &IID_INetFwRemoteAdminSettings},
  {"IID_INetFwAuthorizedApplication", &IID_INetFwAuthorizedApplication},
  {"IID_INetFwAuthorizedApplications", &IID_INetFwAuthorizedApplications},
  {"IID_INetFwMgr", &IID_INetFwMgr},
  {"IID_INetFwRule", &IID_INetFwRule},
  {"IID_INetFwRule2", &IID_INetFwRule2},
  {"IID_INetFwRules", &IID_INetFwRules},
  {"IID_INetFwServiceRestriction", &IID_INetFwServiceRestriction},
  {"IID_INetFwOpenPort", &IID_INetFwOpenPort},
  {"IID_INetFwOpenPorts", &IID_INetFwOpenPorts},
  {"IID_INetFwIcmpSettings", &IID_INetFwIcmpSettings},
  {"IID_INetFwService", &IID_INetFwService},
  {"IID_INetFwServices", &IID_INetFwServices},
  {"IID_INetFwProfile", &IID_INetFwProfile},
  {"IID_INetFwPolicy", &IID_INetFwPolicy},
  {"IID_INetFwPolicy2", &IID_INetFwPolicy2},
  {"IID_INetFwProduct", &IID_INetFwProduct},
  {"IID_INetFwProducts", &IID_INetFwProducts},
  
  {NULL,NULL} };

//#include <msxml2.h>
#pragma comment (lib, "msxml2.lib")
com_info xml_com[]=
{{"IID_IXMLDOMDocument2",&IID_IXMLDOMDocument2},{NULL,NULL}};

#include <Exdisp.h>
//#pragma comment (lib, "msxml2.lib")
com_info ex_com[]=
{
  {"IID_IWebBrowser",&IID_IWebBrowser},
  {"IID_IWebBrowserApp",&IID_IWebBrowserApp},
  {"IID_IWebBrowser2",&IID_IWebBrowser2},
  {"IID_IShellWindows",&IID_IShellWindows},
  {NULL,NULL}};
  
 
#include <wbemcli.h>
#pragma comment (lib, "WbemUuid.lib")  
com_info wmi_com[]=
{
  {"IID_IWbemClassObject",&IID_IWbemClassObject},
  {"IID_IWbemObjectAccess",&IID_IWbemObjectAccess},
  {"IID_IWbemQualifierSet",&IID_IWbemQualifierSet},
  {"IID_IWbemServices",&IID_IWbemServices},
  {"IID_IWbemLocator",&IID_IWbemLocator},
  {"IID_IWbemObjectSink",&IID_IWbemObjectSink},
  {"IID_IEnumWbemClassObject",&IID_IEnumWbemClassObject},
  {"IID_IWbemCallResult",&IID_IWbemCallResult},
  {"IID_IWbemContext",&IID_IWbemContext},
  {"IID_IUnsecuredApartment",&IID_IUnsecuredApartment},
  {"IID_IWbemUnsecuredApartment",&IID_IWbemUnsecuredApartment},
  {"IID_IWbemStatusCodeText",&IID_IWbemStatusCodeText},
  {"IID_IWbemBackupRestore",&IID_IWbemBackupRestore},
  {"IID_IWbemBackupRestoreEx",&IID_IWbemBackupRestoreEx},
  {"IID_IWbemRefresher",&IID_IWbemRefresher},
  {"IID_IWbemHiPerfEnum",&IID_IWbemHiPerfEnum},
  {"IID_IWbemConfigureRefresher",&IID_IWbemConfigureRefresher},
  {"IID_IWbemShutdown",&IID_IWbemShutdown},
  {"IID_IWbemObjectTextSrc",&IID_IWbemObjectTextSrc},
  {"IID_IMofCompiler",&IID_IMofCompiler},
  {NULL,NULL}
};

#include <urlmon.h>
com_info url_com[]=
{
  {"IID_IAsyncMoniker",&IID_IAsyncMoniker},
  {"IID_IAsyncBindCtx",&IID_IAsyncBindCtx},
  {"IID_IPersistMoniker",&IID_IPersistMoniker},
  {"IID_IMonikerProp",&IID_IMonikerProp},
  {"IID_IBindProtocol",&IID_IBindProtocol},
  {"IID_IBinding",&IID_IBinding},
  {"IID_IBindStatusCallback",&IID_IBindStatusCallback},
  {"IID_IBindStatusCallbackEx",&IID_IBindStatusCallbackEx},
  {"IID_IAuthenticate",&IID_IAuthenticate},
  {"IID_IAuthenticateEx",&IID_IAuthenticateEx},
  {"IID_IHttpNegotiate",&IID_IHttpNegotiate},
  {"IID_IHttpNegotiate2",&IID_IHttpNegotiate2},
  {"IID_IHttpNegotiate3",&IID_IHttpNegotiate3},
  {"IID_IWinInetFileStream",&IID_IWinInetFileStream},
  {NULL,NULL}
};

//#include <napmanagement.h>
//#include <napcommon.h>
com_info com[]=
{
  {"IID_INapClientManagement",&IID_INapClientManagement},
  {"IID_INapClientManagement2",&IID_INapClientManagement2},
  {"IID_INapComponentInfo",&IID_INapComponentInfo},
  {"IID_INapComponentConfig",&IID_INapComponentConfig},
  {"IID_INapComponentConfig2",&IID_INapComponentConfig2},
  {"IID_INapComponentConfig3",&IID_INapComponentConfig3},
  {NULL,NULL}
};

#include <taskschd.h>
#pragma comment(lib, "taskschd.lib")
com_info ts_com[]=
{
  {"IID_ITaskFolderCollection",&IID_ITaskFolderCollection},
  {"IID_ITaskService",&IID_ITaskService},
  {"IID_ITaskHandler",&IID_ITaskHandler},
  {"IID_ITaskHandlerStatus",&IID_ITaskHandlerStatus},
  {"IID_ITaskVariables",&IID_ITaskVariables},
  {"IID_ITaskNamedValuePair",&IID_ITaskNamedValuePair},
  {"IID_ITaskNamedValueCollection",&IID_ITaskNamedValueCollection},
  {"IID_IRunningTask",&IID_IRunningTask},
  {"IID_IRunningTaskCollection",&IID_IRunningTaskCollection},
  {"IID_IRegisteredTask",&IID_IRegisteredTask},
  {"IID_ITrigger",&IID_ITrigger},
  {"IID_IIdleTrigger",&IID_IIdleTrigger},
  {"IID_ILogonTrigger",&IID_ILogonTrigger},
  {"IID_ISessionStateChangeTrigger",&IID_ISessionStateChangeTrigger},
  {"IID_IEventTrigger",&IID_IEventTrigger},
  {"IID_ITimeTrigger",&IID_ITimeTrigger},
  {"IID_IDailyTrigger",&IID_IDailyTrigger},
  {"IID_IWeeklyTrigger",&IID_IWeeklyTrigger},
  {"IID_IMonthlyTrigger",&IID_IMonthlyTrigger},
  {"IID_IMonthlyDOWTrigger",&IID_IMonthlyDOWTrigger},
  {NULL,NULL}
};
  
#include <bits.h>   
#pragma comment(lib, "Bits.lib")  
com_info bits_com[]=
{
  {"IID_IBackgroundCopyManager", &IID_IBackgroundCopyManager},  
  {"IID_IBackgroundCopyJobHttpOptions", &IID_IBackgroundCopyJobHttpOptions},  
  //{"IID_IBackgroundCopyJob5", &IID_IBackgroundCopyJob5},  
  {"IID_IBackgroundCopyJob4", &IID_IBackgroundCopyJob4},  
  {"IID_IBackgroundCopyJob3", &IID_IBackgroundCopyJob3},  
  {"IID_IBackgroundCopyJob2", &IID_IBackgroundCopyJob2},  
  {"IID_IBackgroundCopyJob",  &IID_IBackgroundCopyJob},  
  //{"IID_IBackgroundCopyFile6", &IID_IBackgroundCopyFile6},  
  //{"IID_IBackgroundCopyFile5", &IID_IBackgroundCopyFile5},  
  {"IID_IBackgroundCopyFile4", &IID_IBackgroundCopyFile4},  
  {"IID_IBackgroundCopyFile3", &IID_IBackgroundCopyFile3},  
  {"IID_IBackgroundCopyFile2", &IID_IBackgroundCopyFile2},  
  {"IID_IBackgroundCopyFile", &IID_IBackgroundCopyFile},  
  {"IID_IBackgroundCopyError", &IID_IBackgroundCopyError},  
  //{"IID_IBackgroundCopyCallback3", &IID_IBackgroundCopyCallback3},  
  {"IID_IBackgroundCopyCallback2", &IID_IBackgroundCopyCallback2},  
  {"IID_IBackgroundCopyCallback", &IID_IBackgroundCopyCallback},  
  {"IID_IBackgroundCopyJobHttpOptions", &IID_IBackgroundCopyJobHttpOptions},  
  {NULL,NULL}
};
*/

#include "iid_list.h"
#include "iid_dat.h"

void list_com(void)
{
  DWORD                 i, j, x, ofs;
  PIMAGE_SECTION_HEADER sec=SecHdr();
  PBYTE                 pRawData;
  
  // foreach section
  for (i=0; i<SecSize(); i++) 
  {
    ofs = rva2ofs (sec[i].VirtualAddress);
    
    if (ofs != -1)
    {
      pRawData = (PBYTE) (lpAddress + ofs);
      
      // is this initialized data?
      if (sec[i].Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA) {
        if (sec[i].SizeOfRawData < sizeof(IID)) continue;
        
        // for each component
        for (j=0; com[j].iid != NULL; j++) {
          // search data section
          for (x=0; x<sec[i].SizeOfRawData - sizeof(IID); x++) {
            if (memcmp((void*)com[j].iid, &pRawData[x], sizeof(IID))==0) {
              printf("Found %s in %s\n", com[j].s, file);
            }
          }
        }
      }
    }
  }
}

void scan_file(char path[])
{ 
    HANDLE                hFile, hMap;
    PIMAGE_DATA_DIRECTORY dir;
    
    file = path;
    api_cnt = 0;
    
    if (strstr(path, "aitstatic.exe")) return;
    
    hFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, 
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        
    if (hFile != INVALID_HANDLE_VALUE) {
      low = GetFileSize(hFile, &high);
      if (low > sizeof(IMAGE_DOS_HEADER) + sizeof(IMAGE_NT_HEADERS)) {
        hMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
        if (hMap != NULL) {
          lpAddress = (LPBYTE)MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
          if (lpAddress != NULL) {
            // valid DOS and NT header?
            if (valid_dos_hdr()) {
              if (valid_nt_hdr()) {
                dir = Dirs();
                if (dir != NULL) {
                  //printf ("trying %s\n", path);
                  //exp_dir(dir);
                  imp_dir(dir);
                  delay_dir(dir);
                  //list_com();
                }
              }
            }        
            UnmapViewOfFile((LPCVOID)lpAddress);
          }
          CloseHandle(hMap);
        }        
      }
      CloseHandle(hFile);
    }
    if (found) { putchar('\n'); found=0; }
}

#ifdef TEST
int main(int argc, char *argv[])
{
    if (argc < 2) {
      printf ("usage: pe_dump <EXE | DLL>\n");
      return 0;
    }    
    
    scan_file(argv[1]);
    
    return 0;
}
#endif

