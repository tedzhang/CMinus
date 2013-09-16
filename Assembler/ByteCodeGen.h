/********************************************************************
created:	2006/03/03
Copyright by Pointer(YongZhang81@gmail.com)

purpose:	Assembler implementaion. This module is to interpret the
            assembly intrustion to bytecode(machine code).
*********************************************************************/
#pragma  once
#include "Globals.h"
#include "DsDecl.h"
#include "Parse.h"

class CScanner;
class CByteCodeGen:public CParser
{
public:
	CByteCodeGen(CScanner *pScanner, CSymbolTable& symTbl,CHashTable & hashTbl,bool bGenereatedLsting= false);
	std::pair<const char *,int>  GetByteCodes();
	int GetErrors();
private:
	virtual void processInstruction(CScanner::Token &tok);
	virtual void processDirective(CScanner::Token &tok);
	//Instruction generation 
	void GenerateByteCodeForMove();
	void GenerateByteCodeForLea();
	void GenerateByteCodeForArithmOp(Operation op);
	void GenerateByteCodeForLogicOp(Operation op);
	void GenereateByteCodeForStackOp(Operation op);
	int m_iAddress;
	bool m_bGenereatedLst;
private:
	void EmitByteCode(const char* str,int len);
	inline void ClearBuffer(int,int);
	static const int  MAXFILESIZE = 4*1024;
	static const int MAXINSTRUCTIONSIZE = 12;
	int m_iCurPos;
	char buffer[MAXINSTRUCTIONSIZE+1];
	char bytecodesstr[MAXFILESIZE+1];
	FILE * m_fplstFile;
	int m_nErrors;
	//FOR generated list file 
	/*char lstflbuffer[MAXFILESIZE];
	char lstbuffer[255];
	int m_iLstPos;
	void EmitLst(const char* str,int len);*/

private:
	int HandleIndirAddress(int iStart,bool &bRelative2EBPOffset);
	int FindIdentiferAddr(const char* name,bool& bisGlobalVar);
};