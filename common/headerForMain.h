#pragma once
#include <windows.h>

// ǿ���� Nvidia GPU ����
// 
#ifdef FORCE_NVIDIA_GPU
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
#endif

using namespace std;
