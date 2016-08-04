#include "zbc_gb_core.h"
#include "zbc_gb_log.h"

#include <QCoreApplication>
#include <windows.h>

SERVICE_STATUS Status;
SERVICE_STATUS_HANDLE hStatus;

#define SERVICE_NAME L"GladBack"
int ARGC;
char** ARGV;

// What to do with stop signal to service
// TODO: add info about stop service to log file
void WINAPI ControlHandler( DWORD request )
{
    switch(request){
      case SERVICE_CONTROL_STOP:
        Status.dwWin32ExitCode = 0;
        Status.dwCurrentState = SERVICE_STOPPED;
        SetServiceStatus(hStatus, &Status);
        break;

      case SERVICE_CONTROL_SHUTDOWN:
        Status.dwWin32ExitCode = 0;
        Status.dwCurrentState = SERVICE_STOPPED;
        SetServiceStatus(hStatus, &Status);
        break;

      default:
        SetServiceStatus(hStatus, &Status);
        break;
    }
}


//Add info about service and run core of GladBack
void ServiceMain()
{
    Status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    Status.dwCurrentState = SERVICE_START_PENDING;
    Status.dwControlsAccepted = SERVICE_ACCEPT_STOP |
                                SERVICE_ACCEPT_SHUTDOWN;
    Status.dwWin32ExitCode = 0;
    Status.dwServiceSpecificExitCode = 0;
    Status.dwCheckPoint = 0;
    Status.dwWaitHint = 0;

    hStatus = RegisterServiceCtrlHandlerEx(SERVICE_NAME,
                                           (LPHANDLER_FUNCTION_EX)ControlHandler,
                                           0);
    if(hStatus == (SERVICE_STATUS_HANDLE)0)
        return;

    Status.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(hStatus, &Status);

// Run сore of GladBack
    QCoreApplication app(ARGC, ARGV);
    ZBC_GB_Core sysCore;
    sysCore.run();
    app.exec();

    while(1);
}


//
int main(int argc, char** argv)
{
    ARGC = argc;
    ARGV = argv;
    SERVICE_TABLE_ENTRY ServiceTable[1];
    ServiceTable[0].lpServiceName = (LPWSTR)SERVICE_NAME;
    ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;
    StartServiceCtrlDispatcher(ServiceTable);
}
