/********************************************************************
created:	2006/03/07
created:	7:3:2006   0:28
Copyright by Pointer(YongZhang81@gmail.com)

purpose:	Symbol table implementation
*********************************************************************/

#pragma  once

#include "Globals.h"


typedef struct paramItem
{
	TokenType type;
	const char* name;
	BOOL bIsArray;
	int iArraySize;
	int offset;
	struct paramItem *next;
}*ParamItemList,*LocalVarList;

typedef struct paramBucketItem
{
	char *funName;
	ParamItemList paramList;//for the function paramters
	LocalVarList  localVarList;
	struct paramBucketItem *next;
}ParamBucketItem;

void localVar_Insert(char* funName,char *varName,TokenType type,BOOL bisArray,int iArraySize);
LocalVarList localvar_lookup(const char* funName);
int local_varlookupOffset(const char* funName,const char* varName);

void paramst_Insert(char* name,TokenType type,const char* parName,BOOL bisArray,int iArraySize);
int  paramst_lookupParamOffset(char* funname,char* paramname);
ParamItemList parmst_lookup(char* name);

void printFunctionTable();