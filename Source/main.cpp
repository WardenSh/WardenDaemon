#include "Core/PackageObserver.h"

#include <cstdlib>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>

#include <bits/ostream.tcc>
#include <syslog.h>

void do_heartbeat()
{
    // TODO: implement processing code to be performed on each heartbeat
}

int main()
{
    /*

    // Fork the current process
   pid_t pid = fork();

   // The parent process continues with a process ID greater than 0
   if(pid > 0)
   {
      exit(EXIT_SUCCESS);
   }

   // A process ID lower than 0 indicates a failure in either process
   else if(pid < 0)
   {
      exit(EXIT_FAILURE);
   }

   // The parent process has now terminated, and the forked child process will continue
   // (the pid of the child process was 0)

   // Since the child process is a daemon, the umask needs to be set so files and logs can be written
   umask(0);

   // Open system logs for the child process
   openlog("daemon-named", LOG_NOWAIT | LOG_PID, LOG_USER);
   syslog(LOG_NOTICE, "Successfully started WardenDaemon");

   // Generate a session ID for the child process
   pid_t sid = setsid();
   // Ensure a valid SID for the child process
   if(sid < 0)
   {
      // Log failure and exit
      syslog(LOG_ERR, "Could not generate session ID for child process");

      // If a new session ID could not be generated, we must terminate the child process
      // or it will be orphaned
      exit(EXIT_FAILURE);
   }

   // Change the current working directory to a directory guaranteed to exist
   if((chdir("/")) < 0)
   {
      // Log failure and exit
      syslog(LOG_ERR, "Could not change working directory to /");

      // If our guaranteed directory does not exist, terminate the child process to ensure
      // the daemon has not been hijacked
      exit(EXIT_FAILURE);
   }

   // A daemon cannot use the terminal, so close standard file descriptors for security reasons
   close(STDIN_FILENO);
   close(STDOUT_FILENO);
   close(STDERR_FILENO);

   // Daemon-specific intialization should go here
   const int SLEEP_INTERVAL = 5;

   // Enter daemon loop
   while(1)
   {
      // Execute daemon heartbeat, where your recurring activity occurs
      do_heartbeat();

      // Sleep for a period of time
      sleep(SLEEP_INTERVAL);
   }

   // Close system logs for the child process
   syslog(LOG_NOTICE, "Stopping daemon-name");
   closelog();

   // Terminate the child process when the daemon completes
   exit(EXIT_SUCCESS);

    */


    auto packageManager = PackageManager::GetInstance();
    auto packageObserver = PackageObserver::GetInstance();

    packageManager->Init();
    packageObserver->Init();
    packageObserver->Start();


    sleep(99999);
    return 0;
}
