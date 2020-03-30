#pragma once

#include <Windows.h>

class WinService
{
	SERVICE_STATUS serviceStatus;
	SERVICE_STATUS_HANDLE statusHandle;
	HANDLE serviceStopEvent = INVALID_HANDLE_VALUE;
public:
	static WinService* instance() {
		static WinService service;
		return &service;
	}

	bool registerHandler(LPCWSTR name, LPHANDLER_FUNCTION handler);
	bool createStopEvent();

	bool setStartPending();
	bool setStopped(DWORD exitCode);
	bool setRunning();
	bool setStopPending();

	HANDLE getStopEvent() {
		return serviceStopEvent;
	}

	DWORD getState() {
		return serviceStatus.dwCurrentState;
	}

	bool closeStopEvent() {
		return CloseHandle(serviceStopEvent);
	}
	
private:

	bool setStatus() {
		return SetServiceStatus(statusHandle, &serviceStatus);
	}

	WinService() {}
};

