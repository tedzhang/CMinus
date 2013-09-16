#include "stdafx.h"
#include "Proxy.h"

#define MAXDIRSIZE 255
inline CString CProxy::AppendExt(const CString& strFileName,const CString& fleExt )
{
	int pos = strFileName.ReverseFind('.');
	CString exefilename = strFileName;
	exefilename.Delete(pos,strFileName.GetLength() -pos);
	exefilename.Append(fleExt);
	return exefilename;
}
CString CProxy::Execute(const CString& strFileName,CProxy::ExeOpinion opinion)
{ 
	    bool bRunVM = false;
        switch(opinion)
		{
          case CProxy::kLEX:
          case CProxy::kSYNATX:
		  case CProxy::kTYPECHECKSYMTBL:
          case CProxy::kGENPCODE:
		  case CProxy::kGENASSEMCODE:
			  m_strCommandLine.Format("Compiler.exe -%d %s",opinion,strFileName);
			  break;
		  case CProxy::kGENEXE:
			  {
				CString asmFileName = AppendExt(strFileName,".asm");
				m_strCommandLine.Format("Assembler.exe -%d %s",opinion,asmFileName);
			  }
			  break;

		  case CProxy::kRUNEXE:
			  {
                CString exefilename = AppendExt(strFileName,".run");

                 m_strCommandLine.Format("VM.exe -%d %s",opinion,exefilename); 
			    bRunVM = true;
				Prepare(bRunVM,exefilename);

			  }
			  break;
		  default:
			  ASSERT(0);
		}
		if(!bRunVM)
		  Prepare(false,CString());
		return m_strResult;
}

BOOL CProxy::Prepare(bool bRunVM,const CString& exeFileName)
{
	//Step 1 :Create pipe
	SECURITY_ATTRIBUTES sa;
	sa.nLength=sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor=NULL;
	sa.bInheritHandle=TRUE;
	HANDLE hRead,hWrite;

	if(!CreatePipe(&hRead,&hWrite,&sa,0))
		return FALSE;

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	si.cb=sizeof(STARTUPINFO);

	GetStartupInfo(&si);
    if(!bRunVM)
	{
		si.hStdError=hWrite;
		si.hStdOutput=hWrite;
		si.dwFlags=STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	}
	else
	{
		si.lpTitle = (LPSTR)(LPCTSTR)exeFileName;
	}
	si.wShowWindow=bRunVM ?SW_SHOW:SW_HIDE;


	//Execute the consol exe
	if(!CreateProcess(NULL,(LPSTR)(LPCTSTR)m_strCommandLine,NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi))
	{
		return FALSE;
	}
	CloseHandle(hWrite);

	

	while(true)
	{
		DWORD bytesRead;
		if(!ReadFile(hRead,m_outputinfo,MAXOUTPUTINFOSIZE,&bytesRead,NULL))
			break;

		m_outputinfo[bytesRead] = '\0';
		m_strResult += m_outputinfo;
	}
}