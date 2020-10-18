#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

using namespace std;

// Shows a pop up with a error message and exits the program with -1
void handleError(const char* title, const char*  message) {
	MessageBoxA(NULL, message, title, NULL);
	exit(-1);
}

// Checks if a file exists in the given path
bool fileExists(string file_path) {
	struct stat buffer;
	return (stat(file_path.c_str(), &buffer) == 0);
}

//Gets the process ID given the process name. Example: "notepad.exe"
DWORD getProcessId(const WCHAR* process_name)
{
	DWORD process_id = NULL;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hSnapshot != INVALID_HANDLE_VALUE) {
		PROCESSENTRY32 process_entry;
		process_entry.dwSize = sizeof(process_entry);

		if (Process32First(hSnapshot, &process_entry))
		{
			do {
				if (!_wcsicmp(process_entry.szExeFile, process_name))
				{
					process_id = process_entry.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnapshot, &process_entry));
		}
	}
	CloseHandle(hSnapshot);

	return process_id;
}

// Inject a dll in a process
void injectDLL(const char* dll_path, DWORD process_id) {
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
	if (hProcess != INVALID_HANDLE_VALUE) {
		//Allocate memory in the target process (size of the path string to the DLL)
		void *allocated_memory = VirtualAllocEx(hProcess, NULL, strlen(dll_path) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE); 
		
		//Writes the DLL path into the target process
		!WriteProcessMemory(hProcess, allocated_memory, dll_path, strlen(dll_path) + 1, nullptr);

		HANDLE hThread = CreateRemoteThread(hProcess, nullptr, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryA, allocated_memory, NULL, nullptr);
		
		//Cleanup
		if (hThread) CloseHandle(hThread);
		if (hProcess) CloseHandle(hProcess);
		VirtualFreeEx(hProcess, allocated_memory, NULL, MEM_RELEASE);
	}
}

int main()
{
	const string dll_path = "C:\\testDll.dll";
	const WCHAR* process_name = L"notepad.exe";

	cout << "\nChecking if DLL exists..." << endl;
	if (!fileExists(dll_path)) handleError("File not found", ("File \"" + dll_path + "\" not found").c_str());
	cout << "DLL exists!" << endl;

	cout << "\nGetting target process ID..." << endl;
	DWORD process_id = getProcessId(process_name);
	if (process_id == NULL) handleError("Couldn't get process ID", "Make sure the target process is running");
	cout << "Got it! It's: " << process_id << endl;

	wcout << "\nInjecting DLL in \"" << process_name << "\"..." << endl;
	injectDLL(dll_path.c_str(), process_id);
	cout << "DLL injected! \n";
}
