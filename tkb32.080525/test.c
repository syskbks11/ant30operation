#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

/*
 * 'open_port()' - Open serial port 1.
 *
 * Returns the file descriptor on success or -1 on error.
 */

int main(void)
{
  int fd; /* File descriptor for the port */
  char buffer[255];  /* Input buffer */
  struct termios options;
  int nbytes;

  fd = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY);
  printf("fd %d\n",fd);
  if (fd == -1)
    {
      /*
       * Could not open the port.
       */

      perror("open_port: Unable to open /dev/ttyS0 - ");
    }
  else
    fcntl(fd, F_SETFL, 0);



  /*
   * Get the current options for the port...
   */
  tcgetattr(fd, &options);

  /*
   * Set the baud rates to 19200...
   */
  cfsetispeed(&options, B4800);
  cfsetospeed(&options, B4800);

  /*
   * Enable the receiver and set local mode...
   */
  options.c_cflag     |= (CLOCAL | CREAD);
  options.c_lflag     &= ~(ICANON | ECHO | ECHOE | ISIG);
  options.c_oflag     &= ~OPOST;
  options.c_cc[VMIN]  = 0;
  options.c_cc[VTIME] = 10;
  options.c_cflag &= ~PARENB;
  options.c_cflag &= ~CSTOPB;
  options.c_cflag &= ~CSIZE;
  options.c_cflag |= CS8;

  /*
   * Set the new options for the port...
   */
  tcsetattr(fd, TCSANOW, &options);

  while(1)
    {
      /* read characters into our string buffer until we get a CR or NL */
      nbytes = read(fd, buffer, 254);
      if(nbytes>0){
	buffer[nbytes]=0;
	printf("%s", buffer);
      }
    }

  return (-1);
}
