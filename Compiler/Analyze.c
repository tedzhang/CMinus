/********************************************************************
created:	2006/02/28
Copyright by Pointer(YongZhang81@gmail.com)

purpose:	Type check and semantic check
*********************************************************************/
#include "Analyze.h"
#include "Symtab.h"
#include "SymUtil.h"



typedef void (*FunPtrVisit)(TreeNode*);
static int iTypeErrorCount = 0;

//t is the root node of the function call.it's child is parameter list
static BOOL CheckFunctionCallArgs(TreeNode *t )
{
	TokenType funRetType;
	ASSERT(t);
	{
		TreeNode * p = t->child[0];
		if(p)
		{
			ParamItemList head = parmst_lookup(t->attr.name);
			while(p && head)
			{
				if(p->type != head->type)
				{
					return FALSE;
				}
				p = p->sibling;
				head = head->next;
			}
			if(!p && !head )
				return TRUE;
			else
				return FALSE;

		}
	}

	return TRUE;

}


////////////////////////////////////////////////////////////////////////////
static void Traverse(TreeNode *t,FunPtrVisit preTraverse,FunPtrVisit postTraverse)
{
	int i ;

	if(t)
	{
		if(preTraverse)
			preTraverse(t);
		for(i = 0; i < MAXCHILDREN;++i)
			Traverse(t->child[i],preTraverse,postTraverse);
		if(postTraverse)
			postTraverse(t);
		Traverse(t->sibling,preTraverse,postTraverse);
	}

}

static void TypeError(TreeNode *t ,const char* msg)
{
   ASSERT(t);
   printf("Type error in line : %d %s\n",t->lineno,msg);
   iTypeErrorCount++;
}

int GetSemanticErrors()
{
	return iTypeErrorCount;
}

static void InsertNode(TreeNode *t )
{
	if(t)
	{
		switch(t->nodeKind)
		{
		case kVarDeclaration:
			{
				if(st_lookup(t->attr.name,t->scope,t->bIsArray) == kError)
				{
					st_insert(t->attr.name,t->lineno,t->type,t->scope,t->bIsArray);
					//insert local variable
					if(strcmp(t->scope,GLOBALSCOPE)!= 0)
					 localVar_Insert(t->scope,t->attr.name,t->type,t->bIsArray,t->iArraySize);
				}
				else
				{
					TypeError(t,"Redefine variable");
				}

			}
			break;
		case kFunDeclaration:
			{
				if(st_lookup(t->attr.name,t->scope,t->bIsArray) == kError)
					st_insert(t->attr.name,t->lineno,t->type,t->scope,t->bIsArray);
				else
				{
					TypeError(t,"Redefine function");

				}

			}
			break;
		case kParam:
			{
				if(st_lookup(t->attr.name,t->scope,t->bIsArray) == kError )
				{
					st_insert(t->attr.name,t->lineno,t->type,t->scope,t->bIsArray);
					paramst_Insert(t->scope,t->type,t->attr.name,t->bIsArray,t->iArraySize);
				}
				else
				{
					TypeError(t,"redefine parameter");
				}
			}
			break;
		case kExp:
		{
			switch(t->kind.exp)
			{
			  case kIDExp:
				  {
					  //because type is given in the declaration
					  TokenType type; 
					
					  if((type = st_lookup(t->attr.name,t->scope,t->bIsArray)) != kError )
					  {
						  st_insert(t->attr.name,t->lineno,type,t->scope,t->bIsArray);
					  }
					  else if((type = st_lookup(t->attr.name,GLOBALSCOPE,t->bIsArray)) != kError)
					  {
						  st_insert(t->attr.name,t->lineno,type,t->scope,t->bIsArray);
					  }
					  else if( !t->bIsArray && ((type = st_lookup(t->attr.name,t->scope,TRUE) )!= kError)||
						                        (type = st_lookup(t->attr.name,GLOBALSCOPE,TRUE) != kError)
							 )
					  {
						 t->bIsArray = TRUE;
                         st_insert(t->attr.name,t->lineno,type,t->scope,TRUE);
					  }
					  else
					  {
						  //
						  TypeError(t,"Undefine variable");
					  }
				  }
				  break;
			}
		}
		break;
		case kStmt:
		{
			switch(t->kind.stmt)
			{
			case kCallStmt:
				{
					TokenType type;
					if((type =st_lookup(t->attr.name,GLOBALSCOPE,t->bIsArray)) == kError)
					{
						TypeError(t,"Unresovled symbol");
					}
					else
					  st_insert(t->attr.name,t->lineno,type,t->scope,t->bIsArray);
				}
			}

		}
	
	  }
	}

}

