#include "pch.h"
#include "winservice.h"

bool WinService::registerHandler(LPCWSTR name, LPHANDLER_FUNCTION handler)
{
	statusHandle = RegisterServiceCtrlHandler(name, handler);
	return statusHandle != NULL;
}

bool WinService::createStopEvent()
{
	serviceStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	return serviceStopEvent != NULL;
}

bool WinService::setStartPending()
{
	ZeroMemory(&serviceStatus, sizeof(serviceStatus));
	serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	serviceStatus.dwControlsAccepted = 0;
	serviceStatus.dwCurrentState = SERVICE_START_PENDING;
	serviceStatus.dwWin32ExitCode = 0;
	serviceStatus.dwServiceSpecificExitCode = 0;
	serviceStatus.dwCheckPoint = 0;
	return setStatus();
}

bool WinService::setStopped(DWORD exitCode = 0)
{
	serviceStatus.dwControlsAccepted = 0;
	serviceStatus.dwCurrentState = SERVICE_STOPPED;
	serviceStatus.dwWin32ExitCode = exitCode;
	serviceStatus.dwCheckPoint = 1;
	return setStatus();
}

bool WinService::setRunning()
{
	serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	serviceStatus.dwCurrentState = SERVICE_RUNNING;
	serviceStatus.dwWin32ExitCode = 0;
	serviceStatus.dwCheckPoint = 0;
	return setStatus();
}

bool WinService::setStopPending()
{
	serviceStatus.dwControlsAccepted = 0;
	serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
	serviceStatus.dwWin32ExitCode = 0;
	serviceStatus.dwCheckPoint = 4;
	return setStatus();
}


