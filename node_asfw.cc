#include "node_asfw.h"
#include <string>
#include <iostream>
#include <sstream>
#include <winuser.h>

#if defined(WIN32)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN

void setForegroundWindow(HWND &hwnd) {
    WINDOWPLACEMENT wp;
    wp.length = sizeof(WINDOWPLACEMENT);

    if (GetWindowPlacement(hwnd, &wp)) {
    	//std::wcout << "showCmd= " << wp.showCmd << std::endl;
    	//std::wcout << "flags= " << wp.flags << std::endl;
        if (wp.showCmd == SW_MINIMIZE || wp.showCmd == SW_SHOWMINIMIZED || wp.showCmd == SW_SHOWMINNOACTIVE) {
        	//std::wcout << "RESTORE MODE" << std::endl;
            ShowWindow(hwnd, SW_RESTORE);
        }
        else {
        	//std::wcout << "SHOW MODE" << std::endl;
            ShowWindow(hwnd, SW_SHOW);
    	}
    }
    else {
        ShowWindow(hwnd, SW_SHOW);
    }

    BOOL ret = SetForegroundWindow(hwnd);
    if (ret == 0) {
        DWORD lastError = GetLastError();
        if (lastError != 0) {
            std::stringstream ss;
            ss << "Windows error code " << lastError;
            auto err = ss.str();
            Nan::ThrowError(err.c_str());
        }
    }
}

const wchar_t* GetWC(const char *c)
{
    const size_t cSize = strlen(c)+1;
    wchar_t* wc = new wchar_t[cSize];
    mbstowcs (wc, c, cSize);

    return wc;
}

const std::wstring CONVERSION_FAILED_STR = L"<string conversion failed>";
std::wstring ToCString(v8::Isolate *isolate, v8::Local<v8::String> &str) {
	//	auto t = *k;
	//	v8::String::Utf8Value value(k);
	v8::String::Utf8Value value(isolate, str);

	if (*value) {
		std::wstring out = GetWC(*value);
		return out;
	}
	else {
		return CONVERSION_FAILED_STR;
	}
}

struct enumWindowsState {
	int found = 0;
	std::wstring toFind;
};

BOOL CALLBACK _SetForegroundWindowByName_callback(HWND hwnd, LPARAM lParam) {
    const DWORD TITLE_SIZE = 1024;
	WCHAR windowTitle[TITLE_SIZE];
	GetWindowTextW(hwnd, windowTitle, TITLE_SIZE);
	enumWindowsState *state = reinterpret_cast<enumWindowsState*>(lParam);

	std::wstring title(windowTitle);

	//std::wcout << "Looking for " << state->toFind << " in " << title << std::endl;
	if (title.find(state->toFind) != std::wstring::npos) {
		if (state->found == 0) {
			setForegroundWindow(hwnd);
		}
		state->found++;
	}

	return true;
}

NAN_METHOD(_SetForegroundWindowByName) {
    if (info[0]->IsUndefined()) {
        Nan::ThrowError("SetForegroundWindowByName needs one argument");
    }
    Nan::MaybeLocal<v8::String> maybeArg = Nan::To<v8::String>(info[0]);
    if (maybeArg.IsEmpty()) {
        Nan::ThrowError("SetForegroundWindowByName needs a string argument");
    }

	v8::Isolate *isolate = info.GetIsolate();
	auto windowName = ToCString(isolate, maybeArg.ToLocalChecked());

	enumWindowsState state;
	state.toFind = windowName;

	//std::wcout << "Looking for " << state.toFind << std::endl;
    EnumWindows(_SetForegroundWindowByName_callback, reinterpret_cast<LPARAM>(&state));
}

NAN_METHOD(_SetForegroundWindow) {
    if (info[0]->IsUndefined()) {
        Nan::ThrowError("SetForegroundWindow needs one argument");
    }
    auto maybeArg = Nan::To<int64_t>(info[0]);
    if (maybeArg.IsNothing()) {
        Nan::ThrowError("SetForegroundWindow needs a number argument");
    }

    HWND hwnd = (HWND)(maybeArg.FromJust());
    setForegroundWindow(hwnd);
}

#else // defined(WIN32)

NAN_METHOD(_SetForegroundWindow) {
    Nan::ThrowError("SetForegroundWindow is not implemented on this platform");
}

NAN_METHOD(_SetForegroundWindowByName) {
    Nan::ThrowError("SetForegroundWindowByName is not implemented on this platform");
}

#endif // !defined(WIN32)
