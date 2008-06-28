
/*
'****************************************************************
'Windows API/Global Declarations
'****************************************************************
Declare Function ExitWindowsEx Lib "user32" (ByVal uFlags As Long, ByVal dwReserved As Long) As Boolean

Public Const EWX_LOGOFF = 0
Public Const EWX_SHUTDOWN = 1
Public Const EWX_REBOOT = 2
Public Const EWX_FORCE = 4


Public Sub ShutDownWindows95(ByVal pExitType As Byte)
  Dim Success As Boolean

  Success = ExitWindowsEx(pExitType, 0)

  If Success Then
    MsgBox "Shutting down Windows NOW!"
    End
  Else
    MsgBox "Function failed..."
  End If

End Sub


*/

/*


ExitWindowsEx
The ExitWindowsEx function either logs off the current user, shuts down the system, or shuts down and restarts the system. It sends the WM_QUERYENDSESSION message to all applications to determine if they can be terminated. 

BOOL ExitWindowsEx(
  UINT uFlags,       // shutdown operation
  DWORD dwReserved   // reserved
);
 
Parameters
uFlags 
Specifies the type of shutdown. This parameter must include one of the following values: Value Meaning 
EWX_LOGOFF Shuts down all processes running in the security context of the process that called the ExitWindowsEx function. Then it logs the user off. 
EWX_POWEROFF Shuts down the system and turns off the power. The system must support the power-off feature.
Windows NT: The calling process must have the SE_SHUTDOWN_NAME privilege. For more information, see the following Remarks section. 
 
EWX_REBOOT Shuts down the system and then restarts the system. 
Windows NT: The calling process must have the SE_SHUTDOWN_NAME privilege. For more information, see the following Remarks section. 
 
EWX_SHUTDOWN Shuts down the system to a point at which it is safe to turn off the power. All file buffers have been flushed to disk, and all running processes have stopped. 
Windows NT: The calling process must have the SE_SHUTDOWN_NAME privilege. For more information, see the following Remarks section. 
 


This parameter can optionally include the following values: Value Meaning 
EWX_FORCE Forces processes to terminate. When this flag is set, the system does not send the WM_QUERYENDSESSION and WM_ENDSESSION messages. This can cause the applications to lose data. Therefore, you should only use this flag in an emergency. 
EWX_FORCEIFHUNG Windows NT 5.0 and later: Forces processes to terminate if they do not respond to the WM_QUERYENDSESSION or WM_ENDSESSION message. This flag is ignored if EWX_FORCE is used. 



dwReserved 
Reserved; this parameter is ignored. 
Return Values
If the function succeeds, the return value is nonzero.

If the function fails, the return value is zero. To get extended error information, callGetLastError. 

Remarks
The ExitWindowsEx function returns as soon as it has initiated the shutdown. The shutdown or logoff then proceeds asynchronously. 

During a shutdown or log-off operation, applications that are shut down are allowed a specific amount of time to respond to the shutdown request. If the time expires, the system displays a dialog box that allows the user to forcibly shut down the application, to retry the shutdown, or to cancel the shutdown request. If the EWX_FORCE value is specified, the system always forces applications to close and does not display the dialog box. If the EWX_FORCEIFHUNG value is specified, the system forces hung applications to close and does not display the dialog box. 

Console processes receive a separate notification message, CTRL_SHUTDOWN_EVENT or CTRL_LOGOFF_EVENT, as the situation warrants. A console process routes these messages to its HandlerRoutine functions, which are added and removed by calls to the SetConsoleCtrlHandler function. ExitWindowsEx sends these notification messages asynchronously; thus, an application cannot assume that the console notification messages have been handled when a call to ExitWindowsEx returns. 

Windows NT: To shut down or restart the system, the calling process must use theAdjustTokenPrivileges function to enable the SE_SHUTDOWN_NAME privilege. For more information about security privileges, seePrivileges. 

Windows 95: ExitWindowEx does not work from a console application, as it does on Windows NT. 

QuickInfo
  Windows NT: Requires version 3.1 or later.
  Windows: Requires Windows 95 or later.
  Windows CE: Unsupported.
  Header: Declared in winuser.h.
  Import Library: Use user32.lib.

See Also
System Shutdown Overview, System Shutdown Functions,AdjustTokenPrivileges,HandlerRoutine,SetConsoleCtrlHandler 



  */
