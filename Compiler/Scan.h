/********************************************************************
created:	2006/02/25
Copyright by Pointer(YongZhang81@gmail.com)

purpose:	Lex parser implementation
*********************************************************************/
#pragma	    once

#include "Globals.h"
#define MAXTOKENLENGTH 50

extern char tokenstring[MAXTOKENLENGTH+1];

TokenType GetToken();

int GetLexErrors();


