/********************************************************************
created:	2006/03/03
Copyright by Pointer(YongZhang81@gmail.com)

purpose:	Assembler implementaion. This module is to interpret the
assembly intrustion to bytecode(machine code).
*********************************************************************/


#include "ByteCodeGen.h"


CByteCodeGen::CByteCodeGen(CScanner *pScanner,CSymbolTable& symTbl,
							 CHashTable & hashTbl,bool bGenereatedLsting):
			                 CParser(pScanner,hashTbl,symTbl),m_iCurPos(0),m_nErrors(0)
							/* m_bGenereatedLst(bGenereatedLsting),m_iAddress(0)
							 ,m_fplstFile(NULL)*/
{
	m_curTok.type = TOK_BEGINFILE; 
							
}

////////////////////////////////////////////////////////////////////
//// Add code to genereated lst file
////
/////////////////////////////////////////////////////////////////////
//void CByteCodeGen::SetLstFileName(const char* lstfilename)
//{
//	ASSERT(m_bGenereatedLst);
//	m_fplstFile = fopen(lstfilename,"w+");
//	ASSERT(m_fplstFile);
//	m_iLstPos = 0;
//}
std::pair<const char *,int> CByteCodeGen:: GetByteCodes()
{
	return std::make_pair(bytecodesstr,m_iCurPos);
}


void CByteCodeGen::processDirective(CScanner::Token &tok)
{
	while(m_curTok.type != TOK_NOMORE && m_curTok.type != TOK_ENDFILE)
	{
		m_curTok = m_pScanner->GetToken();
	}
	match(m_curTok.type);
}

void CByteCodeGen::processInstruction(CScanner::Token &tok)
{
	switch(tok.Contents.op)											
	{
	case MOV:
		GenerateByteCodeForMove();
		break;

	case LEA:
		{
			GenerateByteCodeForLea();
		}
		break;

	case ADD:
	case SUB:
	case MUL:
	case DIV:
		{
			GenerateByteCodeForArithmOp(tok.Contents.op);//m_Address +=7;
		}
		break;
		//logical instruction
	case CMP:
		{
			//m_Address +=7;
			buffer[0] = CMP;
			match(CMP);
			
			buffer[1] =m_curTok.Contents.reg;
			match(m_curTok.Contents.reg);
			match(TOK_COMMA);
			if(m_curTok.type == TOK_REG)
			{
				buffer[2] &= 0;buffer[2] |= 0x4;
				buffer[3] = m_curTok.Contents.reg;
				match(m_curTok.Contents.reg);
				match(TOK_NOMORE);
			}
			else if(m_curTok.type == TOK_ID)
			{
				buffer[2] &=0;buffer[2] |= 0x10;
				bool bisglobalvar = false;
				int addr =FindIdentiferAddr(m_curTok.Contents.toKenstring,bisglobalvar);
				if(bisglobalvar)
				{
					buffer[2] |=0x20;
				}
				::DWord2Bytecode(addr,&buffer[3]);
				match(TOK_ID);
				match(TOK_NOMORE);

			}
			else if(m_curTok.type == TOK_INT_CONST)
			{
				buffer[2] &=0; buffer[2] |=0X8;
				int ival = atoi(m_curTok.Contents.toKenstring);
				::DWord2Bytecode(ival,&buffer[3]);
				match(TOK_INT_CONST);
				match(TOK_NOMORE);
			}
			else if(m_curTok.type == TOK_LPAREN)
			{
				bool brelative;
				HandleIndirAddress(2,brelative);
				match(TOK_NOMORE);
			}
			EmitByteCode(buffer,7);
		}
		break;
	case JMP://
	case JGE://>=
	case JLE://<=
	case JGT://>
	case JLT://<
	case JE://==
	case JZ://ZF =1 JMP
	case JNZ:
		{
			//m_Address +=  5;
			GenerateByteCodeForLogicOp(tok.Contents.op);
		}
		break;
	case INT://interrupt instruction
		{
			//interrput vector's number < 2^8 -1.so 2bytes enough
			buffer[0] = INT;
			match(INT);
			int ival = atoi(m_curTok.Contents.toKenstring);
			::Word2Bytecode(ival,&buffer[1]);
			match(TOK_INT_CONST);
			match(TOK_NOMORE);
			EmitByteCode(buffer,3);
			//m_Address +=3;
		}
		break;
	case PUSH:
	case POP:
		{
			GenereateByteCodeForStackOp(tok.Contents.op);
			//m_Address +=5;
		}
		break;
	case CALL:
		{
			//m_Address +=5;
			buffer[0] = CALL;
			match(CALL);
			bool bIsGlobalvar = false;
			int iaddr = FindIdentiferAddr(tok.Contents.toKenstring,bIsGlobalvar);
			::DWord2Bytecode(iaddr,&buffer[1]);
			match(TOK_ID);
			match(TOK_NOMORE);
			EmitByteCode(buffer,5);
		}
		break;
	case RET:
		{
			//m_Address +=1;
			buffer[0] = RET;
			match(RET);
			match(TOK_NOMORE);
			EmitByteCode(buffer,1);
		}
		break;
	case HALT:
		{
			//m_Address +=1;
			buffer[0] = HALT;
			match(HALT);
			match(TOK_NOMORE);
			EmitByteCode(buffer,1);
		}
		break;
	default:
		{
			ASSERT(0);
		}
		break;
	}


}
void CByteCodeGen::GenereateByteCodeForStackOp(Operation op)
{
	match(op);
	buffer[0] = op;
	if(m_curTok.type == TOK_REG)
	{
		
		buffer[1] &=0;buffer[1] |=0X4;
		buffer[2] = m_curTok.Contents.reg;
		ClearBuffer(3,3);
		match(TOK_REG);
	}
	else if(m_curTok.type == TOK_LPAREN)
	{
		bool brelative;
		int bytes =HandleIndirAddress(1,brelative);
		ClearBuffer(1+bytes,5-bytes);
	}
	else if(m_curTok.type == TOK_ID)
	{
		buffer[1] &=0;buffer[1]|=0x10;
		bool bIsGlobalVar = false;
		int ival = FindIdentiferAddr(m_curTok.Contents.toKenstring,bIsGlobalVar);
		if(bIsGlobalVar)
		{
			buffer[1] |= 0x20;
		}
		::DWord2Bytecode(ival,&buffer[2]);
		match(TOK_ID);
	}
	else if(m_curTok.type == TOK_INT_CONST)
	{
		buffer[1] &=0; buffer[1]|=0x8;
		::DWord2Bytecode(atoi(m_curTok.Contents.toKenstring),&buffer[2]);
		match(TOK_INT_CONST);
	}
	match(TOK_NOMORE);
	EmitByteCode(buffer,6);


}

