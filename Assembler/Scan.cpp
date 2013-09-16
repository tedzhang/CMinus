#include "Scan.h"

CScanner::Str2DirMap CScanner::m_str2dirmap;
CScanner::Str2OpMap  CScanner::m_str2opmap;
CScanner::Str2RegMap CScanner::m_str2RegMap;

CScanner::CScanner(FILE *fpsrc):lineno(0),linepos(0),buffersize(0),lexerrors(0)
{
  m_fpSource = fpsrc;
}

char CScanner::GetNextChar()
{
	if(linepos >= buffersize)
	{
		if(fgets(LineBuffer,MAXLINEBUFFERSIZE,m_fpSource))
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

void  CScanner::unGetNextChar()
{
	--linepos;
}

CScanner::Token CScanner::GetToken()
{
  
	Token token;
	token.lineno = lineno;
	bool bInComment = false;
	char array[MAXTOKENLENGTH];

	while(1)
	{
		char ch = GetNextChar();
		if(ch =='\t'|| ch ==' '|| (ch != '\n'&& bInComment == true))
			continue;
		if(ch == EOF || ch == 0)//some times the 0 is the end of file flag to the end of the file
			break;
		if(ch == ';')
		{
			bInComment = true;
			continue;
		}
		else if( ch == '\n')
		{
			bInComment = false;
			token.type = TOK_NOMORE;
			return token;
		}
		else if(ch == ',')
		{
			token.type = TOK_COMMA;
			return token;
		}
		else if(ch == '[')
		{
			token.type = TOK_LPAREN;
			return token;
		}
		else if(ch == ']')
		{
			token.type = TOK_RPAREN;
			return token;
		}
		else if(ch == '+')
		{
			token.type = TOK_ADD;
			return token;
		}
		else if( ch == '-')
		{
			token.type = TOK_SUB;
			return token;
		}
		else if(ch =='.')
		{
			ch = GetNextChar();
			int i = 0;
			while(isalpha(ch))
			{
				array[i++] = ch;
				ch = GetNextChar();
			}
			unGetNextChar();
			array[i] ='\0';
			token.type = TOK_DIRECTIVE;
			token.Contents.directive = GetDirective(array);
			return token;

		}
		else if(isalpha(ch))
		{
			int i = 0;
			while(isalpha(ch) || isdigit(ch))
			{
				array[i++] = ch;
				ch = GetNextChar();
			}
			unGetNextChar();//back up one character
			array[i] ='\0';
			//Judge it's reg /op or id
			Register rg;
			Operation op;
			if((rg = GetReg(array))!= NRG)
			{
				token.type = TOK_REG;
				token.Contents.reg = rg;
			}
			else if((op = GetOperation(array)) != (Operation)(-1))
			{
				token.type = TOK_OPERATION;
				token.Contents.op = op;
			}
			else
			{
				token.type = TOK_ID;
				token.Contents.toKenstring = strdup(array);
			}
			return token;
		}
		else if(isdigit(ch))
		{
			int i = 0;
			while(isdigit(ch))
			{
				array[i++] = ch;
				ch = GetNextChar();
			}
			unGetNextChar();//back up one character
			array[i] ='\0';
			token.type = TOK_INT_CONST;
			token.Contents.toKenstring = strdup(array);
			return token;
		}
		else
		{
			printf("error token line %d %c\n",lineno,ch);
			lexerrors++;
			break;
		}


	}
	token.type = TOK_ENDFILE;
	return token;
}

Register CScanner::GetReg(const char* regname)
{
	if(m_str2RegMap.size() == 0)
	{
		int i = 0; int isize = sizeof(RegNames)/sizeof(RegNames[0]);
		for(;i < isize ;++i)
		{
			m_str2RegMap.insert(std::make_pair(RegNames[i].regName,RegNames[i].reg));
		}
	}
	Str2RegMap::const_iterator iter = m_str2RegMap.find(regname);
	return iter == m_str2RegMap.end()? NRG:iter->second;
  
}
Directive CScanner::GetDirective(const char *dirname)
{
	if(m_str2dirmap.size() == 0)
	{
		int i = 0; int isize = sizeof(Directives)/sizeof(Directives[0]);
		for(; i <isize;++i)
		{
			
			m_str2dirmap.insert(std::make_pair(Directives[i].name,Directives[i].dir));
		}
	}
	//for directive ,we have the "." before it.so we need remove it.
	Str2DirMap::iterator iter = m_str2dirmap.find(dirname);
//	ASSERT(iter != m_str2dirmap.end());

	return iter->second;


}
Operation CScanner::GetOperation(const char* opname)
{
	if(m_str2opmap.size() == 0)
	{
		int i = 0; int isize = sizeof(OpNames)/sizeof(OpNames[0]);
		for(;i <isize;++i)
		{
			m_str2opmap.insert(std::make_pair(OpNames[i].name,OpNames[i].op));
		}
	}
	Str2OpMap::iterator iter = m_str2opmap.find(opname);
	//ASSERT(iter != m_str2opmap.end());
	return iter == m_str2opmap.end()?(Operation)(-1):iter->second;

}
int CScanner::GetErrors()
{
	return lexerrors;
}
CScanner::~CScanner()
{ 
	fclose(m_fpSource);
}