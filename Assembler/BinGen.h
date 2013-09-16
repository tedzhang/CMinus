/********************************************************************
created:	2006/03/04
Copyright by Pointer(YongZhang81@gmail.com)

purpose:	Assembler implementaion. This module is to exectuable file 
            for the virtual machine.
*********************************************************************/

#pragma  once
#include "Globals.h"
#include "DsDecl.h"

class CByteCodeGen;
class CBinGenerator
{
public:
	CBinGenerator(CByteCodeGen& byteGen,CSymbolTable& symbolTbl,CHashTable& hslTbl);
	void SetBinTargetFileName(const char* strTarFileName);
	void GenerateBin();
	~CBinGenerator();
private:
	FILE *m_fpBin;
	CByteCodeGen &m_byteGen;
	CSymbolTable &m_symTbl;
	CHashTable   &m_hasTbl;
private:
	inline void Write(char* buffer,int val);
	inline void Write(char* buffer,const char *str,int size);
	inline void Write(char* buffer,char ch);
	int m_icurpos;
	static const int MAXOPRANDSIZE =4;
	char outbuffer[MAXOPRANDSIZE];

	

};
