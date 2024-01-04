// DLLX_impl.h
#include "DLLX.h"
#include <Windows.h>
#include <TlHelp32.h>
#include <TlHelp32.h>

namespace DLLX {
    void InjectDLL(DWORD processId, const wchar_t* dllPath) {
        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);

        if (hProcess) {
            size_t pathLen = wcslen(dllPath) + 1;
            LPVOID remoteString = VirtualAllocEx(hProcess, nullptr, pathLen * sizeof(wchar_t), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

            if (remoteString) {
                WriteProcessMemory(hProcess, remoteString, dllPath, pathLen * sizeof(wchar_t), nullptr);
                HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0, (LPTHREAD_START_ROUTINE)LoadLibraryW, remoteString, 0, nullptr);

                if (hThread) {
                    WaitForSingleObject(hThread, INFINITE);
                    CloseHandle(hThread);
                }

                VirtualFreeEx(hProcess, remoteString, 0, MEM_RELEASE);
            }

            CloseHandle(hProcess);
        }
    }

    void EjectDLL(DWORD processId, const wchar_t* dllName) {
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);

        if (hSnapshot != INVALID_HANDLE_VALUE) {
            MODULEENTRY32W moduleEntry;
            moduleEntry.dwSize = sizeof(MODULEENTRY32W);

            if (Module32FirstW(hSnapshot, &moduleEntry)) {
                do {
                    if (wcscmp(moduleEntry.szModule, dllName) == 0) {
                        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);

                        if (hProcess) {
                            HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0, (LPTHREAD_START_ROUTINE)FreeLibrary, moduleEntry.hModule, 0, nullptr);

                            if (hThread) {
                                WaitForSingleObject(hThread, INFINITE);
                                CloseHandle(hThread);
                            }

                            CloseHandle(hProcess);
                        }

                        break;
                    }
                } while (Module32NextW(hSnapshot, &moduleEntry));
            }

            CloseHandle(hSnapshot);
        }
    }

    void SuspendProcess(DWORD processId) {
        HANDLE hThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        if (hThreadSnapshot != INVALID_HANDLE_VALUE) {
            THREADENTRY32 te32;
            te32.dwSize = sizeof(THREADENTRY32);

            if (Thread32First(hThreadSnapshot, &te32)) {
                do {
                    if (te32.th32OwnerProcessID == processId) {
                        HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);
                        if (hThread != NULL) {
                            SuspendThread(hThread);
                            CloseHandle(hThread);
                        }
                    }
                } while (Thread32Next(hThreadSnapshot, &te32));
            }

            CloseHandle(hThreadSnapshot);
        }
    }

    void ResumeProcess(DWORD processId) {
        HANDLE hThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        if (hThreadSnapshot != INVALID_HANDLE_VALUE) {
            THREADENTRY32 te32;
            te32.dwSize = sizeof(THREADENTRY32);

            if (Thread32First(hThreadSnapshot, &te32)) {
                do {
                    if (te32.th32OwnerProcessID == processId) {
                        HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te32.th32ThreadID);
                        if (hThread != NULL) {
                            ResumeThread(hThread);
                            CloseHandle(hThread);
                        }
                    }
                } while (Thread32Next(hThreadSnapshot, &te32));
            }

            CloseHandle(hThreadSnapshot);
        }
    }

    DWORD GetProcessIdByName(const wchar_t* processName) {
        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);

        HANDLE hProcessSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        if (hProcessSnapshot != INVALID_HANDLE_VALUE) {
            if (Process32First(hProcessSnapshot, &pe32)) {
                do {
                    if (wcscmp(pe32.szExeFile, processName) == 0) {
                        CloseHandle(hProcessSnapshot);
                        return pe32.th32ProcessID;
                    }
                } while (Process32Next(hProcessSnapshot, &pe32));
            }

            CloseHandle(hProcessSnapshot);
        }

        return 0;
    }

    void InjectDLLByName(const wchar_t* processName, const wchar_t* dllPath) {
        DWORD processId = GetProcessIdByName(processName);
        if (processId != 0) {
            InjectDLL(processId, dllPath);
        }
    }

    void EjectDLLByName(const wchar_t* processName, const wchar_t* dllName) {
        DWORD processId = GetProcessIdByName(processName);
        if (processId != 0) {
            EjectDLL(processId, dllName);
        }
    }
}
