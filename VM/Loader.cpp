/********************************************************************
created:	2006/03/07
created:	7:3:2006   0:25
Copyright by Pointer(YongZhang81@gmail.com)

purpose: The loader module. This moudle will load the bytecode file
and read them into the memory
*********************************************************************/

#include <windows.h>
#include "Loader.h"

int CExeLoader::m_totalBytes;
CExeLoader::CExeLoader(const char* exeFileName)
{
	m_fpExe = fopen(exeFileName,"r+");
	ASSERT(m_fpExe);
	m_filename = exeFileName;
}

int CExeLoader::GetTotalBytes()
{
	return m_totalBytes;
}
bool CExeLoader::InitVM()
{
    int exefilesize = GetFileSize(m_filename);
	ASSERT(exefilesize );
    ExeHeaderRec headerrc;
	if(!GetHeaderInfo(headerrc))
		return false;
	ASSERT(headerrc.iBytecodeSize != 0);
	if(headerrc.iBytecodeSize <= 0)
		return false;


	int bytecodestart = EXE_HEADERSIZE + headerrc.iStrTblSize + headerrc.iSymTblSize;
	int bytecodeend   = (bytecodestart)+(headerrc.iBytecodeSize -1);
	int totalbytes    = DEFAULT_HEAPSIZE *1024 + DEFAULT_STACKSIZE *1024+headerrc.iBytecodeSize;

	//To do:check the request memory is less than available memory
    
	RAM = (char*)malloc(totalbytes);
	m_totalBytes = totalbytes;

	Reg[TargetMachine::EIP] = 0;
	Reg[TargetMachine::ESP] = Reg[TargetMachine::EBP] = totalbytes-1;
	
	MemRegs[BE] = headerrc.iBytecodeSize -1;
	MemRegs[HS] = headerrc.iBytecodeSize;
	MemRegs[HE] = headerrc.iBytecodeSize +DEFAULT_HEAPSIZE*1024-1;

	MemRegs[SB] = MemRegs[HE]+1;
	MemRegs[ST] = totalbytes-1;

	//Read the byte code to RAM
	//Step 1: Go to the exe file's byte code start position
	//Remember ,we just finished reading the header information
	fseek(m_fpExe,headerrc.iStrTblSize+headerrc.iStrTblSize,SEEK_CUR);
	char *pbytecodes = (char*)malloc(headerrc.iBytecodeSize);
	if(fread((void*)pbytecodes,sizeof(char),headerrc.iBytecodeSize,m_fpExe) != headerrc.iBytecodeSize)
	{
		printf("The exectuable file is corrputed\n");
		return false;
	}
    memcpy(RAM,pbytecodes,headerrc.iBytecodeSize);
	/*for(int i = 0; i < headerrc.iBytecodeSize;++i)
	{
		RAM[i]= *pbytecodes++;
	}*/

    fclose(m_fpExe);
	free(pbytecodes);

	return true;
}

int CExeLoader::GetFileSize(const char* fname)
{
	WIN32_FILE_ATTRIBUTE_DATA fdata;
	if(::GetFileAttributesEx(fname,GetFileExInfoStandard,&fdata))
		return fdata.nFileSizeLow;
	return 0;
}

bool CExeLoader:: GetHeaderInfo(ExeHeaderRec &rec)
{
	//ExeHeaderRec rec;
	char buffer[255];
	if(fread((void*)&buffer,sizeof(char),EXE_HEADERSIZE,m_fpExe) < EXE_HEADERSIZE)
	{
		ASSERT(0);
		printf("reading error\n");
	}
	else if(buffer[0] != MAGIC_NUM1 || buffer[1] != MAGIC_NUM2)
	{
		printf("this file is not an executable file\n");
		ASSERT(0);
	}
	else
	{
		rec.magic[0] = MAGIC_NUM1;
		rec.magic[1] = MAGIC_NUM2;
		rec.iSymTblSize = ::Bytecode2Dword(&buffer[2]);
		rec.iStrTblSize = ::Bytecode2Dword(&buffer[6]);
		rec.iBytecodeSize = ::Bytecode2Dword(&buffer[10]);
		return true;
	}
	return false;

}