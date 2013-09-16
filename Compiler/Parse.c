/********************************************************************
created:	2006/02/26
Copyright by Pointer(YongZhang81@gmail.com)

purpose:	Synatx parser implementation.Use LL(1)
*********************************************************************/

#include "Parse.h"
#include "Scan.h"

TokenType token;
//variable used to ascertatin the tree node's scope
char scope[255];
static int iSynatxErrorCount = 0;

static TreeNode *Declaration_List();
static TreeNode *Declaration();
static TreeNode *Var_Declaration();
static TreeNode *Fun_Declaration();
static TreeNode *Params();
static TreeNode *Param_list();
static TreeNode *Param();
static TreeNode *Compound_Stmt();
static TreeNode *Local_Declaration();
static TreeNode *Stmt_List();
static TreeNode *Stmt();
static TreeNode *Exp_Stmt();
static TreeNode *Sel_Stmt();
static TreeNode *Iteration_Stmt();
static TreeNode *Return_Stmt();
static TreeNode *Exp();
static TreeNode *Var();
static TreeNode *Simple_Exp();
static TreeNode *Addtive_Exp();
static TreeNode *Term();
static TreeNode *Factor();
static TreeNode *Call();
static TreeNode *Args();
static TreeNode *Arg_List();



static void match(TokenType expected)
{
	if(token == expected) token = GetToken();
	else
	{
		printf("Synax Error occured in line %d -> %s\n",lineno,tokenstring);
		iSynatxErrorCount++;
	}
}

static TreeNode* newNode(NodeKind kind)
{
	TreeNode *t = (TreeNode*) malloc(sizeof(TreeNode));
	int i = 0;
	if( t == NULL)
		printf("out of memory");
	else
	{
		for( ; i < MAXCHILDREN;++i)
			t->child[i] = NULL;
		t->sibling = NULL;
		t->nodeKind = kind;
		t->lineno    = lineno;
		t->bIsArray  = FALSE;
		t->iArraySize = -1;
		t->scope = strdup(scope);
		t->bIsAlreadyGenereated = FALSE;
	}
	return t; 
}
//static  TreeNode *newPrgNode()
//{
//	TreeNode *t = (TreeNode*) malloc(sizeof(TreeNode));
//	int i = 0;
//	if( t == NULL)
//		printf("out of memory");
//	else
//	{
//		for( ; i < MAXCHILDREN;++i)
//			t->child[i] = NULL;
//		t->sibling = NULL;
//		t->nodeKind = kPrg;
//		t->lineno    = 0;
//	}
//	return t;
//
//}
//static  TreeNode *newStmtNode(StmtKind kind)
//{
//	TreeNode *t = (TreeNode*) malloc(sizeof(TreeNode));
//	int i = 0;
//	if( t == NULL)
//		printf("out of memory");
//	else
//	{
//		for( ; i < MAXCHILDREN;++i)
//		 t->child[i] = NULL;
//		t->sibling = NULL;
//		t->nodeKind = kStmt;
//		t->kind.stmt = kind;
//		t->lineno    = lineno;
//	}
//	return t;
//
//}
//static  TreeNode *newExpNode(ExpKind kind)
//{
//	TreeNode *t = (TreeNode*) malloc(sizeof(TreeNode));
//	int i = 0;
//	if( t == NULL)
//		printf("out of memory");
//	else
//	{
//		for( ; i < MAXCHILDREN;++i)
//			t->child[i] = NULL;
//		t->sibling = NULL;
//		t->nodeKind = kExp;
//		t->kind.exp = kind;
//		t->lineno    = lineno;
//	}
//	return t;
//
//}
//static  TreeNode *newDeclarationNode(DeclarationKind kind)
//{
//	TreeNode *t = (TreeNode*) malloc(sizeof(TreeNode));
//	int i = 0;
//	if( t == NULL)
//		printf("out of memory");
//	else
//	{
//		for( ; i < MAXCHILDREN;++i)
//			t->child[i] = NULL;
//		t->sibling = NULL;
//		t->nodeKind = kDeclaration;
//		t->kind.dec = kind;
//		t->lineno    = lineno;
//	}
//	return t;
//}




