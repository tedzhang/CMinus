#pragma once 
#include "Globals.h"
#include <vector>
#include <map>
#include <string>
typedef struct globalVariable
{
	int textidx;//The index of this string in the string table;
	kDataType dType;
	int iArraySize;//if it's array.it's the arraysize.
	int size;//total byte size
	int offset;//address
	int lineno;
}GlobalVar;

typedef enum
{
	kRetVoid = 0,
	kRetVal  =1,
}ProcedureRetType;
//function
typedef struct stackFrame
{
	int textidx;
	int fpOffset;//offset relative to EBP
	int line;
}StackFrame;
typedef struct label 
{
	int textidx;
	int address;//offset relative to EBP
	int line;
}Label;

typedef struct  procedure
{
	int textidx;
	int address;
	int line;
	/*ProcedureRetType retType;
    
	StackFrame *arg;
	StackFrame *retAddress;*/
	std::vector<Label *> labels;
	int GetSize()
	{
		int iLabelSize = labels.size()*sizeof(Label);
	     return 12+iLabelSize;
	}

	/*int nArgCount;
	int nLocalVarCount;
	int nLabelCount;*/

}Procedure;



class CSymbolTable
{
public:
	typedef std::vector<GlobalVar*> GLOBALVARTABLE;
	typedef std::vector<Procedure*> PROCEDURETABLE;
	GLOBALVARTABLE& GetGlobalVarTable();
	PROCEDURETABLE& GetProcsTable();
	int  AddGlobalVar(GlobalVar *pGblVar);//return the inserted element's position
	int  AddProcedure(Procedure *pProc);
	std::pair<int,int>  AddLabel(Label *lbl);
	
private:
	GLOBALVARTABLE m_globalVars;
	PROCEDURETABLE m_procs;

};
typedef std::vector<const char*> STRINGTABLE;

class CHashTable
{
public:

	typedef enum
	{

		kGlobalVar = 0,
		kProc   ,
		//kProcRet,
		//kProcLoc,
		kProcLbl,
	}ElementType;

	typedef struct elementRc
	{
		//ElementType type;
		int textidx;//idx to the string table;
		ElementType type;
		union
		{
			int GlobalVarPos;//the index in the global variable array 
			struct  
			{
				int iProcIdx;
				int iLabelPos;
			} ProcIdx;
		}Index;
	}ElementRc;
	typedef std::map<std::string,ElementRc*> ItemBucket;
	ElementRc *Lookup(const char* str);
	void       Insert(const char* str,ElementType type,int iGlobalPos,int iProcPos,int iLabelPos);
	STRINGTABLE& GetStringTable(){ return m_strTable; }
	int          GetStringTableSize(){ return m_strTable.size();}

private:
	static const int TABLESIZE = 211;
	ItemBucket m_hashtbl[TABLESIZE];
    STRINGTABLE m_strTable;
	static int hashpjw(const char *s);
};

