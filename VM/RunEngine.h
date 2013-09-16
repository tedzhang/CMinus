#pragma  once 

/********************************************************************
created:	2006/03/07
created:	7:3:2006   0:25
Copyright by Pointer(YongZhang81@gmail.com)

purpose: The execution engine.
*********************************************************************/

#include "Globals.h"
using namespace TargetMachine;

class CRunEngine
{
public:
	CRunEngine();
	void Execute(bool isInDebugMode= false);

private:
	int GetValFromByteCode(int start,bool& bIsAddress,bool &bRelativeEbp,bool &bIsRegister,int &regval);
	
private:
	inline void HandleMove();
	inline void HandleLea();
	inline void HandleArithmeticOp(Operation op);
	inline void HandleCmp();
	inline void HandleLogicOp(Operation op);
	inline void HandleStackOP(Operation op);
	inline void HandleInterrupt();
	inline void HandleCall();
	inline void HandleRet();
	inline void HandleHalt();
};