static TreeNode *Declaration_List()
{
	TreeNode * t = NULL;
	TreeNode *p  = NULL;
	TreeNode *r = NULL;
	strcpy(scope,GLOBALSCOPE);
	t= Declaration();
	p = t;
	
	//because declaration = first(typespecifer|compoundstmt)
	while((token != kEndFile) && (token == kInt || token == kVoid || token == kLBParen))
	{
		TreeNode *q = NULL;
		q =  Declaration();
		if(q != NULL)
		{
			if( t == NULL) t = p =q;
			else
		    {
			  while(p->sibling != NULL) p = p->sibling;
			  assert(p && p->sibling == NULL);
			  p->sibling =q;
			  p = q;
		    }

		}
	}
	return t;
}
//The production fun_declaration->typespecifer id(params)|compund_stmt should be
//fun_declaration->typespecifer id(params) compund_stmt ,no "|".The book is incorrect
static  TreeNode *Declaration()
{
	TreeNode * t = NULL;
	if(token == kInt || token == kVoid)
	{
		t = newNode(kVarDeclaration);//temp
		t->type = token;
		match(token);//match type specifier
		//match var declaration
		t->attr.name = strdup(tokenstring);/*(char*)(malloc(strlen(tokenstring)+1));
		strcpy(t->attr.name,tokenstring);*/

		match(kID);
		if( token == kLMParen)//array declaration
		{
			match(token);
			t->bIsArray = TRUE;
			t->iArraySize = atoi(tokenstring);
			match(kNumber);
			match(kRMParen);
			match(kSemicolon);
			return t;
		}
		else if(token == kSemicolon) //variable declaration
		{
			match(kSemicolon);
			return t;
		}//variable
		else if(token == kLParen)
		{
			//now it's functin declaration .we should change its type to function declaration
			t->nodeKind = kFunDeclaration;
			//set its scope
			strcpy(scope,t->attr.name);
			t->scope = strdup(GLOBALSCOPE);//function declaration should be in global declaration
			match(kLParen);
			t->child[0] = Params();
			match(kRParen);
			t->child[1] = Compound_Stmt();
			return t;

		}
		else
		{
			printf("Synatx Error :Unknow declaration");

		}
	
	}
	/*else if(token == kLBParen)
	{
		t = Compound_Stmt();
		return t;
	}*/
	else
	{
		printf("Synatx Error :Unknow declaration");
	}
	return NULL;
}
static TreeNode *Var_Declaration()
{
	TreeNode *t = NULL ;
	if(token == kInt || token == kVoid)
	{
		t= newNode(kVarDeclaration);
		t->type = token;
		match(token);//match type specifier
		//match var declaration
		t->attr.name = strdup(tokenstring);/*(char*)(malloc(strlen(tokenstring)+1));
		strcpy(t->attr.name,tokenstring);*/
		match(kID);
		if(token == kLMParen)//array
		{
			match(kLMParen);
			match(kNumber);
			t->bIsArray = TRUE;
			t->iArraySize = atoi(tokenstring);
			match(kRMParen);
		}
		match(kSemicolon);
	}
	return t;
}
//static TreeNode *Fun_Declaration()
//{
//	
//	TreeNode *t = NULL;
//	if(token == kInt || token == kVoid)//like "int fun(a,b)"
//	{
//		t = newNode(kFunDeclaration);
//		t->type = token;
//		match(token);
//		if(token != kID)
//		{
//			printf("Syntax Error:Function name can not found\n");
//		}
//		else
//		{
//			t->attr.name = (char*)malloc(strlen(tokenstring)+1);
//			strcpy(t->attr.name,tokenstring);
//			strcpy(scope,t->attr.name);//hash the function name
//			strcpy(t->scope,scope);
//			match(kID);
//		}
//		if(token != kLParen)
//		{
//			printf("Syntax Error:\"(\" is missing\n");
//		}
//		else
//		{
//			match(kLParen);
//			t->child[0] = Params();
//		}
//		return t;
//	}
//	else if( token = kLBParen)
//	{
//		t = Compound_Stmt();
//		return t;
//	}
//	return NULL;
//}
//static  TreeNode *Declaration()
//{
//	TreeNode * t = NULL;
//	if(token == kInt || token == kVoid)
//	{
//		int currtoken = token;
//		match(token);//match type specifier
//		//match var declaration
//		t = newDeclarationNode(kVarDeclaration);
//		t->attr.name = (char*)(malloc(strlen(tokenstring)+1));
//		strcpy(t->attr.name,tokenstring);
//		t->declType = currtoken== kInt? kIntDecl: kVoidDecl;
//		match(kID);
//		if( token == kLMParen)
//		{
//			match(token);
//			match(kNumber);
//			t->child[0] = newExpNode(kConstExp);
//			t->child[0]->attr.val = atoi(tokenstring);
//			match(kRMParen);
//
//		}
//		if(token == kSemicolon) 
//		{
//			match(kSemicolon);
//			return t;
//		}
//		t->kind.dec = kFunDeclaration;
//		match(kLParen);
//		t->child[0] = Params();
//		match(kRParen);
//		return t;
//	}
//	t = Compound_Stmt();
//	return t;
//}
static TreeNode *Params()//production 7
{
	TreeNode * t = NULL;
	if(token == kVoid)
	{
		match(token);
		return NULL;
	}
	else
		return Param_list();
}

