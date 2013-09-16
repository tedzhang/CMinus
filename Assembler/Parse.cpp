#include "Parse.h"
#include "DsDecl.h"

CParser::CParser(CScanner *scanner,CHashTable& hashTable,CSymbolTable &symTable):m_pScanner(scanner),
                                                          m_GlobalOffset(0),m_Address(0),
	    												  m_hashTbl(hashTable),
														  m_symbolTbl(symTable)
{
}
bool CParser::Process()
{
	m_curTok = m_pScanner->GetToken();
	while(m_curTok.type != TOK_ENDFILE)
	{
		switch(m_curTok.type)
		{
		case TOK_OPERATION:
			{
				processInstruction(m_curTok);
			}
			break;
		case TOK_DIRECTIVE:
			{
				processDirective(m_curTok);
			}
			break;
		default:
			{
				//printf("Error happens in line %d\n",m_curTok.lineno);
				m_curTok = m_pScanner->GetToken();
			}
		 
		}
		//m_curTok = m_pScanner->GetToken();
	}
	return true;

}
int CParser::GetErrorNums()
{
	return 0;
}

bool CParser::match(Register expectedRg)
{
	bool bMatched = false;
	if(m_curTok.type == TOK_REG)
	{
		if(m_curTok.Contents.reg == expectedRg)
		{
			m_curTok = m_pScanner->GetToken();
			bMatched = true;
		}
		else
		 bMatched = false;
	}
	if(!bMatched)
	 printf("Synax error in line %d ,Expected reg %s\n",m_curTok.lineno,RegNames[expectedRg].regName);

	return bMatched;
}
bool CParser::match(Directive expdir)
{
	bool bMatched = false;
	if(m_curTok.type == TOK_DIRECTIVE)
	{
		if(m_curTok.Contents.directive == expdir)
		{
			m_curTok = m_pScanner->GetToken();
			bMatched = true;
		}
		else
			bMatched = false;
	}
	if(!bMatched)
		printf("Synax error in line %d ,Expected dir %s\n",m_curTok.lineno,Directives[expdir].name);

	return bMatched;

}
bool CParser::match(TokenType expType)
{
	if(m_curTok.type == expType) {m_curTok = m_pScanner->GetToken();return true;}
	else
		printf("Synax error in line %d ,illegal identifier\n",m_curTok.lineno);
	return false;
}
bool CParser::match(Operation expop)
{
	if(m_curTok.type == TOK_OPERATION)
	{
		if(m_curTok.Contents.op == expop)
		{
			m_curTok = m_pScanner->GetToken();
			return true;
		}

	}
	else
		printf("Synax error in line %d,expect operation %s\n",m_curTok.lineno,OpNames[expop].name);
	return false;
}

void CParser::processDirective(CScanner::Token &tok)
{
	switch(tok.Contents.directive)
	{
	case GDW:
		{
			match(GDW);
			char * str = m_curTok.Contents.toKenstring;
			match(TOK_ID);
			int iLength = 1;
			if(m_curTok.type == TOK_INT_CONST)
			{
				iLength = atoi(m_curTok.Contents.toKenstring);
				match(TOK_INT_CONST);
			}
			match(TOK_NOMORE);

			if(m_hashTbl.Lookup(str)) return;
			GlobalVar *gvar = new GlobalVar;
			gvar->dType = SZ_DWORD;
		    gvar->iArraySize = iLength;
			gvar->lineno = m_curTok.lineno;
			gvar->size = iLength * (int)(SZ_DWORD);
			//We allocate the global variable in heap segment
			gvar->offset = m_GlobalOffset;
			m_GlobalOffset += gvar->size;
			gvar->textidx = m_hashTbl.GetStringTableSize();
			int ipos = m_symbolTbl.AddGlobalVar(gvar);
			m_hashTbl.Insert(str,CHashTable::kGlobalVar,ipos,0,0);
		}
		break;
	case FBEGIN:
		{
			match(FBEGIN);
			char *str = m_curTok.Contents.toKenstring;
			match(TOK_ID);
			match(TOK_NOMORE);

			if(m_hashTbl.Lookup(str)) return;
			Procedure *prc = new Procedure;
			prc->line = m_curTok.lineno;
			prc->address = m_Address;
			prc->textidx = m_hashTbl.GetStringTableSize();
			int iProcPos = m_symbolTbl.AddProcedure(prc);
			m_hashTbl.Insert(str,CHashTable::kProc,0,iProcPos,0);
		}
		break;
	case FEND:
		{
           match(FEND);
		  if(m_curTok.type== TOK_NOMORE ||m_curTok.type == TOK_ENDFILE)
			   match(m_curTok.type);
		}
		break;
	case LABEL:
		{
			match(LABEL);
			char *str = m_curTok.Contents.toKenstring;
			match(TOK_ID);
			match(TOK_NOMORE);
			if(m_hashTbl.Lookup(str)) return;
			Label *lbl = new Label;
			lbl->address = m_Address;
			lbl->line = m_curTok.lineno;
			lbl->textidx = m_hashTbl.GetStringTableSize();
			std::pair<int,int> prc2LblPos = m_symbolTbl.AddLabel(lbl);
			m_hashTbl.Insert(str,CHashTable::kProcLbl,0,prc2LblPos.first,prc2LblPos.second);
		}
		break;
	case PAR:
		{
			//it's useless currently
		};
		break;
	default:
		{
			ASSERT(0);
		}
		break;
	}

}
void CParser::processInstruction(CScanner::Token &tok)
{
	switch(tok.Contents.op)
	{
	case MOV:
		m_Address +=8;
		break;

	case LEA:
		{
			m_Address +=7;
		}
		break;
		
	case ADD:
	case SUB:
	case MUL:
	case DIV:
		{
			m_Address +=7;
		}
		break;
		//logical instruction
	case CMP:
		{
			m_Address +=7;
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
			m_Address +=  5;
		}
		break;
	case INT://interrupt instruction
		{
			//interrput vector's number < 2^8 -1.so 2bytes enough
			m_Address +=3;
		}
		break;
	case PUSH:
		{
			m_Address +=6;
		}
		break;
		//add some limitation here. only can pop to register
	case POP:
		{
			m_Address +=6;
		}
		break;
	case CALL:
		{
			m_Address +=5;
		}
		break;
	case RET:
		{
			m_Address +=1;
		}
		break;
	case HALT:
		{
			m_Address +=1;
		}
		break;
	default:
		{
			ASSERT(0);
		}
		break;
	}
	while(m_curTok.type != TOK_NOMORE)
	  m_curTok = m_pScanner->GetToken();
	match(TOK_NOMORE);

}

