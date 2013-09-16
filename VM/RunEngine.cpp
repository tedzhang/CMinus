#include "RunEngine.h"
#include "Loader.h"
/********************************************************************
	created:	2006/03/07
	created:	7:3:2006   0:25
	Copyright by Pointer(YongZhang81@gmail.com)
        
	purpose: The execution engine.
*********************************************************************/

using namespace TargetMachine;

CRunEngine::CRunEngine()
{

}

void CRunEngine::Execute(bool isInDebugMode/* = false */)
{

	while(RAM[Reg[EIP] ] != HALT)
	{

		switch(RAM[Reg[EIP]])
		{
		case MOV: HandleMove();break;
		case LEA: HandleLea(); break;
		case ADD:
		case SUB:
		case MUL:
		case DIV:
			HandleArithmeticOp((Operation)RAM[Reg[EIP]]);
			break;
		case JMP://
		case JGE://>=
		case JLE://<=
		case JGT://>
		case JLT://<
		case JE://==
		case JZ://ZF =1 JMP
		case JNZ:
			HandleLogicOp((Operation)RAM[Reg[EIP]]);
			break;
		case CMP:
			HandleCmp();
			break;
		case  CALL:
			HandleCall();
			break;
		case PUSH:
		case POP:
			{
				HandleStackOP((Operation)RAM[Reg[EIP]]);
			}
			break;
		case RET:
			{
				HandleRet();
			}
			break;
		case INT:
			{
				HandleInterrupt();
			}
			break;
		default:
			{
				ASSERT(0);
			}
			break;

		}
	}
	HandleHalt();
}




void CRunEngine::HandleMove()
{
	bool bIsAddress;bool bIsRegister;int regval;bool brelative2ebp;
	int lval = GetValFromByteCode(1,bIsAddress,brelative2ebp,bIsRegister,regval);
	int rval ;
    if(bIsAddress)
	{
		
		if(brelative2ebp)
		{
		   rval= GetValFromByteCode(4,bIsAddress,brelative2ebp,bIsRegister,regval);
           //assert bIsRegister is true
		   ASSERT(bIsRegister);
		   *(int*)(lval) = Reg[regval];
		}
		else
		{
			rval= GetValFromByteCode(3,bIsAddress,brelative2ebp,bIsRegister,regval);
			//assert bIsRegister is true
			ASSERT(bIsRegister);
			*(int*)(lval) = Reg[regval];
		}

	}
	else
	{
		if(bIsRegister)
		{
			Register lrg = (Register)regval;
			rval = GetValFromByteCode(3,bIsAddress,brelative2ebp,bIsRegister,regval);
			if(bIsAddress)
			{
				rval = *(int*)(rval);
				Reg[lrg] = rval;
			}
			else
			{
				if(bIsRegister)
				{
					Reg[lrg] = Reg[regval];
				}
				else
				{
					Reg[lrg] = rval;
				}
			}
		}
	}

	Reg[EIP] +=8;
}
//such as LEA eax,x or lea eax [EBP+4]
void CRunEngine::HandleLea()
{
	Register rg1 = (Register)RAM[Reg[EIP]+1];
	bool bIsAddress;bool brelative2ebp;bool bisreg;
	int regval;
	int rightval = GetValFromByteCode(2,bIsAddress,brelative2ebp,bisreg,regval);
	ASSERT(rightval != -1);
	if(bIsAddress)
	{
		 Reg[rg1] = rightval;
	}
	else
	{
		if(bisreg)
		{
			Reg[rg1] = Reg[regval];
		}
		else
		{
			Reg[rg1] = rightval;
		}
	}

  
	Reg[EIP] += 7;

}
void CRunEngine::HandleArithmeticOp(Operation op)
{
	Register rg1 = (Register)RAM[Reg[EIP]+1];

	bool bIsAddress;bool brelative2ebp;bool bisreg;
	int regval;
	int rightval = GetValFromByteCode(2,bIsAddress,brelative2ebp,bisreg,regval);
	ASSERT(rightval != -1);
	if(bIsAddress)
	{
		rightval = *(int*) rightval;
	}
	switch(op)
	{
	case ADD:
		Reg[rg1] += rightval;break;
	case MUL:
		Reg[rg1] *= rightval;break;
	case DIV:
		if(rightval == 0)
		{
			printf("Error divide by zero");
		}
		Reg[rg1] /=rightval;break;
	case SUB:
		Reg[rg1] -= rightval;
	}
	Reg[EIP] += 7;

}