void CByteCodeGen::GenerateByteCodeForMove()
{
	//move instruction is very special
	//for example 1. mov eax,1 2.mov eax,ebx,3,move [eax],ebx 4,move [EBP+/-4],eax
	match(MOV);
	buffer[0] = MOV;
	switch(m_curTok.type)
	{
	case TOK_REG:
		{
			int offset = 0;
			char& ch = buffer[1];
			ch &= 0;//flag
			ch |= 0x4;//oprand is imediate
			buffer[2] = m_curTok.Contents.reg;
			match(TOK_REG);
			match(TOK_COMMA);
			if(m_curTok.type == TOK_INT_CONST)//immediate number,totally 7 bytes
			{
				match(TOK_INT_CONST);
				buffer[3] = 0;
				buffer[3]|= 0x8;//set the bit 3 to 1 indicate it's a immediate
				int ival =atoi(m_curTok.Contents.toKenstring);
				::DWord2Bytecode(ival,&buffer[4]);

				//buffer[8] ='\0';
				offset  = 8;

			}
			else if(m_curTok.type == TOK_REG)//totally 4 bytes
			{
				
				buffer[3] = 0;
				buffer[3] |= 0x4;
				buffer[4] = m_curTok.Contents.reg;
				ClearBuffer(5,3);
				//buffer[5] ='\0';
				match(TOK_REG);
				offset = 8;
			}
			else if(m_curTok.type == TOK_LPAREN)
			{
			  bool brelative2bp;int bytes;
			  bytes = HandleIndirAddress(3,brelative2bp);
			  for(int i = bytes+3;i<8;++i)
				  buffer[i] ^= buffer[i];
			}
			match(TOK_NOMORE);
			EmitByteCode(buffer,8);
			//m_Address += offs/et;
		}
		break;
	case TOK_LPAREN:
		{

			bool breative2Bp;
			int size = HandleIndirAddress(1,breative2Bp);
			match(TOK_COMMA);
			if(m_curTok.type == TOK_REG)
			{
				if(breative2Bp)
				{
					buffer[4] &=0;
					buffer[4] |= 0x4;//oprand is register
					buffer[5]  = m_curTok.Contents.reg;
				}
				else
				{
					buffer[3] &= 0;
					buffer[3] |= 0x4;//oprand is register
					buffer[4] = m_curTok.Contents.reg;
				}
				match(TOK_REG);

			}
			else
			{
				printf("Error in move instruction line %d\n",m_curTok.lineno);
				return;
			}

			EmitByteCode(buffer,8);
			match(TOK_NOMORE);
			//m_Address += offset;
			break;
		}
	default:
		{
			printf("Error happens in line %d",m_curTok.lineno);
		}
		break;

	}

}

