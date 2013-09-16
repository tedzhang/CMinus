/********************************************************************
created:	2006/02/26
Copyright by Pointer(YongZhang81@gmail.com)

purpose:	Synatx parser implementation.Use LL(1)
*********************************************************************/
#pragma		  once
#include "Globals.h"

TreeNode *parse();
void PrintTree(TreeNode *t);

int GetParseErrors();