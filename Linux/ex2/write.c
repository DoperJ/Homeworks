#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
main(int argc ,char * argv[])
{
 int testdev ;
 int i ;
 int a,b,c,d,e,f;
 int m[6];
 char buf[10] ;
 int pos = 0 ;

 testdev=open("/dev/scull",O_RDWR) ;
 if(testdev==-1)
 {
 printf("can not open file \n") ;

 }
 printf("the char device is opening \n") ;
 printf("please input the number: \n") ;
 //scanf("",&a,&b,&c,&d,&e,&f);
 scanf("%s",&m);
 
 pos=lseek(testdev,0,SEEK_SET);
 write(testdev,m,6) ;
 printf("the new buff has been written: %s\n",&m) ;

}