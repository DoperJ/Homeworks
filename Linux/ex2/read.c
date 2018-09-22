#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
main(int argc ,char * argv[])
{
 int testdev ;
 int i ;
 char buf[6] ;
 int pos = 0 ;

 testdev=open("/dev/scull",O_RDWR) ;
 if(testdev==-1)
 {
 printf("can not open file ") ;

 }
 printf("the char device is opening \n") ;

 pos = lseek(testdev,0,SEEK_SET);
 read(testdev,buf,6) ;
 printf("the buff is as follows:\n") ;
 for(i = 0 ;i <6 ;i++)
 printf("%c",buf[i]) ;
 printf("\n") ;
 close(testdev) ;
}