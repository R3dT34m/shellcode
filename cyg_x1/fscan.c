/**
  Copyright (C) 2016, 2017 Odzhan. All Rights Reserved.

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
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <windows.h>
#include <shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

char fullpath[MAX_PATH*2];

void scan_file(char path[]);

typedef void (*cb_func) (char path[], WIN32_FIND_DATA *wfd);

typedef struct _fs_t {
    DWORD files;
    DWORD dirs;
} fs_t;

fs_t cnt={0,0};

LONG WINAPI VectoredExceptionHandler(PEXCEPTION_POINTERS pExceptionInfo)
{
  printf("exception for %s\n", fullpath);
    return EXCEPTION_CONTINUE_SEARCH;
}

LONG WINAPI TopLevelExceptionHandler(PEXCEPTION_POINTERS pExceptionInfo)
{
  printf("exception for %s\n", fullpath);
    return EXCEPTION_CONTINUE_SEARCH;
}

void count_files (char path[], WIN32_FIND_DATA *wfd)
{
    if (wfd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
      cnt.dirs++;
    } else {
      cnt.files++;
    }
}

void print_files (char path[], WIN32_FIND_DATA *wfd)
{
    sprintf (fullpath, "%s\\%s", path, wfd->cFileName);
    
    if (wfd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
      //printf ("\r%-260s", fullpath);
    } else {
      //printf ("\n%s", fullpath);
      scan_file(fullpath);
    }
    count_files(NULL, wfd);
}

// scan fld for files matching fspec
void file_scan (char fld[], char fspec[], cb_func callback)
{
    WIN32_FIND_DATA wfd;
    HANDLE          hFind;
    char            path[1024];
    
    // scan for files matching specification
    sprintf (path, "%s\\%s", fld, fspec);
    
    hFind=FindFirstFile (path, &wfd);
    
    if (hFind!=INVALID_HANDLE_VALUE) {
      do {
        // not a directory?
        if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
          // callback?
          if (callback!=NULL) {
            callback (fld, &wfd);
          }
        }
      } while (FindNextFile (hFind, &wfd));
      FindClose (hFind);
    }
}

// scan fld for folders matching fldspec
// for each one found, scan it for files matching fspec
void fld_scan (char fld[], char fldspec[], char fspec[], cb_func callback, BOOL bFirst)
{
    WIN32_FIND_DATA wfd;
    HANDLE          hFind=NULL;
    char            path[1024];        
        
    // first scan fld for files matching fspec
    if (!bFirst) {
      //printf ("\n%s : ", fld);
      if (PathMatchSpec(fld, fldspec)) {
        file_scan (fld, fspec, callback);
      } else {
        //printf("failed");
      }
    } else {
      file_scan (fld, fspec, callback);
    }
    // scan fld for folders
    sprintf (path, "%s\\*", fld);    
   
    // now scan for folders matching fldspec
    hFind=FindFirstFile (path, &wfd);
    
    if (hFind!=INVALID_HANDLE_VALUE) {
      do {
        // if this is a directory
        if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
          // if this is not "." or ".."
          if ((strcmp (wfd.cFileName, "." )!=0) && 
              (strcmp (wfd.cFileName, "..")!=0))
          {
            // if it matches folder specification
            //if (PathMatchSpec(wfd.cFileName, fldspec)) {
              // if we have callback, call it
              if (callback!=NULL) callback(fld, &wfd);
              // format path
              sprintf (path, "%s\\%s", fld, wfd.cFileName); 
              // scan folder
              fld_scan (path, fldspec, fspec, callback, FALSE);              
            //}           
          }
        }
      } while (FindNextFile (hFind, &wfd));
      FindClose (hFind);
    }
}

void base_scan (char base[], char fldspec[], char fspec[])
{
  printf ("\nSearching %s\\%s for %s", base, fldspec, fspec);
  
  file_scan (base, fspec, print_files);
  fld_scan (base, fldspec, fspec, print_files, 1);
}

// example of using fscan function
int main (int argc, char *argv[])
{
    char *base=NULL, *fldspec="*", *fspec="*";
    DWORD dwSize = MAX_PATH;
    char  drives[MAX_PATH];
    PVOID OldValue=NULL;
    
    Wow64DisableWow64FsRedirection (&OldValue);
    
    if (argc<2) {
      printf ("  usage: fscan <base path> <folder spec> <file spec>\n");
      return 0;
    }
    
    AddVectoredExceptionHandler(1, VectoredExceptionHandler);
    SetUnhandledExceptionFilter(TopLevelExceptionHandler);
    
    base=argv[1];
    if (argc>=3) {
      fldspec=argv[2];
    }
    
    if (argc==4) {
      fspec=argv[3];
    }
    
    if (base[0]=='*') {
      // search all drives
      dwSize = GetLogicalDriveStrings(dwSize, drives);
      base = drives;
      while (*base) {
        //PathRemoveBackslash (base);
        fld_scan (base, fldspec, fspec, print_files, TRUE);
        base += strlen(base) + 1;
      }
    } else {
      fld_scan(base, fldspec, fspec, print_files, TRUE);
    }
    
    /*printf ("\n\nDirectories: %lu\nFiles: %lu\nTotal: %lu", 
      cnt.dirs, cnt.files, cnt.dirs + cnt.files);*/
    return 0;
}