#define WINVER 0x0500
#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <stdio.h>
#include <time.h>
//#include <Powrprof.h> //Powrprof.lib for SetSuspendState
//#pragma comment(lib, "Powrprof.lib")

void error (char* func) {
   LPVOID lpMsgBuf;
   FormatMessage( 
       FORMAT_MESSAGE_ALLOCATE_BUFFER | 
       FORMAT_MESSAGE_FROM_SYSTEM | 
       FORMAT_MESSAGE_IGNORE_INSERTS,
       NULL,
       GetLastError(),
       MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
       (LPTSTR) &lpMsgBuf,
       0,
       NULL 
   );
   // Process any inserts in lpMsgBuf.
   // ...
   // Display the string.
   //MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
   printf("%s: %s", func, (LPCTSTR)lpMsgBuf);
   // Free the buffer.
   LocalFree( lpMsgBuf );
}


int countdown(int hour, int min, int sec, bool do_beep = false) {
   const int beep_every_x_sec = 10;
   static int beep_count = 0;
   char last_buf[128];
   char buf[128];
   time_t t1, t2, t;
   /*struct tm *newtime;*/
   time(&t1);
   t2 = t1 + (hour*60+min)*60 + sec;
   {
      struct tm *diff;
      time_t d = t2 - t1;
      diff = gmtime(&d);
      sprintf(last_buf, "%02d:%02d:%02d", diff->tm_hour, diff->tm_min, diff->tm_sec);
      printf("%s", last_buf);
   }
   if (do_beep) {
      printf("\007");
      beep_count = beep_every_x_sec;
   }

   t = t1;
   while (t < t2) {
      SleepEx(200, false);
      time(&t);
      {
         struct tm *diff;
         time_t d = t2 - t;
         diff = gmtime(&d); /*newtime = localtime(&t1);*/
         sprintf(buf, "%02d:%02d:%02d", diff->tm_hour, diff->tm_min, diff->tm_sec);
         if (strcmp(buf, last_buf)){
            if (do_beep) {
              if (beep_count == 0) {
                printf("\007");
                beep_count = beep_every_x_sec;
              }
              beep_count--;
            }
            printf("\b\b\b\b\b\b\b\b");
            printf("%s", buf);
            strcpy(last_buf, buf);
         }
      }
   }
   printf("\n");
   return 0;
}


