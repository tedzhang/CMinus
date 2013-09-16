/********************************************************************
	created:	2006/03/07
	created:	7:3:2006   0:28
	Copyright by Pointer(YongZhang81@gmail.com)
        
	purpose:	Symbol table implementation
*********************************************************************/

#pragma  once 
#include "SymUtil.h"

#define PARAMHASHTABLESIZE 19
#define SHIFT 4

static ParamBucketItem* parmHashTable[PARAMHASHTABLESIZE];

int simplehash(char *name )
{
	int i = 0;
	int iCount = strlen(name);
	int h = 0,p = 0;
	ASSERT(name);
	for(; i < iCount ;++i)
	{
		h = ((h <<SHIFT)+name[i])%PARAMHASHTABLESIZE;
	}
	return h;
}
void localVar_Insert(char* funName,char *varName,TokenType type,BOOL bisArray,int iArraySize)
{
	
	int loc = simplehash(funName);

	ParamBucketItem *pRec = parmHashTable[loc];
	while(pRec && !(strcmp(pRec->funName,funName) == 0))
		pRec = pRec->next;

	if(!pRec )//can not find .this function has no parameters
	{
		pRec = (ParamBucketItem*)malloc(sizeof(ParamBucketItem));
		pRec->funName = funName;
		pRec->paramList = NULL;
		pRec->localVarList = (LocalVarList)malloc(sizeof(struct paramItem));
		pRec->localVarList->bIsArray = bisArray;
		pRec->localVarList->iArraySize = iArraySize;
		pRec->localVarList->type = type;
		if(bisArray)
		{
			//the first array element
			pRec->localVarList->offset = (-4)*iArraySize;
		}
		else
		  pRec->localVarList->offset = -4;//first local variable

		pRec->localVarList->name = varName;
		pRec->localVarList->next = NULL;
		pRec->next = parmHashTable[loc];
		parmHashTable[loc] = pRec;
	}
	else
	{
		LocalVarList q = NULL;
		LocalVarList p =pRec->localVarList;
		while(p && p->next != NULL)
			p = p->next;

		q = (LocalVarList)malloc(sizeof(struct paramItem));
		if(!p)
		{
			pRec->localVarList = q;
			q->offset = -4;

		}
		else
		{
			int iOffSize = 1;
			if(bisArray)
			{
				iOffSize = iArraySize;
			}
			p->next = q;
			//newly added one is an array
			q->offset = p->offset -4 *iOffSize;
		}
		q->bIsArray = bisArray;
		q->iArraySize = iArraySize;
		q->type = type;
		q->name = varName;
		q->next = NULL;
	}
}
LocalVarList localvar_lookup(const char* funName)
{
	int loc = simplehash(funName);
	ParamBucketItem * pItem = parmHashTable[loc];
	while(pItem && strcmp(funName,pItem->funName)!= 0)
		pItem = pItem->next;
	return pItem ? pItem->localVarList : NULL;

}
int local_varlookupOffset(const char* funName,const char* varName)
{
	LocalVarList pItem = localvar_lookup(funName);
	if(pItem)
	{
		//
		while(pItem )
		{
			if(strcmp(pItem->name,varName) == 0)
			{
				return pItem->offset;
			}
			pItem = pItem->next;
		}
		//ASSERT(0);
	}
	//ASSERT(0);
	return -1;

}
 void paramst_Insert(char* name,TokenType type,const char* parName,BOOL bisArray,int iArraySize)
{
	int loc = simplehash(name);

	ParamBucketItem *pRec = parmHashTable[loc];
	while(pRec && !(strcmp(pRec->funName,name) == 0))
		pRec = pRec->next;

	if(!pRec )
	{
		pRec =(ParamBucketItem*)malloc(sizeof(ParamBucketItem));
		pRec->funName = name;
		pRec->localVarList = NULL;
		pRec->paramList = (ParamItemList)malloc(sizeof(struct paramItem));
		pRec->paramList->type = type;
		pRec->paramList->name = parName;
		pRec->paramList->bIsArray = bisArray;
		pRec->paramList->offset = 8;//first is 8
		pRec->paramList->next = NULL;
		pRec->next = parmHashTable[loc];
		parmHashTable[loc] = pRec;

	}
	else
	{
		ParamItemList p =pRec->paramList;
		while(p->next != NULL) 
			p= p->next;
		p->next = (ParamItemList) malloc(sizeof(struct paramItem));
		p->next->type = type;
		p->next->name = parName;
		p->next->bIsArray = bisArray;
		p->next->offset = p->offset +4;
		p->next->next = NULL;
	}
}

int  paramst_lookupParamOffset(char* funname,char* paramname)
{
	ParamItemList pItem = parmst_lookup(funname);
	if(pItem)
	{
		//
		while(pItem )
		{
			if(strcmp(pItem->name,paramname) == 0)
			{
				return pItem->offset;
			}
			pItem = pItem->next;
		}
		
	}
	return -1;

}

ParamItemList parmst_lookup(char* name)
{
	int loc = simplehash(name);
	ParamBucketItem * pItem = parmHashTable[loc];
	while(pItem && strcmp(name,pItem->funName)!= 0)
		pItem = pItem->next;
	return pItem ? pItem->paramList : NULL;
}

void printFunctionTable()
{
	int i = 0;
	for(; i < PARAMHASHTABLESIZE;++i)
	{
		ParamBucketItem * pItem = parmHashTable[i];
		if(pItem)
		{
			ParamItemList p = pItem->paramList;
			LocalVarList  q = pItem->localVarList;
			while(p)
			{
				printf("funname:%s param(offset): %s(%d)\n",pItem->funName,p->name,p->offset);
				p= p->next;
			}
			while(q)
			{
				printf("funname:%s varname(offset): %s(%d)\n",pItem->funName,q->name,q->offset);
				q= q->next;
			}

		}
	}
}