static TreeNode *Param_list()//production 8
{
	TreeNode *t = Param();
	TreeNode *q = t;
	while(token == kComma)
	{  
		TreeNode *p;
		match(kComma);
		p = Param();
		if(p != NULL)
		{
			if( t == NULL)
				t= p = q;
			else
			{
				q->sibling = p;
				q  = p;
			}
		}
		else
		{
			printf("Synatx error:parameter error\n");
		}
	}
	return t;

}
//Production 9 can be simplified.only int is allowed
static TreeNode *Param()
{
	TreeNode *t = NULL ;

	if(token == kInt )
	{
		t = newNode(kParam);
		t->type = kInt;
        match(token);
		t->attr.name =strdup(tokenstring); /*(char*)(malloc(strlen(tokenstring)+1));
		strcpy(t->attr.name,tokenstring);*/
		match(kID);
		if(token == kLMParen)
		{   
			t->bIsArray = TRUE;
			match(kLMParen);
			match(kRMParen);
		}
	}
	return t;

}
//static TreeNode *Param_list()
//{
//	TreeNode *t = Param();
//	TreeNode *q = t;
//	while(token == kComma)
//	{  
//		TreeNode *p;
//		match(token);
//		p = Param();
//		if(p != NULL)
//		{
//			if( t == NULL)
//			  t= p = q;
//			else
//			{
//				q->sibling = p;
//				q  = p;
//			}
//		}
//		
//	}
//	return t;
//
//}




static TreeNode *Compound_Stmt()
{
	TreeNode *t = NULL ,*p = NULL,*q = NULL;
	if(token == kLBParen)
	{
		match(kLBParen);
		t = Local_Declaration();
        p = Stmt_List();
		//Get the last node who doesn't have sibling
		if(t)
		{
			q = t;
			while(q->sibling != NULL) q= q->sibling;
			q->sibling = p;
		}
		else //no local declaration
			t = p;
		match(kRBParen);
	}
	return t;
}
static TreeNode *Stmt_List()
{
	TreeNode *t  = Stmt();
	TreeNode *p  = t;
	while (p != NULL)
	{
		TreeNode *q  = Stmt();
		p->sibling = q;
		p = q;
	}
	return t;

}
static TreeNode *Local_Declaration()
{
	TreeNode *t = Var_Declaration();
	TreeNode *p = t;
	while(token == kInt || token == kVoid)
	{
		TreeNode *q;
		q = Var_Declaration();
		if(q != NULL)
		{
			if(t == NULL) t = p = q;
			else
			{
				p->sibling =q;
				p = q;
			}
		
		}
		
	}
	return t;
}