int CRunEngine::GetValFromByteCode(int start,bool& bIsAddress,bool &bRelativeEbp,bool &bIsRegister,int &regval)
{

	char ch = RAM[Reg[EIP]+start] ;
	bIsRegister = false;
	bRelativeEbp = false;
	static const int NUMS = CExeLoader::GetTotalBytes();
	//Test the first bit
	if(ch &0x1)//first bit is 1,indicate it's address
	{
		bIsAddress = true;
		if(ch & 0x2)//test the second bit.it's relative to ebp or not
		{
			ASSERT(ch & 0x8);
			bRelativeEbp = true;
			//Get the offset
			int offset  = ::Bytecode2Word(&RAM[Reg[EIP]+start+1]);
			ASSERT((Reg[EBP]+offset) < NUMS);
			int rightval = (int)(&RAM[Reg[EBP]+offset]);
			return rightval;
		}
		else//not relative to ebp
		{
			if(ch & 0x4)//it's a register?
			{
				Register rg2 = (Register)RAM[Reg[EIP]+start+1];
				/*int rightval = (int)(&RAM[Reg[rg2]]);*/
				int rightval = Reg[rg2];
				ASSERT(rightval > 0);
				return rightval;
			}
			else if(ch & 0x8)//immediate and relative .it's forbidden currently
			{
				ASSERT(0);
			}
			else if(ch & 0x10)//it's address
			{
				bool bIsGlobalVar = false;
				if(ch &0x20) bIsGlobalVar = true;
				int address = ::Bytecode2Dword(&RAM[Reg[EIP]+start+1]);
                if(!bIsGlobalVar)
				{
					int rightval = (int)(&RAM[address]);
					return rightval;
				}
				else
				{
                      return (int)(&RAM[MemRegs[HS]+address]);
				}
				
			}
			else
			{
				ASSERT(0);
			}

		}

	}
	else//it's value
	{
		bIsAddress = false;
		if(ch &0x4)//register
		{
			Register reg2 = (Register)(RAM[Reg[EIP]+start+1]);
			bIsRegister = true;
			regval = reg2;
			int rightval = Reg[reg2];
			return rightval;
		}
		else if(ch & 0x8)//immediate value
		{
			int rconst = ::Bytecode2Dword(&RAM[Reg[EIP]+start+1]);
			return rconst;
			
		}
		else if( ch& 0x10)//address
		{
			bool bIsGlobalVar = false;
			if(ch &0x20) bIsGlobalVar = true;
			int address = ::Bytecode2Dword(&RAM[Reg[EIP]+start+1]);
			if(!bIsGlobalVar)
			{
				int righval = Bytecode2Dword(&RAM[address]);
				return righval;

			}
			else
			{
              return (int)(&RAM[MemRegs[HS]+address]);
			}

		
		}
	}
	//Get the address for global variable
	
	return -1;
}
void CRunEngine::HandleCmp()
{

	int result;
	//the first oprand is register
	Register rg;
	rg = (Register)RAM[Reg[EIP]+1];
	bool bIsAddress;
	bool brelative2ebp;bool bisreg;
	int regval;
	int rightval ;
	if((rightval = GetValFromByteCode(2,bIsAddress,brelative2ebp,bisreg,regval) )!= -1)
	{
		if(bIsAddress)
		{
			result = Reg[rg] - *(int*)rightval;
		}
		else
			result = Reg[rg] - rightval;
		
		if(result >0)
		{
			Reg[CF] = 0;
			Reg[ZF] = 0;
		}
		else if(result == 0)
		{
			Reg[CF] = 0;
			Reg[ZF] =1;
		}
		else if(result <0)
		{
			Reg[CF] = 1;
			Reg[ZF] = 0;
		}

	}
	else
	{
		ASSERT(0);
	}
	Reg[EIP] += 7;
   

}
void CRunEngine::HandleLogicOp(Operation op)
{
	bool jmp = false;
	switch(op)
	{
	case JMP://
		{
			Reg[EIP] = Bytecode2Dword(&RAM[Reg[EIP]+1]);
			jmp = true;

		}
		break;
	case JGE://>=
		{
			if(Reg[CF] == 0)
			{
				Reg[EIP] = Bytecode2Dword(&RAM[Reg[EIP]+1]);
				jmp = true;
			}

		}
		break;
	case JLE://<=
		{
			if(Reg[CF] == 1)
			{
				Reg[EIP] = Bytecode2Dword(&RAM[Reg[EIP]+1]);
				jmp = true;
			}
				

		}
		break;
	case JGT://>
		{
			if(Reg[CF] == 0 && Reg[ZF] == 0)
			{
				Reg[EIP] = Bytecode2Dword(&RAM[Reg[EIP]+1]);
				jmp = true;
			}

		}
		break;
	case JLT://<
		{
			if(Reg[CF] == 1 && Reg[ZF] == 0)
			{
				Reg[EIP] = Bytecode2Dword(&RAM[Reg[EIP]+1]);
				jmp = true;
			}
		}
		break;
	case JE://==
	case JZ://ZF =1 JMP
		{
			if(Reg[ZF] == 1)
			{
				Reg[EIP] = Bytecode2Dword(&RAM[Reg[EIP]+1]);
				jmp = true;
			}

		}
		break;
	case JNZ:
		{
			if(Reg[ZF] == 0)
			{
				Reg[EIP] = Bytecode2Dword(&RAM[Reg[EIP]+1]);
				jmp =true;
			}
		}
		break;
	}
    if(!jmp)
	 Reg[EIP] += 5;


}
void CRunEngine::HandleStackOP(Operation op)
{
	switch(op)
	{
	case PUSH:
		{
          	
			//Get the value
            int val;bool bisAddress;
			bool brelative2ebp;bool bisreg;
			int regval;
			val = GetValFromByteCode(1,bisAddress,brelative2ebp,bisreg,regval);
			if(bisAddress)
			{
				val = *(int*)val;
			}
			Reg[ESP] -= 4;
			::DWord2Bytecode(val,&RAM[Reg[ESP]]);
			/**(int*)(&RAM[Reg[ESP]]) = val;*/

		}
		break;
	case POP:
		{
			
			//Get the value
			bool brelative2ebp;bool bisreg;
			int regval;bool bisAddress;
			int val = GetValFromByteCode(1,bisAddress,brelative2ebp,bisreg,regval);
			if(bisAddress)
			{
				val = *(int*)val;
			}
			else
			{
				if(bisreg)
				{
					Reg[regval] = ::Bytecode2Dword(&RAM[Reg[ESP]]);
				}
				else
				{

                   //can not be immediate number .only it's an address value
					*(int*)val = ::Bytecode2Dword(&RAM[Reg[ESP]]);
				}
			}
			/*RAM[val]= *(int*)(&RAM[Reg[ESP]]);*/
			Reg[ESP] += 4;
		}
		break;
	}
	Reg[EIP] += 6;
}
void CRunEngine::HandleInterrupt()
{
	int intNumber = ::Bytecode2Word(&RAM[Reg[EIP]+1]);
	switch(intNumber)
	{
	case 1://printf the value in eax.Only integer is allowed currently
		{
		  printf("Output value: %d\n",Reg[EAX]);

		}
		break;
	case 2:
		{
			printf("Input an integer: \n");
			int val;
			scanf("%d",&val);
			Reg[EAX] = val;
		}
		break;
	default:
		{
			ASSERT(0);
		}
		break;
	}
	Reg[EIP] +=3;
}
void CRunEngine::HandleCall()
{
  int funaddr = Bytecode2Dword(&RAM[Reg[EIP]+1]);
  Reg[EIP] +=5;
  //PUSH IP
  Reg[ESP]  = Reg[ESP] -4;
  *(int*)(&RAM[Reg[ESP]]) = Reg[EIP];
  Reg[EIP]  = funaddr;

}
void CRunEngine::HandleRet()
{
	//pop up eip
	int reteip = *(int*)(&RAM[Reg[ESP]]);
	Reg[ESP] += 4;
	//set the eip to the saved eip
	Reg[EIP] = reteip;
}
void CRunEngine::HandleHalt()
{
	printf("Exit execution..........\n");

}
