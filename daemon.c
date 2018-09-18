#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <syslog.h>

#define DAEMON_NAME "behemoth"


/*
 * Pro ukonceni beziciho daemona:
 *	ps -e
 *	kill -pid-
 *
 * Pro vypsani logu:		
 * 	(sudo) tail -n 100 /var/log/messages
 */


//nas daemon process
void process()
{
	syslog(LOG_NOTICE, "Writing to my Syslog");
}

int main()
{
	//nastaveni co logovat, co ne; Pro debug zapnute, pro live erzi lze jednoduse zmenit maska a logovat se prestane
	setlogmask(LOG_UPTO(LOG_NOTICE));

	//otevre logovani
	openlog(DAEMON_NAME, LOG_CONS | LOG_NDELAY | LOG_PERROR | LOG_PID, LOG_USER);

	//zaloguje start
	syslog(LOG_NOTICE, "Starting my daemon!");

	//promenne pro procesy
	pid_t pid, sid;

	//forkneme parent process
	pid = fork();
	if(pid < 0) 
	{
		return 1;
	}

	//ukoncime parent process
	if(pid > 1) 
	{
		return 0;	
	}

	//zmena file masky neboli zmena permissions
	umask(0);

	//vytvorime novou signaturu id pro nase dite
	sid = setsid();
	if(sid < 0)
	{
		return 1;
	}

	//zmena aktualniho pracovniho adresare; V pripade, ze selze, se ukonci proces
	//lze menit podle vlastniho uvazeni
	if((chdir("/")) < 0)
	{
		return 1;
	}

	//zavreni File Descriptoru aneb vsech standartnich vstupu a vystupu na terminal/ dosouboru/ ...
	//pokud je odkomentovane, tak se process() zapisuje do logu, a na terminal se vypise jen to, ze se spusti daemon.
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	/* --- MAIN PROCESS --- */
	while(1)
	{
		process();
		sleep(60);
	}

	closelog();

	return 0;
}

