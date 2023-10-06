// showtime.cc

#include<stdio.h>
#include<time.h>

int main( void )
{
  puts(
       "Content-type: text/html\n"
       "\n"
       "<HTML>\n"
       "<HEAD>\n"
       "<TITLE> Show the current time </TITLE>\n"
       "</HEAD>\n"
       "<BODY bgcolor="#e0ffc8">\n"
       "<H2>"
       );

  time_t t = time(NULL);

  printf( "<P>The current time is listed below.<BR>\n"
	  "%s.</P>", ctime(&t) );

  puts(
       "</H2>\n"
       "<HR>\n"
       "</BODY>\n"
       "</HTML>"
       );

  return 0;
}