static TreeNode *Stmt()
{
	TreeNode *t =NULL;
	switch(token)
	{
	case kIf:
		t = Sel_Stmt();
	    break;
	case kWhile:
		t = Iteration_Stmt();
		break;
	case kReturn:
		t =Return_Stmt();
		break;
	case kLBParen:
		t = Compound_Stmt();
		break;
	case kID:
	case kLParen:
		t = Exp_Stmt();
		break;
	} 
	return t;
}
//production 14 expstmt->exp;|;
static TreeNode *Exp_Stmt()
{
	TreeNode *t = NULL;
	if(token == kSemicolon)
	{
		match(kSemicolon);
		return NULL;
	}
	t = Exp();
	match(kSemicolon);
	return t;

}
//////////////////////////////////////////
//static TreeNode *ExpHelper_Ext()
//{
//  TreeNode *t = NULL;
//  switch(token)
//  {
//    case kAssign:
//		match(kAssign);
//		t = newExpNode(kAssignExp);
//		t->child[0] = Exp();
//		break;
//	case kLE:
//	case kLT:
//	case kEQ:
//	case kGE:
//	case kGT:
//	case kNE:
//		t = newExpNode(kOpExp);
//		t->attr.op = token;
//		match(token);
//		t->child[0] = Simple_Exp();
//		break;
//  }
//  return t;
//}
//static TreeNode *ExpHelper()
//{
//	TreeNode *t = NULL,*p = NULL;
//	switch(token)
//	{
//	case  kAssign:
//		match(kAssign);
//		t = Exp();
//		break;
//	case kLParen:
//		match(kLParen);
//		t = Args();
//		match(kRParen);
//		break;
//	case kLMParen:
//		match(kLMParen);
//		t = Exp();
//		match(kRMParen);
//		p=ExpHelper_Ext();
//		if(p != NULL)
//			t->child[0] = p;//assignment
//		break;
//	default:
//		t = ExpHelper_Ext();
//	}
//	return t;
//}
//simpilify production 18 change from exp->var = expression|simple_expression
//to exp->simple_expression = expression|simple_expression
//leave the work to semantic check
static TreeNode *Exp()
{
   TreeNode *t = NULL;	
   t= Simple_Exp();
   if(t && token == kAssign)
   {
	   TreeNode *p;
	   match(kAssign);
	   p = newNode(kExp);
	   p->kind.exp = kOpExp;
	   p->attr.op = kAssign;
	   p->child[0] = t;//left
	   p->child[1] = Exp();//right
	   t = p;
   }
	 
   return t;
}
static TreeNode *Sel_Stmt()
{
	TreeNode *t = newNode(kStmt);
	t->kind.stmt = kIfStmt;
	match(kIf);
	match(kLParen);
	t ->child[0] =Exp();
	match(kRParen);
	t->child[1] = Stmt();
	if(token== kElse)
	{
		match(kElse);
		t->child[2] = Stmt();
	}
	return t;

}
static TreeNode *Iteration_Stmt()
{
	TreeNode *t = newNode(kStmt);
	t->kind.stmt = kWhileStmt;

	match(kWhile);
	match(kLParen);
	t->child[0] = Exp();
	match(kRParen);
	t->child[1] = Stmt();
	return t;
}
static TreeNode *Return_Stmt()
{
	TreeNode *t = newNode(kStmt);
	t->kind.stmt = kReturnStmt;

	match(kReturn);
    t->child[0] = Exp();
	match(kSemicolon);
	return t;

}