int main (int argc, char** argv) {
/*
int WINAPI WinMain(
  HINSTANCE hInstance,  // handle to current instance
  HINSTANCE hPrevInstance,  // handle to previous instance
  LPSTR lpCmdLine,      // pointer to command line
  int nCmdShow          // show state of window
  ) {
  */
   if (0) {
      if (InitiateSystemShutdown("Alan", "messages", 10, false, false)) {
         printf("success\n");
      } else {
         printf("failure\n");

         LPVOID lpMsgBuf;
         FormatMessage( 
             FORMAT_MESSAGE_ALLOCATE_BUFFER | 
             FORMAT_MESSAGE_FROM_SYSTEM | 
             FORMAT_MESSAGE_IGNORE_INSERTS,
             NULL,
             GetLastError(),
             MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
             (LPTSTR) &lpMsgBuf,
             0,
             NULL 
         );
         // Process any inserts in lpMsgBuf.
         // ...
         // Display the string.
         //MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
         printf("%s", (LPCTSTR)lpMsgBuf);
         // Free the buffer.
         LocalFree( lpMsgBuf );
 
      }
      return 0;
   }
   if (0) {
      if (LockWorkStation()) printf("success\n");
      else printf("failure\n");
      return 0;
   }

   
   if (argc < 4) {
     printf("USAGE: %s hour min sec [suspend:1,shutdown:0] [quiet:(0,1)] [test:(0,1)]\n", argv[0]);
     printf("  sample: suspend after 1 hour quietly: %s 1 0 0 1 1\n", argv[0]);
     printf("  sample: shutdown after 1 hour with warning beeps: %s 1 0 0\n", argv[0]);
      return -1;
   }

   bool is_suspend;
   char* operation_string;
   if (argc >= 5 && atoi(argv[4]) > 0) {
     is_suspend = true;
     operation_string = "suspend";
   } else {
     is_suspend = false;
     operation_string = "shutdown";
   }

    bool is_quiet = false;
    if (argc >= 6) {
      is_quiet = atoi(argv[5]) > 0;
      printf("is_quiet=%s\n", is_quiet ? "true" : "false");
    }

    bool is_test = false;
    if (argc >= 7) {
      is_test = atoi(argv[6]) > 0;
      printf("is_test=%s\n", is_test ? "true" : "false");
    }


   {
      int hour, min, sec;
      hour = atoi(argv[1]); min = atoi(argv[2]); sec = atoi(argv[3]);

      time_t t1, t2;
      time(&t1); t2 = t1 + (hour*60+min)*60+sec;

      printf("start     time: %s", ctime(&t1));
      printf("shutdown  time: %s", ctime(&t2));
   

      /* countdown to shutdown */
      if (is_test) {
        printf("test mode.\nT-minus to ending program (computer will not %s): ", operation_string);
      } else {
        printf("T-minus to %s: ", operation_string);
      }
      countdown(hour, min, sec);

      /* countdown for chance to cancel */
      printf("T-minus for chance to cancel: ");
      countdown(0, 0, 45, !is_quiet);

      if (is_test) {
        return 0;
      }

      /*
      Windows NT: To shut down or restart the system, the calling process must use the AdjustTokenPrivileges
      function to enable the SE_SHUTDOWN_NAME privilege. For more information about security privileges, see Privileges. 
      */

      {
         HANDLE hToken; 
         TOKEN_PRIVILEGES tkp; 
 
         // Get a token for this process. 
         if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
             error("OpenProcessToken"); 
 
         // Get the LUID for the shutdown privilege. 
         LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
 
         tkp.PrivilegeCount = 1;  // one privilege to set
         tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
 
         // Get the shutdown privilege for this process. 
         AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0); 
 
         // Cannot test the return value of AdjustTokenPrivileges. 
         if (GetLastError() != ERROR_SUCCESS) error("AdjustTokenPrivileges"); 
 
         // Shut down the system and force all applications to close. 
         //if (!ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0)) error("ExitWindowsEx"); 
 
      }

      if (is_suspend) {
        HMODULE hPowerFunctions = LoadLibrary("Powrprof.dll");
        typedef BOOLEAN (WINAPI*tSetSuspendStateSig)(BOOLEAN, BOOLEAN, BOOLEAN);

        tSetSuspendStateSig SetSuspendState = (tSetSuspendStateSig)GetProcAddress(hPowerFunctions, "SetSuspendState");
        
        if (SetSuspendState(FALSE, TRUE, FALSE)) {
          printf("suspending..."); // successful
        } else {
          LPVOID lpMsgBuf;
          if (!FormatMessage( 
              FORMAT_MESSAGE_ALLOCATE_BUFFER | 
              FORMAT_MESSAGE_FROM_SYSTEM | 
              FORMAT_MESSAGE_IGNORE_INSERTS,
              NULL,
              GetLastError(),
              MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
              (LPTSTR) &lpMsgBuf,
              0,
              NULL ))
          {
            // Handle the error.
            printf("error suspending. also error finding out why.");
          } else {
            // Process any inserts in lpMsgBuf.
            // ...

            // Display the string.
            //MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
            printf((LPCTSTR)lpMsgBuf);

            // Free the buffer.
            LocalFree( lpMsgBuf );
          }

        }
      } else {
        if (ExitWindowsEx(EWX_POWEROFF | EWX_FORCE, 0)) printf("shutting down..."); //EWX_POWEROFF | EWX_FORCEIFHUNG
        else error("ExitWindowsEx");
      }
      
      return 0;
   }
}

/*

  BOOL InitiateSystemShutdown(
  LPTSTR lpMachineName,   // pointer to name of computer to shut down
  LPTSTR lpMessage,          // pointer to message to display in 
                             // dialog box
  DWORD dwTimeout,           // time to display dialog box
  BOOL bForceAppsClosed,     // force applications closed flag
  BOOL bRebootAfterShutdown  // reboot flag
);
 

  */