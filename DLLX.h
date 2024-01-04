#pragma once
// DLLX.h
#pragma once
#include "DLLX_impl.h"

#ifdef DLLX_EXPORTS
#define DLLX_API __declspec(dllexport)
#else
#define DLLX_API __declspec(dllimport)
#endif

namespace DLLX {
    DLLX_API void InjectDLL(DWORD processId, const wchar_t* dllPath);
    DLLX_API void EjectDLL(DWORD processId, const wchar_t* dllName);
    DLLX_API void SuspendProcess(DWORD processId);
    DLLX_API void ResumeProcess(DWORD processId);
    DLLX_API DWORD GetProcessIdByName(const wchar_t* processName);
    DLLX_API void InjectDLLByName(const wchar_t* processName, const wchar_t* dllPath);
    DLLX_API void EjectDLLByName(const wchar_t* processName, const wchar_t* dllName);
}