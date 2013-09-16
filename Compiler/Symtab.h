/********************************************************************
created:	2006/03/07
created:	7:3:2006   0:28
Copyright by Pointer(YongZhang81@gmail.com)

purpose:	Symbol table implementation
*********************************************************************/

#pragma  once

#include "Globals.h"

void Init_HashTable();
void st_insert(char *name,int lineno,TokenType type,char *scope,BOOL bIsArray);
TokenType st_lookup(const char *name,char *scope,BOOL isArray);
//for main function we need to check it exists only once
int st_GetEntryCount(const char *name,char *scope,BOOL isArray);
int GetLocation();

void PrintSymbTable();





