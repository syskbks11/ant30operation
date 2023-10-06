#include <stdio.h>
#include <curses.h>

#define YES 1
#define NO 0
 
main()
{
 int ans;

 ans=NO;
 initscr();
 raw();
 noecho;
 do{
   if(ans==NO){
     printf("YES\r");
     ans=YES;
   }else{
     printf("NO \r");
     ans=NO;
   }
 }while(getch()>=30);

 endwin();
 exit(ans);
}