static TreeNode *Var()
{
	TreeNode *t = NULL;
	if(token == kID)
	{
		t = newNode(kExp);
		t->kind.exp = kID;
		t->attr.name =strdup(tokenstring); /*(char*)(malloc(strlen(tokenstring)+1));
		strcpy(t->attr.name,tokenstring);*/
		match(kID);
		if(token == kLMParen)
		{ 
			t->child[0]  = Exp();
			t->bIsArray = TRUE;
		}
	}
	return t;
}
static TreeNode *Simple_Exp()
{
	TreeNode *t = NULL;
	t = Addtive_Exp();
	if(token ==kLT || token == kGT || token == kGE || token == kEQ || token == kLE || token == kNE)
	{
		TreeNode *p = newNode(kExp);
		p->kind.exp = kOpExp;
		p->attr.op = token;
		match(token);
		p->child[0] = t;
		p->child[1] =Addtive_Exp();
		
		t = p;
	}
	return t;
}
static TreeNode *Addtive_Exp()
{
	TreeNode *t  = Term();
	
	while(token == kPlus || token == kMinus)
	{
		TreeNode * q = newNode(kExp);
		q->kind.exp = kOpExp;
		q->attr.op = token;
		
		match(token);
		q->child[0] = t;
		q->child[1]  = Term();
		t = q;
		
	}
	return t;
}
static TreeNode *Term()
{
	TreeNode *t = Factor();
	
	while(token == kMul || token == kDiv )
	{
		TreeNode * q = newNode(kExp);
		q->kind.exp = kOpExp;
		q->attr.op = token;
		match(token);
		
		q->child[0] = t;
	    q->child[1] = Factor();
        t = q;
	}
	return t;
}
#define FUNCTIONCALL 1
#define VAR_ARRAY    2
static TreeNode* FactorHelper(int *ret)
{
  TreeNode *t = NULL;
  *ret = 0;
  if(token == kLMParen) //this is array	
  {
	  match(kLMParen);
	  t = Exp();
	  match(kRMParen);
	  *ret = VAR_ARRAY;
	  return t;
  }
  if(token == kLParen)//This is call
  {
	  match(kLParen);
	  t = Args();
	  match(kRParen);
	  *ret = FUNCTIONCALL;
	  return t;
  }
  return NULL;
}
static TreeNode *Factor()
{
	TreeNode *t =NULL;
	TreeNode *p = NULL;
	int ret ;
	switch(token)
	{
	case kLParen:
		match(kLParen);
		t = Exp();
		match(kRParen);
		return t;

	case kNumber:
		t = newNode(kExp);
		t->kind.exp = kConstExp;
		t->attr.val = atoi(tokenstring);
		match(kNumber);
		return t;
	case kID:
		t =newNode(kExp);
		t->kind.exp = kIDExp;
		t->attr.name = strdup(tokenstring);/*(char*)malloc(strlen(tokenstring)+1);
		strcpy(t->attr.name,tokenstring);*/
		match(kID);
		
		p =FactorHelper(&ret);
		if(ret == FUNCTIONCALL)
		{
			t->nodeKind = kStmt;
			t->kind.stmt = kCallStmt;
			t->child[0]= p;
		}
		else if(ret == VAR_ARRAY)
		{
			t->bIsArray = TRUE;
			t->child[0] = p;
		}
		return t;
	}
	return NULL;
	
}
//Be careful this is a stmt not exp
static TreeNode *Call()
{
  TreeNode *t = newNode(kStmt);
  t->kind.stmt = kCallStmt;
  t->attr.name = strdup(tokenstring);/*(char*)malloc(strlen(tokenstring)+1);
  strcpy(t->attr.name,tokenstring);*/
  match(kID);
  match(kLParen);
  t->child[0] = Args();
  match(kRParen);
  return t;

}
static TreeNode *Args()
{
	return Arg_List();
}
static TreeNode *Arg_List()
{
	TreeNode *t = Exp();
	TreeNode *p = t ;

	while(token == kComma)
	{
		TreeNode *q ;
		match(kComma);
		q = Exp();
		p->sibling = q;
		p = q;
	}
	return t;
}
static TreeNode *Prg()
{
	//TreeNode *t = newNode(kPrg);
	TreeNode *t;
	t =Declaration_List();
	return t;
}

TreeNode* parse()
{
   TreeNode *t ;
   printf("\n----------------Begin synatx parsing ------------------------\n");

   token =GetToken();
   t= Prg();
   if(token !=kEndFile)
 	   printf("Error");
   printf("Synatx error : %d errors\n",iSynatxErrorCount);

   return t;
}
int GetParseErrors()
{
	return iSynatxErrorCount;
}

void PrintTree(TreeNode *t)
{
	int i = 0;
	while(t)
	{
		switch(t->nodeKind)
		{
		 case kStmt:{
            switch(t->kind.stmt){
			case kIfStmt:
				printf("\n if stmt:\n");
				break;
			case kWhileStmt:
				printf("while stmt:\n");
				break;
			case kReturnStmt:
				printf("kReturn stmt:\n");
				break;
			case kCallStmt:
				printf("call function %s \n",t->attr.name);

				break;
			default:
				printf("unknown node:\n" );}		

			        }
			    break;
		case kVarDeclaration:
			if(t->bIsArray)
				printf("VarDecl array %d %s[%d]\n",t->type,t->attr.name,t->iArraySize);
			else
				printf("vardecl var %s\n",t->attr.name);
			break;
		case kFunDeclaration:
			printf("Function declaration: %d %s\n",t->type,t->attr.name);
			break;
		case kParam:
			if(t->bIsArray)
			 printf("param is array %d %s\n",t->type,t->attr.name);
			else 
			 printf("param %d %s \n",t->type,t->attr.name);
			break;
		case kExp:
			{
				switch(t->kind.exp)
				{
				case kIDExp:
					{
						printf("ID: %s\t\t",t->attr.name);
						if(t->bIsArray)
						{
							t = t->child[0];
							printf("[");
							
							PrintTree(t);
								//t = t->sibling;
							printf("]");
							 //printf("[%s]\t",t->attr.name);
						}
					}

				    break;
				case kConstExp:
					printf("const :%d\t\t",t->attr.val);
					break;
				case kOpExp:
					printf("Op :%d \t\t",t->attr.op);
					break;
				default :
					printf("Unknown exp \n");
				}
			}
			break;

		}//end switch
		for(i = 0; i < MAXCHILDREN;++i)
			PrintTree(t->child[i]);
		t =t->sibling;
	}
}