#include <stdlib.h>
//exit()
#include <syslog.h>
//openlog(), syslog(), closelog()

int 
main(int argc, char **argv)
{
	openlog("LOGTEST", LOG_PID, LOG_USER);

	/* where ? /var/log/ */
	syslog(LOG_NOTICE, "NOTICE");//syslog, message
	syslog(LOG_ERR, "ERR");//syslog
	syslog(LOG_WARNING, "WARNING");//syslog, message
	syslog(LOG_INFO, "INFO");//syslog, message
	syslog(LOG_DEBUG, "DEBUG");//syslog, debug


	/* format */
	int a = 10;
	syslog(LOG_INFO, "integer %d", a);//syslog, message
	double b = -10.5;
	syslog(LOG_INFO, "double float %lf", b);//syslog, message

	closelog();
	exit(0);
}
