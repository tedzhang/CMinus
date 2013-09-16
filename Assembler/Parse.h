#pragma  once
#include "Globals.h"
#include "Scan.h"

class CHashTable;
class CSymbolTable;

class CParser
{
public:
	CParser(CScanner *scanner,CHashTable& hashTable,CSymbolTable &symTable);
	bool  Process();
	int GetErrorNums();
protected:
	bool match(Directive expdir);
	bool match(TokenType expType);
	bool match(Register expectedRg);
	bool match(Operation expop);
protected:
	virtual void processDirective(CScanner::Token &tok);
	virtual void processInstruction(CScanner::Token &tok);

protected:
	CScanner *m_pScanner;
	CHashTable &m_hashTbl;
	CSymbolTable &m_symbolTbl;
	CScanner::Token m_curTok;
private:
	
	//For generated address
	int m_GlobalOffset;
	int m_Address;
	
};