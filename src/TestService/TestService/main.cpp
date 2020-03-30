#include "pch.h"
#include <tchar.h>
#include <iostream>

#include "server.h"
#include "winservice.h"
#include <Windows.h>

io_service ioService;

#define SERVICE_NAME  _T("Garland")

VOID WINAPI ServiceMain(DWORD argc, LPTSTR* argv);
VOID WINAPI ServiceCtrlHandler(DWORD);
void StopHandler(const boost::system::error_code& err);

VOID WINAPI ServiceMain(DWORD argc, LPTSTR* argv)
{
	DWORD Status = E_FAIL;

	// Register our service control handler with the SCM
	WinService* winService = WinService::instance();

	if (!winService->registerHandler(SERVICE_NAME, &ServiceCtrlHandler)) {
		return;
	}
	
	if (!winService->setStartPending()) {
		OutputDebugString(_T(
			"ServiceMain: SetServiceStatus returned error"));
	}

	if (!winService->createStopEvent()) {
		if (!winService->setStopped(GetLastError())) {
			OutputDebugString(_T(
				"ServiceMain: SetServiceStatus returned error"));
		}
		return;
	}

	if (!winService->setRunning()) {
		OutputDebugString(_T(
			"ServiceMain: SetServiceStatus returned error"));

	}

	// Handle stop event with boost::asio
	boost::asio::windows::object_handle stop(ioService, winService->getStopEvent());
	stop.async_wait(&StopHandler);
	auto srv = Server::create(ioService, 8001);
	srv->run();
	ioService.run();

	winService->closeStopEvent();

	if (!winService->setStopped(0)) {
		OutputDebugString(_T(
			"ServiceMain: SetServiceStatus returned error"));
	}
}

void StopHandler(const boost::system::error_code& err)
{
	ioService.stop();
}

VOID WINAPI ServiceCtrlHandler(DWORD CtrlCode)
{
	WinService* winService = WinService::instance();

	switch (CtrlCode)
	{
	case SERVICE_CONTROL_STOP:

		if (winService->getState() != SERVICE_RUNNING)
			break;

		if (!winService->setStopPending()) {
			OutputDebugString(_T(
				"ServiceCtrlHandler: SetServiceStatus returned error"));
		}

		SetEvent(winService->getStopEvent());

		break;

	default:
		break;
	}
}

int _tmain(int argc, TCHAR* argv[])
{
	SERVICE_TABLE_ENTRY serviceTable[] = {
		{(LPWSTR)SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain},
		{NULL, NULL}
	};

	if (StartServiceCtrlDispatcher(serviceTable) == FALSE) {
		return GetLastError();
	}
	return 0;
}
