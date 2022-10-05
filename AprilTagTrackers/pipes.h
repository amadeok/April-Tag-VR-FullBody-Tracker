

#include <tlhelp32.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <winnt.h>

std::string GetLastErrorAsString();

HANDLE create_pipe(LPCWSTR pipe_path, bool start_java_server, bool start_pdf_server, int session_id, int id);

DWORD pipe_read(HANDLE handle, void* buffer, DWORD nNumberOfBytesToRead, DWORD lpNumberOfBytesRead);

DWORD pipe_write(HANDLE handle, void* buffer, DWORD nNumberOfBytesToWrite, DWORD lpNumberOfBytesWritten);