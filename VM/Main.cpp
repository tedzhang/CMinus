/********************************************************************
created:	2006/03/07
created:	7:3:2006   0:25
Copyright by Pointer(YongZhang81@gmail.com)

purpose: Virtual-machine implementation
*********************************************************************/
#include "Globals.h"
#include "Loader.h"
#include "RunEngine.h"
#include <stdlib.h>
#include <string.h>
#include <conio.h>

 int Reg[9];
 char* RAM = NULL;
 int MemRegs[5];

 static bool ParseCmd(int argc,char *argv[],char* srcfilename,int *opinion)
 {  
	 char *sopinion = NULL,filename = NULL;
	 int len = 0,i = 0;
	 if(argc < 3)
	 {
		 printf("usage error: compiler -i filename\n");
		 return false;
	 }
	 sopinion = argv[1];
	 strcpy(srcfilename,argv[argc-1]);
	 //srcfilename = argv[2];

	 //opinio is like this "-i".Try to get the i
	 *opinion = atoi(++sopinion);
	 //Get the source file and to generate a dest file's name
	 len = strlen(srcfilename);
	 char first = srcfilename[len-1];
	 char second = srcfilename[len -2];
	 char third  = srcfilename[len-3];

	 if(first != 'n' && first != 'N' && second != 'u' && second != 'U' &&
		 third != 'r' && third != 'R')
	 {
		 printf("Only .run exe file is allowed\n");
		 return false;
	 }
	 return true;
 }

 static void ShowCopyright()
 {
	 //because I need to redirect these info to the ui exe
	 printf("Minus -c Virtual Machine v0.1\n");
	 printf("Copyright pointer(yongzhang81@gmail.com)\n");

 }

#define MAX_PATHNAME 256

 void main(int argc,char* argv[])
 {
	 ShowCopyright();
	 int opinion;
	 char srcFilename[MAX_PATHNAME];
	 if(!ParseCmd(argc,argv,srcFilename,&opinion))
		 return;

	 CExeLoader loader(srcFilename);
	 loader.InitVM();

	 CRunEngine runner;
	 runner.Execute();

	 //Wait for user's input .Give chance for user to
	 //watch the result
	 printf("Press any key to close...\n");
	 getch();

 }