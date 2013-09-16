#pragma  once

class CProxy
{
public:
	typedef enum
	{
		kLEX = 0,
		kSYNATX,
		kTYPECHECKSYMTBL,
		kGENPCODE,
		kGENASSEMCODE,
		kGENEXE,
		kRUNEXE,
	}ExeOpinion;

	CString Execute(const CString& strFileName,CProxy::ExeOpinion opinion);
private:
	BOOL Prepare(bool bRunVM = false,const CString& exeFileName=CString());
	inline CString AppendExt(const CString& strFileName,const CString& fleExt );
private:
	CString m_strCommandLine;
	static const int MAXOUTPUTINFOSIZE = 1024*4;
	char m_outputinfo[MAXOUTPUTINFOSIZE];
	CString m_strResult;

};