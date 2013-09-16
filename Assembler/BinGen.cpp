/********************************************************************
created:	2006/03/04
Copyright by Pointer(YongZhang81@gmail.com)

purpose:	Assembler implementaion. This module is to exectuable file 
            for the virtual machine.
*********************************************************************/
#include "BinGen.h"
#include "ByteCodeGen.h"
#define MAGIC_NUM1 0x44
#define MAGIC_NUM2 0x58


#define MAXEXESIZE 10*1024

CBinGenerator::CBinGenerator(CByteCodeGen& byteGen,CSymbolTable& symbolTbl,CHashTable& hslTbl)
:m_byteGen(byteGen),m_symTbl(symbolTbl),m_hasTbl(hslTbl),m_icurpos(0)
{

}
void CBinGenerator::SetBinTargetFileName(const char* strTarFileName)
{
  m_fpBin = fopen(strTarFileName,"w+");
  ASSERT(m_fpBin != NULL);
}
void CBinGenerator::GenerateBin()
{
   int iSymTblSize = 0;
   //Calculate symboltbl size
   //iSymTblSize += m_symTbl.GetGlobalVarTable().size() *sizeof(GlobalVar);
  // iSymTblSize += m_symTbl.GetProcsTable().size() *(m_symTbl.GetProcsTable().at(0).GetSize());
   int iStrTblSize = m_hasTbl.GetStringTable().size();

   std::pair<const char*,int> bytecodepair = m_byteGen.GetByteCodes();
   int bytecodesize = bytecodepair.second;

   char * pBuffer = (char*) malloc(MAXEXESIZE);

   //
   Write(pBuffer,(char)(MAGIC_NUM1));
   Write(pBuffer,(char)(MAGIC_NUM2));
   int i = 0;
#ifdef GenDebugData
   for(i=0; i < m_symTbl.GetGlobalVarTable().size();++i)
   {
	   GlobalVar * pVar = m_symTbl.GetGlobalVarTable().at(i);
	   Write(pBuffer,pVar->textidx);
	   Write(pBuffer,pVar->dType);
	   Write(pBuffer,pVar->iArraySize);
	   Write(pBuffer,pVar->size);
	   Write(pBuffer,(-1)*pVar->offset);
	   Write(pBuffer,pVar->lineno);
	   
   }
   //To do: add other's write routines
   //so the exe can be debugged
   for(i = 0; i < iStrTblSize;++i )
   {
	   const char *str =  m_hasTbl.GetStringTable().at(i);
	   Write(pBuffer,str,strlen(str)+1);
   }
#endif 
   //write symbol table's size.if including debug data,it should not be 0.so 
   //it's data should be write down
   //write string table's size.same as above
   //write bytecodes's size.same as above
   Write(pBuffer,0);
   Write(pBuffer,0);
   Write(pBuffer,bytecodesize);
   //write byte codes
   Write(pBuffer,bytecodepair.first,bytecodesize);
   fwrite(pBuffer,sizeof(char),m_icurpos,m_fpBin);

   fclose(m_fpBin);
   free(pBuffer);

}

inline void CBinGenerator:: Write(char* buffer,int val)
{
	DWord2Bytecode(val,outbuffer);
	memcpy((void*)(buffer+m_icurpos),outbuffer,4);
	m_icurpos += 4;
}
inline void CBinGenerator::Write(char* buffer,const char *str,int size)
{
	memcpy((void*)(buffer+m_icurpos),str,size);
	m_icurpos += size;

}
inline void CBinGenerator::Write(char* buffer,char ch)
{
	//memcpy((void*))
	memcpy((void*)(buffer+m_icurpos),&ch,1);
	m_icurpos += 1;
}
CBinGenerator::~CBinGenerator()
{
	fclose(m_fpBin);
}