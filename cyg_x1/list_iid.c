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

#define UNICODE
  
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <windows.h>
#include <objbase.h>

#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "ole32.lib")

int main(void)
{
  HKEY    hKey, hSubKey;
  DWORD   err, idx, len, i;
  wchar_t sub_name[256], iid_name[256];
  IID     iid;
  wchar_t *p;
  FILE    *hdr, *dat;
  
  err=RegOpenKey(HKEY_CLASSES_ROOT, L"Interface", &hKey);
  if (err==ERROR_SUCCESS)
  {
    idx = 0;
    hdr = fopen("iid_list.h", "wb");
    dat = fopen("iid_dat.h",  "wb");
    
    fwprintf(dat, L"com_info com[]={\n");
    
    do {
      err=RegEnumKey (hKey, idx, sub_name, 256);
      if (err==ERROR_SUCCESS)
      {
        len=sizeof(iid_name);
        RegQueryValue(hKey, sub_name, iid_name, &len);
        if (iid_name[0]==L'I') {
          IIDFromString(sub_name, &iid);
          
          // remove space
          if ((p=wcschr(iid_name, L' '))!=NULL) *p=0;
          
          fwprintf(hdr, L"IID xIID_%s_%i={", iid_name, idx);
          len = lstrlen(iid_name);
          if (len < 70) {
            for (i=0; i<70-len; i++) {
              fwprintf(hdr, L" ");
            }
          }
          fwprintf(hdr, L"0x%08x, 0x%04x, 0x%04x, ", 
            iid.Data1, iid.Data2, iid.Data3);
            
          for (i=0; i<8; i++) {
            fwprintf(hdr, L"0x%02x", iid.Data4[i]);
            if ((i+1) != 8) fwprintf(hdr, L",");
          }          
          fwprintf(hdr, L"};\n");
          
          fwprintf(dat, L"{\"IID_%s\", &xIID_%s_%i},\n", iid_name, iid_name, idx);
        }
      }
      idx++;
    } while (err != ERROR_NO_MORE_ITEMS);
    fwprintf(dat, L"{NULL, NULL}");
    fwprintf(dat, L"};");
    fclose(dat); fclose(hdr);
    RegCloseKey(hKey);
  }
  return 0;
}
