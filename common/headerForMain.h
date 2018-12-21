#pragma once
#include <windows.h>

// 强制用 Nvidia GPU 运行
// 
#ifdef FORCE_NVIDIA_GPU
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
#endif

using namespace std;