//such as LEA eax,x or lea eax [EBP+4]
//8 bytes
void CByteCodeGen::GenerateByteCodeForLea()
{
	match(LEA);
	buffer[0] = LEA;
	//buffer[1] &= 0;
	//buffer[1] |= 0x4;//register
	buffer[1] = m_curTok.Contents.reg;
	match(m_curTok.Contents.reg);
	match(TOK_COMMA);
	//now match the address
	if(m_curTok.type == TOK_REG)
	{
		buffer[2] &= 0;
		buffer[2] |= 0x4;//register
		buffer[3] = m_curTok.Contents.reg;//waste 3 characters
		match(TOK_REG);
	}
	else if(m_curTok.type == TOK_ID)
	{
		//m_hashTbl.
		buffer[2] &=0;
		buffer[2] |=0x10;//address
		bool bIsGlobalVar = false;
		int ival = FindIdentiferAddr(m_curTok.Contents.toKenstring,bIsGlobalVar);
		if(bIsGlobalVar)
		{
			buffer[2] |= 0x20;//global var
		}
		::DWord2Bytecode(ival,&buffer[3]);
		match(TOK_ID);
	}
	else if(m_curTok.type == TOK_LPAREN)
	{
		 bool brelative2ebp ;
         HandleIndirAddress(2,brelative2ebp);
	}
	match(TOK_NOMORE);
	EmitByteCode(buffer,7);
//	m_Address += 8;

}
//+,-,*,/
void CByteCodeGen::GenerateByteCodeForArithmOp(Operation op)
{
	buffer[0] = op;
	match(op);
	buffer[1] = m_curTok.Contents.reg;
	match(m_curTok.Contents.reg);
	match(TOK_COMMA);
	//Now the 2nd oprand can be immediate number or register or address
	if(m_curTok.type == TOK_INT_CONST)
	{
		buffer[2] &= 0;buffer[2] |=0x8;
		int ival = atoi(m_curTok.Contents.toKenstring);
		::DWord2Bytecode(ival,&buffer[3]);
		match(TOK_INT_CONST);
	}
	else if(m_curTok.type == TOK_REG)
	{
		buffer[2] &= 0; buffer[2] |= 0x4;
		buffer[3] = m_curTok.Contents.reg;
		match(m_curTok.Contents.reg);
		ClearBuffer(4,3);
	}
	else if(m_curTok.type == TOK_LPAREN)
	{
		bool bRelative;
		HandleIndirAddress(2,bRelative);
	}
	match(TOK_NOMORE);
	EmitByteCode(buffer,7);
}
//JG,JE and so on
void CByteCodeGen::GenerateByteCodeForLogicOp(Operation op)
{
	// the oprand is the label's address
	buffer[0] = op;
	//buffer[1] &=0;buffer[1] |=0x10;
	match(op);
	bool bIsGlobalVar;
	int ival = FindIdentiferAddr(m_curTok.Contents.toKenstring,bIsGlobalVar);
	ASSERT(!bIsGlobalVar);
	::DWord2Bytecode(ival,&buffer[1]);
	EmitByteCode(buffer,5);
	match(TOK_ID);
	match(TOK_NOMORE);

}


