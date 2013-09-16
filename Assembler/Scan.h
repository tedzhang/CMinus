#pragma  once 
#include "Globals.h"
#include <map>
#include <string>

class CScanner
{
public:
	typedef struct toKen 
	{
		
		int lineno;
		TokenType type;
		union
		{
			char* toKenstring;
			Register reg;
			Directive directive;
			Operation op;
		}Contents;
	}Token;

    CScanner(FILE*);
	Token GetToken();
	int GetErrors();
	void SetSourceFile(FILE* psrc){ m_fpSource = psrc;}
	void ResetBuffer(){ lineno = 0;linepos = 0; buffersize = 0;lexerrors = 0;}
	~CScanner();
private:
	char GetNextChar();
	void unGetNextChar();
	static const int MAXLINEBUFFERSIZE = 255; 
	static const int MAXTOKENLENGTH = 255;
	char LineBuffer[MAXLINEBUFFERSIZE];
	int linepos;
	int buffersize;
	int lineno;
	int lexerrors;
	FILE *m_fpSource;
private:
	Register GetReg(const char* regname);
	Directive GetDirective(const char *dirname);
	Operation GetOperation(const char* opname);
	//To do.add code for don't distinguish upper case and lower case
//	struct LessThan
//	{
//		bool operator()(const std::string& lhs,const std::string &rhs) const
//		{
//			tolower()toupper()
//		}
//	};

	typedef std::map<std::string,Register> Str2RegMap;
	typedef std::map<std::string,Operation> Str2OpMap;
	typedef std::map<std::string,Directive> Str2DirMap;

	static Str2DirMap m_str2dirmap;
	static Str2OpMap  m_str2opmap;
	static Str2RegMap m_str2RegMap;


};