static void CheckNode(TreeNode *t)
{

	if(t)
	{
		switch(t->nodeKind)
		{
		case kExp:
			{
				switch(t->kind.exp)
				{
				case kConstExp:
					{
						t->type = kInt;
					}
					break;
				case kOpExp:
					{
						switch(t->attr.op)
						{
						case kLE:
						case kGE:
						case kLT:
						case kEQ:
						case kGT:
						case kNE:
							if(t->child[0]->type != kInt && t->child[1]->type != kInt)
								TypeError(t,"binary Op applied to non integers");
							else
							    t->type  = kBoolean;
							break;
						case kAssign://special case for "=",because we simplify the production
							{
								if(t->child[0]->kind.exp != kIDExp)
									TypeError(t,"Assign must assign to a left value");
								else if(kInt != t->child[0]->type || kInt != t->child[1]->type)
									TypeError(t,"assign type unmatch");
							
								break;
							}

						default ://arithmetic operation .such as "+","-","*","/"
							if(t->child[0] && t->child[1])
							{
								if(t->child[0]->type != kInt && t->child[1]->type != kInt)
									TypeError(t,"arithmetic op applied to non integers");
								else 
									t->type = kInt;

							}
							else
							{
								TypeError(t,"arithmetic oprand is incorrect");
							}
							
							break;
						}

					}
					break;
				case kIDExp:
					{
						//find its type from the symbol table
                        TokenType type ;
						if((type = st_lookup(t->attr.name,t->scope,t->bIsArray))!= kError)
						{
							t->type = type;
						}
						else if( (type = st_lookup(t->attr.name,GLOBALSCOPE,t->bIsArray)) != kError)
							t->type = type;
						else 
							TypeError(t,"Undefine variable");

					}
					break;
				default:
					{
						TypeError(t,"Unknown expression type");
					}
					break;
				}

			}
			break;
		case kStmt:
			{
				switch(t->kind.stmt)
				{
				case kIfStmt:
					{
						if(t->child[0]->type != kBoolean)
							TypeError(t,"if stmt's condition is not bool");
					}
					break;
				case kWhileStmt:
					{
						if(t->child[0]->type != kBoolean)
							TypeError(t,"while stmt's condition is not bool");

					}
					break;
				case kCallStmt:
					{
						 TokenType type ;
						 if(strcmp(t->attr.name,INPUTFUNTION)== 0)
						 {
							 t->type = kInt;
							 break;//embedded function .have no necessary to do that
						 }
						 else if(strcmp(t->attr.name,OUTPUTFUNCTION) == 0)
						 {
							 t->type = kVoid;
							 break;
						 }
						
						 if((type = st_lookup(t->attr.name,GLOBALSCOPE,t->bIsArray)) != kError)
						 {
							 t->type = type;
						 }
						 else
						 {
							 TypeError(t,"call undefined function");
						 }
						 if(!CheckFunctionCallArgs(t))
								TypeError(t,"function call's args are incorrect");
					
					}
					break;
				case kReturnStmt:
					{
						//do nothing
						//check the return type is as the function type
						//currently only return int is allowed
						if(t->child[0] != NULL && t->child[0]->type != kInt)
						{
							TypeError(t,"function return type is error");
							
						}
						else if(t->child[0] == NULL)
						{
							t->type = kVoid;
						}

					}
					break;
				default:
					TypeError(t,"Unknow type");
					break;
				}

			}
			break;
		}
	}

}




void BuildSymTable(TreeNode * t)
{
   printf("\n-------------------------- Begin symbol table construction------------------\n");
   Traverse(t,InsertNode,NULL);
   //Get only one main function exists in the function scope
   if(st_GetEntryCount(MAINFUNCTION,GLOBALSCOPE,FALSE) != 1)
   {
	   printf("Error:Only one main function should be existed");
   }
}

void TypeCheck(TreeNode *t)
{
	printf("\n-------------------------- Begin type check ------------------\n");
	Traverse(t,NULL,CheckNode);
	printf("type errors :%d errors\n",iTypeErrorCount);
   
}
