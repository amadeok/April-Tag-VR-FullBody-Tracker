#include "pipes.h"
using namespace std;

std::string GetLastErrorAsString()
{
	//Get the error message ID, if any.
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0)
	{
		return std::string(); //No error message has been recorded
	}
	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
	std::string message(messageBuffer, size);
	LocalFree(messageBuffer);

	return message;
}

HANDLE create_pipe(LPCWSTR pipe_path, bool start_java_server, bool start_pdf_server, int session_id, int id)
{
	HANDLE hwd_ = INVALID_HANDLE_VALUE;

	cout <<  " Creating pipe...\n";
#ifdef __linux__
	sprintf(input_pipe, "%s%s", "/tmp/epdiy_pc_monitor_a_", display_id);
	sprintf(output_pipe, "%s%s", "/tmp/epdiy_pc_monitor_b_", display_id);
	mkfifo(input_pipe, 0666);
	mkfifo(output_pipe, 0666);
#elif _WIN32




	hwd_ = CreateNamedPipe(pipe_path,
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, // FILE_FLAG_FIRST_PIPE_INSTANCE is not needed but forces CreateNamedPipe(..) to fail if the pipe already exists...
		1, 1024 * 64 * 16, 1024 * 64 * 16, NMPWAIT_USE_DEFAULT_WAIT, NULL);

	//printf("C++ ID %d: Connecting to pipe .. \n", id);
	
	bool connect_success = ConnectNamedPipe(hwd_, NULL) ?
		TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
	if (connect_success)
	{
		cout <<  id << " pipe " << pipe_path << " opened correctly\n";
	}
	else
	{
		auto err = GetLastErrorAsString().c_str();
		wxLogError(err);
		cout <<  id << " pipe " << pipe_path << " did NOT open correctly\n";

	}
	// pipe_handle.push_back(hwd_);
	return hwd_;
	// fd0 = CreateFile(TEXT(input_pipe), PIPE_ACCESS_DUPLEX, 0, NULL, OPEN_EXISTING, 0, NULL);

	// if (fd0 == INVALID_HANDLE_VALUE)
	//   printf("invalid handle fd0, error %d \n", GetLastError());

#endif
}


DWORD pipe_read(HANDLE handle, void* buffer, DWORD nNumberOfBytesToRead, DWORD lpNumberOfBytesRead)
{
	DWORD ret = 0;
#ifdef __linux__
	lpNumberOfBytesRead = read(handle, buffer, nNumberOfBytesToRead * sizeof(unsigned char));
#elif _WIN32
	ReadFile(handle, buffer, sizeof(unsigned char) * nNumberOfBytesToRead, &lpNumberOfBytesRead, NULL);
#endif
	return lpNumberOfBytesRead;
}

DWORD pipe_write(HANDLE handle, void* buffer, DWORD nNumberOfBytesToWrite, DWORD lpNumberOfBytesWritten)

{
#ifdef __linux__
	lpNumberOfBytesWritten = write(handle, buffer, nNumberOfBytesToWrite);
#elif _WIN32
	WriteFile(handle, buffer, nNumberOfBytesToWrite, &lpNumberOfBytesWritten, NULL);
#endif
	return lpNumberOfBytesWritten;
}