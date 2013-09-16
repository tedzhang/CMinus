#include "DsDecl.h"
#include "Globals.h"



CSymbolTable::GLOBALVARTABLE& CSymbolTable::GetGlobalVarTable()
{
	return  m_globalVars;
}
CSymbolTable::PROCEDURETABLE& CSymbolTable::GetProcsTable()
{
	return m_procs;
}
int CSymbolTable::AddGlobalVar(GlobalVar *pGblVar)
{
	ASSERT(pGblVar);
	m_globalVars.push_back(pGblVar);
	return m_globalVars.size()-1;

}
int CSymbolTable::AddProcedure(Procedure *pProc)
{
   ASSERT(pProc != NULL);
   m_procs.push_back(pProc);
   return m_procs.size() -1;
}

//the first key is procedures position and the second is the lable's position
std::pair<int,int>  CSymbolTable::AddLabel(Label *lbl)
{
	ASSERT(m_procs.size() > 0);
	int iLoc   = m_procs.size() -1;
	m_procs[iLoc]->labels.push_back(lbl);
	return std::make_pair(iLoc,m_procs[iLoc]->labels.size()-1);
}



////////////////////////////////////////////////////////////////////////////////
int CHashTable::hashpjw(const char *s)
{
	
		unsigned char * p;
		unsigned h = 0, g;

		for (p = ((unsigned char*)s); *p != '\0'; p = p + 1)
		{
			h = (h << 4) + (*p);
			if (g = (h & 0xf0000000))
			{
				h = h ^ (g >> 24);
				h = h ^ g;
			}
		}

		return h % TABLESIZE;
}

CHashTable::ElementRc* CHashTable::Lookup(const char* str)
{
	int loc = hashpjw(str);
	ItemBucket::iterator iter = m_hashtbl[loc].find(str);

	return iter == m_hashtbl[loc].end()?NULL:(iter->second);

}
void   CHashTable::Insert(const char* str,ElementType type,int iGlobalPos,int iProcPos,int iLabelPos)
{
	ElementRc * rc = new ElementRc;
	rc->textidx = m_strTable.size();
	rc->type    = type;
	switch(type)
	{
	case kGlobalVar:
		{
			rc->Index.GlobalVarPos = iGlobalPos;
		}
		break;
	case kProc:
		{
			//rc->Index.iProcIdx = iProcPos;
			rc->Index.ProcIdx.iProcIdx = iProcPos;
		}
		break;
	case kProcLbl:
		{
           rc->Index.ProcIdx.iProcIdx = iProcPos;
		   rc->Index.ProcIdx.iLabelPos = iLabelPos;
		}
		break;
	}

	int loc = hashpjw(str);
	m_hashtbl[loc].insert(std::make_pair(str,rc));
	m_strTable.push_back(str);
	
}