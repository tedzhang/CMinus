/********************************************************************
created:	2006/03/05
Copyright by Pointer(YongZhang81@gmail.com)

purpose:	Assembler implementaion. 
*********************************************************************/

#include "Scan.h"
#include "Parse.h"
#include "DsDecl.h"
#include "ByteCodeGen.h"
#include "BinGen.h"

#define SRCFILE "..\\BIN\\test\\test1.asm"
#define DSTFILE "..\\BIN\\test\\test1.run"

#define KGENEXE  5

static bool ParseCmd(int argc,char *argv[],char* srcfilename,char* dstfilename,int *opinion)
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

	if(first != 'm' && first != 'm' && second != 's' && second != 'S' &&
		third != 'a' && third != 'A')
	{
		printf("Only asm source file is allowed\n");
		return false;
	}
	
	strcpy(dstfilename,srcfilename);
	memcpy(dstfilename+len -3,"run",4);
	return true;
}

static void ShowCopyright()
{
	printf("Minus -c Assembler v0.1\n");
	printf("Copyright pointer(yongzhang81@gmail.com)\n");

}

void main(int argc,char *argv[])
{

	ShowCopyright();

	char srcFileName[255];
	char dstFileName[255];
	int opinion;
	if(!ParseCmd(argc,argv,srcFileName,dstFileName,&opinion))
		return ;
	ASSERT(opinion == KGENEXE);

	FILE * fsrc = fopen(srcFileName,"r+");
	if(!fsrc) return;
	CScanner scanner(fsrc);
	CScanner::Token toke;
	toke.type = TOK_ENDFILE;
	

	CSymbolTable symTbl;
	CHashTable   hslTbl;
	CParser parser(&scanner,hslTbl,symTbl);
	parser.Process();
	fclose(fsrc);

	fsrc = fopen(srcFileName,"r+");
	scanner.ResetBuffer();
	scanner.SetSourceFile(fsrc);

	CByteCodeGen generator(&scanner,symTbl,hslTbl);
	generator.Process();

	CBinGenerator bingen(generator,symTbl,hslTbl);
	bingen.SetBinTargetFileName(dstFileName);
	bingen.GenerateBin();
    printf("\n------------------------Generate Executable File-----------------------\n");
	//success .print success info
	printf("\nExecutable file %s  is generated successfully\n",dstFileName);
	

	
	
}