void CByteCodeGen::EmitByteCode(const char* str,int len)
{
	memcpy((void*)(bytecodesstr+m_iCurPos),(void*)str,len);
	m_iCurPos += len;
}

//void CByteCodeGen::EmitLst(const char* str,int len)
//{
//	memcpy((void*)(lstflbuffer+m_iLstPos),(void*)str,len);
//	
//	m_iLstPos += len;
//	lstflbuffer[m_iLstPos] ='\n';
//}

//such as [EAX],[EBX+/-4]
int  CByteCodeGen::HandleIndirAddress(int iStart,bool &bRelative2EBPOffset)
{
	match(TOK_LPAREN);
	int offset = 8;
	buffer[iStart] &= 0;
	buffer[iStart] |= 0x1;//set the first bit 
	bRelative2EBPOffset = false;
	if(m_curTok.Contents.reg != EBP)
	{
		buffer[iStart]|= 0x4;//address and it's oprand is register
		buffer[iStart+1] = m_curTok.Contents.reg;
		match(m_curTok.Contents.reg);
		match(TOK_RPAREN);
		return  2;
	}
	else if(m_curTok.Contents.reg == EBP)//relative to EBP,and the 4,5it's the offset
	{

		match(EBP);
		//offset is stored in 4,5 byte
		if(m_curTok.type == TOK_RPAREN)//totally 6 bytes
		{
			buffer[iStart]|= 0x4;//address and it's oprand is register
			buffer[iStart+1] = m_curTok.Contents.reg;
			match(TOK_RPAREN);
			return 2;
		}
		//5 bytes
		else if(m_curTok.type == TOK_ADD || m_curTok.type == TOK_SUB)
		{
			buffer[iStart] |= 0x2;//set the second bit,the address it's relative to EBP
			buffer[iStart] |= 0x8;//the offset is immediate;only 16bit
			bRelative2EBPOffset = true;
			if(m_curTok.type == TOK_ADD)
			{
				match(TOK_ADD);
				int ival = atoi(m_curTok.Contents.toKenstring);
				::Word2Bytecode(ival,&buffer[iStart+1]);
				match(TOK_INT_CONST);
				match(TOK_RPAREN);
			}
			else 
			{
				match(TOK_SUB);
				int ival =(-1)*atoi(m_curTok.Contents.toKenstring);
				::Word2Bytecode(ival,&buffer[iStart+1]);
				match(TOK_INT_CONST);
				match(TOK_RPAREN);
			}
			return 3;

		}
	}
	else
	{
		printf("Error not indirect address\n");
	}
	return -1;
}
int CByteCodeGen::FindIdentiferAddr(const char* name,bool& bisGlobalVar)
{
	CHashTable::ElementRc *rc;
	bisGlobalVar = false;
	if((rc = m_hashTbl.Lookup(name)) != NULL)
	{
		switch(rc->type)
		{
		case CHashTable::kGlobalVar:
			{
				GlobalVar * pGbl = m_symbolTbl.GetGlobalVarTable().at(rc->Index.GlobalVarPos);
				ASSERT(pGbl != NULL);
				bisGlobalVar = true;
				return pGbl->offset;
			}
			break;
		case CHashTable::kProc:
			{
              Procedure *prc = m_symbolTbl.GetProcsTable().at(rc->Index.ProcIdx.iProcIdx);
			  ASSERT(prc != NULL);
			  return prc->address;
			}
			break;
		case CHashTable::kProcLbl:
			{
				Procedure *prc = m_symbolTbl.GetProcsTable().at(rc->Index.ProcIdx.iProcIdx);
				ASSERT(prc != NULL);
				Label *lbl = prc->labels.at(rc->Index.ProcIdx.iLabelPos);
				ASSERT(lbl);
				return lbl->address;
			}
			break;
		default:
			{
				ASSERT(0);
			}
		}
	}
	else
	{
		m_nErrors++;
		printf("Undefine symbol %s \n",name);
	}
	ASSERT(0);
	return -1;
}

inline void CByteCodeGen::ClearBuffer(int iStart,int size)
{
	for(int i = iStart;i< iStart+size;++i)
		buffer[i] ^= buffer[i];
}