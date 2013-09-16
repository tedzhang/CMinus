/********************************************************************
created:	2006/03/07
created:	7:3:2006   0:25
Copyright by Pointer(YongZhang81@gmail.com)

purpose: Check the instruction.
*********************************************************************/
#include "ByteCodeCheck.h"
#include "Globals.h"
bool CByteCodeChecker::CheckRegister(char reg)
{
	if(reg >= TargetMachine::NRG)
	{
		printf("Error in register\n");
		return false;
	}
	return true;

}
bool CByteCodeChecker::CheckAddress(int addr)
{
	if(addr > MemRegs[ST])
	{
		printf("Address is error:\n");
		return false;
	}
	return true;

}