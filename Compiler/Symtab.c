/********************************************************************
created:	2006/03/07
created:	7:3:2006   0:28
Copyright by Pointer(YongZhang81@gmail.com)

purpose:	Symbol table implementation
*********************************************************************/

#include "Symtab.h"

#define TABLESIZE 211

typedef struct lineListRec
{
	int lineno;
	struct lineListRec *next;
}LineListRec;

typedef struct bulkListRec
{
	char *name;
	TokenType type;//if the rec type is global/local variable,it's the data type.If the rec
	                //is for function ,the type is return type
	LineListRec *lines;
	char *scope;
	BOOL isArray;
	int iMemLoc;
	struct bulkListRec *next;

}BulkListRec;

BulkListRec *hashTable[TABLESIZE];

static int g_iMemLoc = 0;


int hashpjw(const char *s)
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
void Init_HashTable()
{
	int i = 0;
	for(; i < TABLESIZE;++i)
		hashTable[i] = NULL;
	//because the 2funtions .input .output is given by us.we should add it manually
	 st_insert(INPUTFUNTION,0,kInt,GLOBALSCOPE,FALSE);
	 st_insert(OUTPUTFUNCTION,0,kVoid,GLOBALSCOPE,FALSE);
}
void st_insert(char *name,int lineno,TokenType type,char* scope,BOOL bIsArray)
{
	int loc = hashpjw(name);
	
	BulkListRec *pRec = hashTable[loc];
	while(pRec && !(strcmp(pRec->name,name) == 0 && strcmp(pRec->scope,scope) == 0 && 
		  pRec->isArray == bIsArray))
		pRec = pRec->next;

	if(!pRec )
	{
		pRec =(BulkListRec*)malloc(sizeof(BulkListRec));
		pRec->name = name;
    	pRec->lines = (LineListRec*)malloc(sizeof(LineListRec));
		pRec->lines->lineno = lineno;
		pRec->lines->next = NULL;
		pRec->type = type;
		pRec->scope = scope;
		pRec->isArray = bIsArray;
		pRec->iMemLoc = g_iMemLoc++;
		pRec->next = hashTable[loc];
		hashTable[loc] = pRec;


	}
	else
	{
		LineListRec *p =pRec->lines;
		while(p->next != NULL) 
			p= p->next;
		p->next = (LineListRec*) malloc(sizeof(LineListRec));
		p->next->lineno = lineno;
		p->next->next = NULL;
	}

}
TokenType st_lookup(const char *name,char* scope,BOOL isArray)
{   
	BulkListRec *pRec = NULL;

	pRec = hashTable[hashpjw(name)];
	while(pRec)
	{
		if((strcmp(pRec->name,name) == 0 && strcmp(pRec->scope,scope) == 0 && 
			pRec->isArray == isArray))
			return pRec->type;
		pRec = pRec->next;
	}
	return kError;//didn't find it yet
}

int st_GetEntryCount(const char *name,char *scope,BOOL isArray)
{
	BulkListRec *pRec = NULL;

	pRec = hashTable[hashpjw(name)];
	while(pRec)
	{
		if((strcmp(pRec->name,name) == 0 && strcmp(pRec->scope,scope) == 0 && 
			pRec->isArray == isArray))
		{
			LineListRec *lstRec = pRec->lines;
			int count = 0;
			while(lstRec)
			{
              count++;
			  lstRec = lstRec->next;
			}
			return count;
		}
		pRec = pRec->next;
	}
	return 0;
}

void PrintSymbTable()
{
	int i = 0;
	BulkListRec *prec = NULL;
	
	printf("\n----name----------scope------line------type---------loc------isArray--\n");
	for ( ; i < TABLESIZE;++i)
	{
		prec = hashTable[i];
		while(prec)
		{
			LineListRec *lines = prec->lines;
			printf("---%s  %s",prec->name,prec->scope);
			while(lines)
			{
				printf(" %d",lines->lineno);
				lines = lines->next;
			}
			printf("   %s",g_token2String[prec->type].str);
			printf("   %d",prec->iMemLoc);
			printf("   %s",prec->isArray ? "TRUE":"FALSE");
			printf("\n");
			prec = prec->next;
		}

	}
	
}

int GetLocation()
{
	return g_iMemLoc-1;
}