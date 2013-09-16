#pragma once 

/********************************************************************
created:	2006/03/07
created:	7:3:2006   0:25
Copyright by Pointer(YongZhang81@gmail.com)

purpose: The loader module. This moudle will load the bytecode file
         and read them into the memory
*********************************************************************/
#include "Globals.h"
#include "Utils.h"

class CExeLoader
{
public:
  CExeLoader(const char* exeFileName);
  bool InitVM();
  static int GetTotalBytes();
private:
	

//file reading routines
private:
	bool GetHeaderInfo(ExeHeaderRec &rec);

private:
	int GetFileSize(const char* fname);
	const char* m_filename;
	static int m_totalBytes;

private:
	FILE *m_fpExe;


};