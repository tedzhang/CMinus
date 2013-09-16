/********************************************************************
created:	2006/02/27
Copyright by Pointer(YongZhang81@gmail.com)

purpose:	Minu-c compiler implementation. For this project's detail
            Refer to the book <<Compiler principle and practice>>
*********************************************************************/

#include "Globals.h"
#include "Scan.h"
#include "Parse.h"
#include "Cgen.h"
#include "SymUtil.h"

#define OUTPUT2CONSOLE 0 //0 We use the ui as output,1: use console window

int lineno = 0;
FILE *sourceFile = NULL;
FILE *targetFile = NULL;

#define kLEX  0
#define kSYNATX 1
#define kTYPECHECKSYMTBL 2
#define kGENPCODE        3
#define kGENASSEMCODE    4
#define MAX_FILEPATH    256

#define SOURCEFILE "..\\BIN\\test\\test1.c"
#define TARGETFILE "..\\BIN\\test\\test1.asm"

static BOOL ParseCmd(int argc,char *argv[],char* srcfilename,char* dstfilename,int *opinion)
{  
	char *sopinion = NULL,filename = NULL;
	int len = 0,i = 0;
	if(argc < 3)
	{
		printf("usage error: compiler -i filename\n");
		return FALSE;
	}
	sopinion = argv[1];

	strcpy(srcfilename,argv[argc-1]);
	//srcfilename = argv[2];

	//opinio is like this "-i".Try to get the i
	*opinion = atoi(++sopinion);
	//Get the source file and to generate a dest file's name
	len = strlen(srcfilename);
	if(srcfilename[len -1] != 'c' && srcfilename[len -1] != 'C')
	{
		printf("Only c source file is allowed\n");
		return FALSE;
	}
	strcpy(dstfilename,srcfilename);
	if(*opinion == kGENASSEMCODE)
	memcpy(dstfilename+len -1,"asm",4);
	else if(*opinion == kGENPCODE)
	{
		memcpy(dstfilename+len-1,"pa",3);	
	}
	
	return TRUE;
}

static void ShowCopyInfo()
{
	printf("Minus -c Compiler v0.1\n");
	printf("Copyright Pointer(YongZhang81@gmail.com)\n");

}


void main(int argc,char* argv[])
{
	char srcfilename[MAX_FILEPATH];
	char dstfilename[MAX_FILEPATH];
	int opinion = -1;
	ShowCopyInfo();
	if( !ParseCmd(argc,argv,srcfilename,dstfilename,&opinion))
		return;
	sourceFile = fopen(srcfilename,"r");
	if(!sourceFile) return;

	switch( opinion)
	{
	case kLEX:
		{
			TokenType token ;
			do
			{
				token = GetToken();
			}while(token != kEndFile);
			printf("...................................\n");
			printf("Lex errors: %d errors\n",GetLexErrors());
			fclose(sourceFile);
		}
		break;
	case kSYNATX:
		{
           TreeNode *t;
		   t = parse();
		   PrintTree(t);
		   fclose(sourceFile);
		}
		break;
	case kTYPECHECKSYMTBL:
		{
			TreeNode *t;
			t = parse();
			if(GetParseErrors() == 0)
			{
				Init_HashTable();
				BuildSymTable(t);
				PrintSymbTable();
				TypeCheck(t);

				printFunctionTable();

			}

			fclose(sourceFile);

		}
		break;
	case kGENPCODE:
		{
			TreeNode *t;
			
			t = parse();
			if(GetParseErrors() == 0)
			{
				Init_HashTable();
				BuildSymTable(t);
				TypeCheck(t);
				if(GetSemanticErrors() == 0)
				{
					targetFile = fopen(dstfilename,"w+");
					if(!targetFile) 
					{
						printf("Can not open %s for writing\n",dstfilename);
						return;
					}
					SetGenPcodeOnly();
#if OUTPUT2CONSOLE==1
					SetOutput2Console();
#endif 
					PCodeGen(t,targetFile,TRUE);
					printf("%s is generated successfully\n",dstfilename);

				}
	
			}
			if(targetFile)
			 fclose(targetFile);
		}
		break;
	case kGENASSEMCODE:
		{
			TreeNode *t;
			
			t = parse();
			if(GetParseErrors() == 0)
			{
				Init_HashTable();
				BuildSymTable(t);
				TypeCheck(t);
				if(GetSemanticErrors() == 0)
				{
					targetFile = fopen(dstfilename,"w+");
					if(!targetFile) 
					{
						printf("Can not open %s for writing\n",dstfilename);
						return;
					}
#if OUTPUT2CONSOLE==1
					SetOutput2Console();
#endif
					GenAsmCode(t,targetFile);
					printf("%s is generated successfully\n",dstfilename);
				}

			}
			if(targetFile)
			 fclose(targetFile);
		}
		break;
	default:
		{
			printf("Error: unsupport command\n");
			break;
		}
	}
	

}