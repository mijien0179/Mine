#pragma once

template<typename T>
void SafeRelease(T*& ppt) {
	if (ppt) {
		ppt->Release();
		ppt = nullptr;
	}
}