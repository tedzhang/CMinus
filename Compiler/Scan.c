/********************************************************************
created:	2006/02/25
created:	7:3:2006   0:28
Copyright by Pointer(YongZhang81@gmail.com)

purpose:	Lex parser implementation
*********************************************************************/
#include "Scan.h"

#define MAXLINEBUFFER 255

char tokenstring[MAXTOKENLENGTH+1];
char LineBuffer[MAXLINEBUFFER+1];//each line 

int linepos = 0;
int buffersize = 0;
static int lexErrors = 0;

//keywords find
BOOL ReservedLookUp(char *s,TokenType *pToken)
{
	static struct 
	{
		char *str;
		TokenType token;
	}ReservedWords[] =
	{
		{"else",kElse},
		{"if",kIf},
		{"int",kInt},
		{"return",kReturn},
		{"void",kVoid},
		{"while",kWhile},
	};
	static const int KEYWORDSNUM = sizeof(ReservedWords)/sizeof(ReservedWords[0]);
	int low = 0; int high = KEYWORDSNUM -1;
	int mid; int cond =0;
	while( low <= high)
	{ 
		mid = low +(high - low)/2;
		if((cond = strcmp(ReservedWords[mid].str,s)) < 0)//s > keywords[mid]
			low = mid +1;
		else if( cond > 0)
			high =  mid -1;
		else
		{ 
			*pToken = ReservedWords[mid].token;
		    return TRUE;
		}
	}
	return FALSE;

}


static char GetNextChar()
{
	if(linepos >= buffersize)
	{
		if(fgets(LineBuffer,MAXLINEBUFFER,sourceFile))
		{
			lineno++;
			buffersize = strlen(LineBuffer);
			linepos = 0;
			return LineBuffer[linepos++];
		}
		else
		{
			return EOF;
		}
	}
	return LineBuffer[linepos++];
}
static void  UnGetNextChar()
{
	--linepos;
}

TokenType GetToken()
{
	char ch ;
	char array[MAXTOKENLENGTH+1];
	int i =0;
	TokenType tokenType;
	BOOL bParse = TRUE;

	while(1)
	{
		ch = GetNextChar();
		if(ch =='\t'|| ch ==' ' || ch == '\n')
			continue;
		if(ch == EOF)//to the end of the file
			break;
        if(!bParse) 
		{
			if(ch == '*')
			{
			   if((ch = GetNextChar()) == '/')
			   {
				   bParse = TRUE;
				   
			   }
			   
			}
			continue;
				
		}

		if(isalpha(ch))
		{
			while(isalpha(ch))
			{
				array[i++] = ch;
				ch = GetNextChar();
			}
			UnGetNextChar();//back up one character
			array[i] ='\0';
			//Judge it's id or reserved keyword
			
			if(ReservedLookUp(array,&tokenType))
				return tokenType;
			else
			{
				strcpy(tokenstring,array);
				return kID;
			}
		}
		else if(isdigit(ch))
		{
			while(isdigit(ch))
			{
				array[i++] =ch;
				ch = GetNextChar();
			}
			UnGetNextChar();
			array[i] = '\0';
			strcpy(tokenstring,array);
			return kNumber;
		}
		else if(ch == '+')
		{
			return kPlus;
		}
		else if(ch == '-')
			return kMinus;
		else if(ch == '*')
		{
        	return kMul;
		}
		else if(ch == '/')
		{
			ch = GetNextChar();
			if(ch == '*')
			{ 
			  bParse = FALSE;
			  continue;
			}
			UnGetNextChar();//it's not comment.need back up
         	return kDiv;
		}
		else if(ch == '<')
		{
			ch = GetNextChar();
			if(ch == '=')
				return kLE;
			else
			{
				UnGetNextChar();
				return kLT;
			}
		}
		else if( ch == '>')
		{
			ch = GetNextChar();
			if(ch == '=')
				return kGE;
			else
			{
				UnGetNextChar();
				return kGT;
			}
		}
		else if(ch == '=')
		{
			ch = GetNextChar();
			if(ch == '=')
				return kEQ;
			else
			{
				UnGetNextChar();
				return kAssign;
			}
		}
		else if(ch =='!')
		{
			ch =GetNextChar();
			if(ch == '=')
				return kNE;
		}
		else if( ch == ';')
			return kSemicolon;
		else if(ch == ',')
			return kComma;
		else if( ch == '(')
			return kLParen;
		else if(ch == ')')
			return kRParen;
		else if(ch == '[')
			return kLMParen;
		else if(ch == ']')
			return kRMParen;
		else if( ch == '{')
			return kLBParen;
		else if( ch == '}')
			return kRBParen; 
		else
		{
			printf("Error occurred in line %d %c",lineno,ch);
			lexErrors++;
			return kError;
		}

	}
	return kEndFile;

}

int GetLexErrors()
{
	return lexErrors;
}
