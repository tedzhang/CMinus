/********************************************************************
created:	2006/02/26
Copyright by Pointer(YongZhang81@gmail.com)

purpose:	Code generation implementation.Generate P code(stack based cpu)
            and  assembly(register based cpu).Use some feature of Intel
            assembly
*********************************************************************/
#include "CGen.h"
#include "Symtab.h"
#include "Code.h"
#include "SymUtil.h"

#define MAXCODESIZE 1024*4
#define MAXBUFFER 255
static char pcodestr[MAXCODESIZE+1];
static char asmcodestr[MAXCODESIZE+1];
static int iCodePos = 0;
static char buffer[MAXBUFFER+1];

static BOOL bGenPCodeOnly = FALSE;
static BOOL bOutput2Console = FALSE;

void SetGenPcodeOnly()
{
	bGenPCodeOnly = TRUE;
}

void SetOutput2Console()
{
	bOutput2Console = TRUE;
}



///////////////////////////////////////////////////////////////////////////////////////////////
/////// p Code generated routines
///////
///////////////////////////////////////////////////////////////////////////////////////////////


static void EmitCode(const char * str)
{
	int len = strlen(str);
	memcpy((void*)(pcodestr+iCodePos),(void*)str,len);
	
	iCodePos += (1+len);
	pcodestr[iCodePos -1] ='\n';
}


//////////////////////////////////////////////////////////////////////////////
// Traverse the tree,then emit codes
//
//////////////////////////////////////////////////////////////////////////////
/// generate p code
//////////////////////////////////////////////////////////////////////////////
typedef enum 
{
	PENTER,
	PRET,
	LDC ,
	LDA,
	IXA,
	LOD,
	ADI,
	SBI,
	MPI,
	DVI,
	GRT,
	LESS,
	LEQ,
	GEQ,
	NEQ,
	EQ,
	STN,
	FJP,
	UJP,
	MST,
	CUP,
	PLABEL,
}PCodeOp;
typedef struct pCodeInstruction PInstruction;
struct pCodeInstruction
{
	PCodeOp op;
	union
	{
		int val;
		const char* name;
	}Contents;
	PInstruction *next;
	PInstruction *prev;
};
typedef struct pCodeBlock PCodeBlock; 
struct pCodeBlock//each funtion is a block
{
	char *funName;
	PInstruction * pInstruction;
	BOOL bIsConverted2Asm;
	PCodeBlock *next;//next block
};
PCodeBlock *currentBlock = NULL;
PCodeBlock *pCodeBlockHeader  = NULL;
#define MAKE_NEWVALPINST(OP,VAL) \
{\
   PInstruction *q = NULL;\
   PInstruction *p = (PInstruction *)malloc(sizeof(PInstruction));\
   p->op =  OP;\
   p->Contents.val = VAL;\
   p->next         = NULL;\
   q = currentBlock->pInstruction;\
   while(q&& q->next != NULL)\
       q = q->next;\
   if(q) {q->next = p;p->prev =q;}\
   else  {currentBlock->pInstruction = p;p->prev =NULL;}\
}\



#define MAKE_NEWNAMEPINST(OP,NAME)\
{\
   PInstruction *q = NULL;\
   PInstruction *p = (PInstruction *)malloc(sizeof(PInstruction));\
   p->op = OP;\
   p->Contents.name = NAME;\
   p->next         = NULL;\
   q = currentBlock->pInstruction;\
   while(q && q->next != NULL)\
       q = q->next;\
   if(q) {q->next = p;p->prev = q;}\
   else  {currentBlock->pInstruction = p;p->prev = NULL;}\
}\

#define MAKE_NEWPBLOCK(FUNNAME)\
{\
	static BOOL bFirstTime = TRUE;\
	PCodeBlock * p = (PCodeBlock *)malloc(sizeof(PCodeBlock));\
	p->funName = FUNNAME;\
	p->bIsConverted2Asm = FALSE;\
	p->next = NULL;\
	p->pInstruction=NULL;\
	currentBlock = p;\
	if(bFirstTime)\
	{\
		pCodeBlockHeader = currentBlock;\
		bFirstTime = FALSE;\
	}\
	else{\
	       PCodeBlock *q = pCodeBlockHeader;\
	       while(q && q->next) q= q->next;\
	       q->next = p;\
	    }\
}\



typedef struct globalVar
{
	TokenType  type;//data type
	const char* name;
	BOOL bIsArray;
	int iArraySize;
	struct globalVar *pnext;
}GlobalVar;

static GlobalVar * pGlobalVars;

static void ADD_NEWGLOBALVAR(TokenType type,const char* globalvarname,BOOL bIsArray,int iArraySize) 
{
 GlobalVar *pnew = (GlobalVar*)malloc(sizeof(GlobalVar));
 pnew->type = (type);
 pnew->name = (globalvarname);
 pnew->bIsArray = (bIsArray);
 pnew->iArraySize = (iArraySize);
 pnew->pnext      = NULL;
 {
    GlobalVar *q = pGlobalVars;
    while(q && q->pnext ) q = q->pnext;
	if(!q) 
		pGlobalVars =pnew;
	else
	  q->pnext = pnew;
 }
}



////////////////////////////////////////////////////////////////////////////////////
// P code generation
////////////////////////////////////////////////////////////////////////////////////
static void PCGen(TreeNode *t,BOOL bIsAddr,char *label);
//Gen
#define LISARRAY 0x1
#define RISARRAY 0x2

static void PGenExp(TreeNode *t,BOOL bIsAddr,char *label)
{
	if(t)
	{
      switch(t->kind.exp)
	  {
	    case kConstExp:
			{
				sprintf(buffer,"ldc %d",t->attr.val);
				EmitCode(buffer);
				MAKE_NEWVALPINST(LDC,t->attr.val);
				break;
			}
			break;
		case kIDExp:
			{
				int offset ;
				//int iVariableKind = 0;
				if((offset = local_varlookupOffset(t->scope,t->attr.name))!= -1)
				{
					////It's a localvariable
					////load id value of variable
					

				}
				else if((offset = paramst_lookupParamOffset(t->scope,t->attr.name))!= -1)
				{
					////it's a parameter
					//do nothing
				
				}
				else //it's a global variable
				{
				}

				if(t->bIsArray)
				{
					sprintf(buffer,"lda %s",t->attr.name);
					EmitCode(buffer);
					MAKE_NEWNAMEPINST(LDA,t->attr.name);
                     
					if(t->child[0])//like this a[i]= ...
					{
						PCGen(t->child[0],FALSE,label);
						sprintf(buffer,"ixa element_size(%s)",t->attr.name);
						EmitCode(buffer);
						MAKE_NEWNAMEPINST(IXA,t->attr.name);
					}
					//else//add a flag here to indicate that the array has no offset.Maybe it's a parameter
					//{
					//	MAKE_NEWNAMEPINST(IXA,t->attr.name);

					//}
					

				}
				else
				{
					if(bIsAddr)
					{
						sprintf(buffer,"lda %s",t->attr.name);
						MAKE_NEWNAMEPINST(LDA,t->attr.name);

					}
					else
					{
						sprintf(buffer,"lod %s",t->attr.name);
						MAKE_NEWNAMEPINST(LOD,t->attr.name);
					}
					EmitCode(buffer);

				}
			}
			break;
		case kOpExp:
			{

				if(t->attr.op != kAssign)
				{

					if(bIsAddr)
						ASSERT(0);
					else
					{   
						char * s= NULL,flag=NULL;
						PCodeOp op ;
						PCGen(t->child[0],FALSE,label);
						PCGen(t->child[1],FALSE,label);
						switch(t->attr.op)
						{
							case kPlus:
								s ="adi",op = ADI;break;
							case kMinus:
								s = "sbi",op = SBI;break;
							case  kMul:
								s ="mpi",op=MPI;break;
							case kDiv:
								s = "dvi",op =DVI;break;
							case kGT:
								s ="grt",op = GRT; break;
							case kLT:
								s = "less",op =LESS;break;
							case kLE:
								s ="leq",op = LEQ;break;
							case kGE:
								s ="geq",op =GEQ;break;
							case kNE:
								s = "neq",op =NEQ;break;
							case kEQ:
								s = "eq",op = EQ;break;
								
						   default :break;
						}
						sprintf(buffer,"%s",s);
						EmitCode(buffer);
					
						if(t->child[0]->bIsArray) flag |= LISARRAY;
						if(t->child[1]->bIsArray) flag |= RISARRAY;
						MAKE_NEWNAMEPINST(op,flag);//no name
						

					}
				}
				else
				{
					PCGen(t->child[0],TRUE,label);
					PCGen(t->child[1],FALSE,label);
					sprintf(buffer,"%s","stn");
					EmitCode(buffer);
					MAKE_NEWNAMEPINST(STN,NULL);

				}


			}//end case kop
	   }//end switch
	}//end if
}


static char* GetLabel()
{
	static int i = 0;
	sprintf(buffer,"L%d",i++);
	return strdup(buffer);
}
static void PGenStmt(TreeNode *t,BOOL bIsAddr,char *label)
{
   int i = 0;
   char *lab1= NULL,*lab2=NULL;
   if(t)
   {
	   switch(t->kind.stmt)
	   {
	   case  kIfStmt:
		   {
			   PCGen(t->child[0],bIsAddr,lab1);
			   lab1 = GetLabel();
			   sprintf(buffer,"fjp %s",lab1);//if false then goto l1
			   EmitCode(buffer);
			   MAKE_NEWNAMEPINST(FJP,lab1);
			   PCGen(t->child[1],bIsAddr,lab1);
			   if(t->child[2] != NULL)
			   {
				   lab2 = GetLabel();
				   sprintf(buffer,"ujp %s",lab2);
                   EmitCode(buffer);
				   MAKE_NEWNAMEPINST(UJP,lab2);
			   }
			   sprintf(buffer,"lab %s",lab1);
			   EmitCode(buffer);
			   MAKE_NEWNAMEPINST(PLABEL,lab1);
			   if(t->child[2])
			   {
				   PCGen(t->child[2],bIsAddr,lab2);
				   sprintf(buffer,"lab %s",lab2);
				   EmitCode(buffer);
				   MAKE_NEWNAMEPINST(PLABEL,lab2);
			   }
			  

		   }
		   break;
	   case kWhileStmt:
		   {
			   lab1 = GetLabel();
			   sprintf(buffer,"lab %s",lab1);
			   EmitCode(buffer);
			   MAKE_NEWNAMEPINST(PLABEL,lab1);

			   PCGen(t->child[0],bIsAddr,lab1);
			   lab2 = GetLabel();
			   sprintf(buffer,"fjp %s",lab2);
			   EmitCode(buffer);
			   MAKE_NEWNAMEPINST(FJP,lab2);

			   PCGen(t->child[1],bIsAddr,lab2);
			   sprintf(buffer,"ujp %s",lab1);
			   EmitCode(buffer);
			   MAKE_NEWNAMEPINST(UJP,lab1);

			   sprintf(buffer,"lab %s",lab2);
			   EmitCode(buffer);
			   MAKE_NEWNAMEPINST(PLABEL,lab2);

		   }
		   break;
	   case kReturnStmt:
		   {
             if(t->child[0]!= NULL)
				 PCGen(t->child[0],bIsAddr,lab1);
		   }
		   break;
	   case kCallStmt:
		   {
              //push the parameter
			   //call the function name
			  TreeNode *p =t->child[0];
			  sprintf(buffer,"%s","mst");
			  EmitCode(buffer);
			  MAKE_NEWNAMEPINST(MST,NULL);
			 
			  //Generate code for args.the  push sequence is different in p code 
			  //and assembly code
              if(bGenPCodeOnly == FALSE)
			  {
                 TreeNode* pArgs[255];
				 int iSize = 0;
				 int j = 0;
				 while(p)
				 {
					 pArgs[iSize++] =p;
					 p = p->sibling;
				 }
				 for(j = iSize-1;j >=0;j--)
				 {
					 PCGen(pArgs[j],bIsAddr,label);
					 pArgs[j]->bIsAlreadyGenereated = TRUE;
				 }
			  }
            else//Gen p code args push sequence
			    PCGen(p,bIsAddr,label);
			
			  sprintf(buffer,"cup %s",t->attr.name);
			  EmitCode(buffer);
			  MAKE_NEWNAMEPINST(CUP,t->attr.name);
		   }
		   break;
	   }
	   
   }
   
}

static void PGenVarDeclaration(TreeNode *t,BOOL bIsAddr,char *label)
{
	//do nothing in p code
	if(strcmp(t->scope,GLOBALSCOPE)==0 )
	{
		ADD_NEWGLOBALVAR(t->type,t->attr.name,t->bIsArray,t->iArraySize);

	}

}

static void PGenFunDeclaration(TreeNode *t,BOOL bIsAddr,char *label)
{
     
    sprintf(buffer,"ent %s",t->attr.name);
	EmitCode(buffer);
	//store it in the list
	MAKE_NEWPBLOCK(t->attr.name);
	MAKE_NEWNAMEPINST(PENTER,t->attr.name);

	PCGen(t->child[1],bIsAddr,label);
	//if(t->type == kVoid)
	sprintf(buffer,"%s","ret");
	EmitCode(buffer);
	MAKE_NEWNAMEPINST(PRET,NULL);
	
}

static void PCGen(TreeNode *t,BOOL bIsAddr,char *label)
{
	if(t &&  !t->bIsAlreadyGenereated )//the 2nd condition only useful for args
	{
		switch(t->nodeKind)
		{
		case kExp:
			PGenExp(t,bIsAddr,label);
			break;
		case kStmt:
			PGenStmt(t,bIsAddr,label);
			break;
		case kVarDeclaration:
			PGenVarDeclaration(t,bIsAddr,label);
			break;
		case kFunDeclaration:
			PGenFunDeclaration(t,bIsAddr,label);
			break;
		default:
			break;
		}
		PCGen(t->sibling,bIsAddr,label);
	}
}


void PCodeGen(TreeNode *synatxTree,FILE *pcodeFile,BOOL output2File)
{
	PCGen(synatxTree,FALSE,NULL);
	pcodestr[iCodePos] ='\0';
	if(output2File)
	  printf("\n---------------generated p-codes----------------------------\n");
	if(bOutput2Console)
	  printf("%s",pcodestr);
	if(output2File)
	 fputs(pcodestr,pcodeFile);
 

}
/////////////////////////////////////////////////////////////////////////////////////////////////
///Convert the pcode to assembly code
//////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////
//For testing the register selection algorithm .It's really difficult
//hope me can finish it later.
//////////////////////////////////////////////////////////
//typedef enum { Empty,IntConst,Address,Value}OprandKind;
//typedef struct st_record
//{
//	OprandKind kind;
//	union { int val;char* name;
//	}contents;
//}ST_RC;
//
//#define STACKSIZE 255
//static ST_RC STACK[STACKSIZE+1];
//static int top = STACKSIZE+1;
//void  ST_PUSH(ST_RC rc)
//{
//	--top;
//	STACK[top].kind = rc.kind;
//	if(rc.kind == IntConst)
//	{
//		STACK[top].contents.val = rc.contents.val;
//	}
//	else if(rc.kind != Empty)
//	{
//		STACK[top].contents.name = rc.contents.name;
//	}
//}
//ST_RC ST_POP()
//{
//	return STACK[top++];
//}
//static int iTempCount =0;
//static char* Make_Temp(char *funName)
//{
//	sprintf(buffer,"_%s_@%d@%s",funName,iTempCount++,funName);
//	st_insert(funName,0,kInt,funName,FALSE);
//	return strdup(buffer);
//}
////////////////////////////////////////////////////////////////////////////////////////////////
//
//
////////////////////////////////////////////////////////////////////////////////////////////////
///// Register selection algorithm
/////
///////////////////////////////////////////////////////////////////////////////////////////////
//static  struct 
//{
//	Register reg;
//	BOOL bIsUsed;
//	int  iMemLoc;//which memory position is currently in reg
//}RegisterUseRec[] =
//{
//	{ EAX,FALSE,-1},
//	{ EBX,FALSE,-1},
//	{ ECX,FALSE,-1},
//	{ EDX,FALSE,-1},
//};
//Register GetRegToUse()
//{
//	int i = 0;
//	for(;i < sizeof(RegisterUseRec)/sizeof(RegisterUseRec[0]) ;++i)
//	{
//		if(!RegisterUseRec[i].bIsUsed)
//			return RegisterUseRec[i].reg;
//	}
//	return NRG;
//}
//void InitReg()
//{
//	int i = 0;
//	for(;i < sizeof(RegisterUseRec)/sizeof(RegisterUseRec[0]) ;++i)
//	{
//		RegisterUseRec[i].bIsUsed = FALSE;
//	}
//
//}
//void PUSHREGS()
//{
//	int i = 0;
//	for(;i < sizeof(RegisterUseRec)/sizeof(RegisterUseRec[0]) ;++i)
//	{
//		sprintf(buffer,"PUSH %s",RegNames[i].regName);
//		EmitAsmCode(buffer);
//	}
//
//}
//void POPREGS()
//{
//	int i = 0;
//	for(;i < sizeof(RegisterUseRec)/sizeof(RegisterUseRec[0]) ;++i)
//	{
//		sprintf(buffer,"POP %s",RegNames[i].regName);
//		EmitAsmCode(buffer);
//	}
//
//}
//BOOL IsIdleReg(Register rg)
//{
//	return !RegisterUseRec[rg].bIsUsed;
//}
//Register GetReg(const char* name)
//{
//	int i = 0;
//	for(;i < 4;++i)
//	{
//		if(strcmp(RegNames[i].regName,name) == 0)
//			return (Register)i;
//	}
//	return NRG;
//}
////////////////////////////////////////////////////////////////////////////////////////////////////
//Convert p-code to assembly code
//void ConvertPCode2AssemblyCode()
//{
//	PCodeBlock *p = pCodeBlockHeader;
//	while(p)
//	{
//		PInstruction *pInst = p->pInstruction;
//		char         *funName = p->funName;
//		while(pInst)
//		{
//			ST_RC rc;
//			switch(pInst->op)
//			{
//			case PENTER:
//				{
//					sprintf(buffer,"%s %s;function enter",Directives[FBEGIN].name,funName);
//					EmitAsmCode(buffer);
//					//PUSHREGS();
//				}
//				break;
//			case PRET:
//				{
//                   // POPREGS();
//					ST_RC rc1;
//					if(st_lookup(funName,GLOBALSCOPE,FALSE) != kVoid)
//					{
//						rc1 = ST_POP();
//						if(rc1.kind != IntConst)
//						{
//							sprintf(buffer,"MOV EAX,%s",rc1.contents.name);
//						}
//						else
//						{
//							sprintf(buffer,"MOV EAX,%d",rc1.contents.val);
//						}
//						EmitAsmCode(buffer);
//					}
//					sprintf(buffer,"%s;function ret",Directives[FEND].name);
//					EmitAsmCode(buffer);
//				}
//				break;
//			case LDC :
//				{
//					rc.kind =IntConst;
//					rc.contents.val = pInst->Contents.val;
//					ST_PUSH(rc);
//				}
//				break;
//			case LDA:
//				{
//					rc.kind =Address;
//					rc.contents.name= pInst->Contents.name;
//					ST_PUSH(rc);
//				}
//				break;
//		
//			case IXA:
//				{
//					int iElementSize = pInst->Contents.val;//should be 4
//					ST_RC rc1,rc2,rc3;
//					Register rg;
//					int offset =0;
//					rc1 = ST_POP();
//					if(rc1.kind != Address)
//					 rc2 = ST_POP();
//					if(rc1.kind == IntConst)
//					{
//						offset = rc1.contents.val *iElementSize;
//						if((rg = GetReg(rc2.contents.name) )!= NRG)
//						{
//							sprintf(buffer,"%s %s,%d",OpNames[ADD].name,RegNames[rg].regName,offset);
//							EmitAsmCode(buffer);
//						}
//						else
//						{
//							BOOL bPop = FALSE;
//							rg = GetRegToUse();
//							if(rg == NRG)
//							{
//								sprintf(buffer,"%s %s",OpNames[PUSH].name,RegNames[EAX].reg);
//								EmitAsmCode(buffer);
//								bPop = TRUE;
//								rg = EAX;
//								
//							
//							}
//							sprintf(buffer,"%s %s,offset %s",OpNames[MOV].name,RegNames[rg].regName,rc2.contents.name);
//							EmitAsmCode(buffer);
//
//							sprintf(buffer,"%s %s,%d",OpNames[ADD].name,RegNames[rg].regName,offset);
//							EmitAsmCode(buffer);
//
//						
//							if(bPop)
//							{
//								rc3.contents.name =Make_Temp(funName);
//								sprintf(buffer,"MOV %s,EAX",rc3.contents.name);
//								EmitAsmCode(buffer);
//								
//								sprintf(buffer,"%s %s","POP ","EAX");
//							    EmitAsmCode(buffer);
//								rg = NRG;
//							}
//						}
//					}
//					else if(rc1.kind == Value)
//					{
//						if((rg = GetReg(rc1.contents.name)) != NRG)
//						{
//							Register rg2;
//							sprintf(buffer,"MUL %s,%d",RegNames[rg].regName,iElementSize);
//							EmitAsmCode(buffer);
//							if((rg2 = GetReg(rc2.contents.name)) != NRG )
//							{
//								RegisterUseRec[rg2].bIsUsed = FALSE;
//							}
//							sprintf(buffer,"ADD %s,offset %s",RegNames[rg].reg,rc2.contents.name);
//							EmitAsmCode(buffer);
//						}
//						else
//						{
//							Register rg2;
//							if((rg2 = GetReg(rc2.contents.name)) != NRG )
//							{
//								Register rg3;
//								if((rg3 = GetRegToUse())!= NRG)
//								{
//									sprintf(buffer,"MOV %s,%s",RegNames[rg3].regName,rc1.contents.name);
//									EmitAsmCode(buffer);
//									sprintf(buffer,"MUL %s,%d",RegNames[rg3].regName,iElementSize);
//									EmitAsmCode(buffer);
//									sprintf(buffer,"ADD %s,%s",RegNames[rg2].regName,RegNames[rg3].regName);
//									EmitAsmCode(buffer);
//								}
//								else
//								{
//									sprintf(buffer,"PUSH EAX");
//									EmitAsmCode(buffer);
//									sprintf(buffer,"MOV %s,%s",RegNames[EAX].regName,rc1.contents.name);
//									EmitAsmCode(buffer);
//									sprintf(buffer,"MUL %s,%d",RegNames[EAX].regName,iElementSize);
//									EmitAsmCode(buffer);
//									sprintf(buffer,"ADD %s,EAX",RegNames[rg2].regName);
//									EmitAsmCode(buffer);
//									sprintf(buffer,"POP EAX");
//									EmitAsmCode(buffer);
//								}
//								rg = rg2;
//							}//rc1 and rc2 are both not in the register
//							else
//							{
//								Register rg3;
//								if((rg3 = GetRegToUse())!= NRG)
//								{
//									sprintf(buffer,"MOV %s,%s",RegNames[rg3].regName,rc1.contents.name);
//									EmitAsmCode(buffer);
//									sprintf(buffer,"MUL %s,%d",RegNames[rg3].regName,iElementSize);
//									EmitAsmCode(buffer);
//									sprintf(buffer,"ADD %s,offset %s",RegNames[rg3].regName,rc2.contents.name);
//									EmitAsmCode(buffer);
//									rg = rg3;
//								}
//								else
//								{
//									sprintf(buffer,"PUSH EAX");
//									EmitAsmCode(buffer);
//									sprintf(buffer,"MOV %s,%s",RegNames[EAX].regName,rc1.contents.name);
//									EmitAsmCode(buffer);
//									sprintf(buffer,"MUL %s,%d",RegNames[EAX].regName,iElementSize);
//									EmitAsmCode(buffer);
//									sprintf(buffer,"ADD %s,offset %s",RegNames[EAX].regName,rc2.contents.name);
//									EmitAsmCode(buffer);
//									rc3.contents.name =Make_Temp(funName);
//									sprintf(buffer,"MOV %s,EAX",rc3.contents.name);
//									EmitAsmCode(buffer);
//									sprintf(buffer,"POP EAX");
//									EmitAsmCode(buffer);
//									rg = NRG;
//									
//
//								}
//
//
//							}
//
//						}
//						
//					}
//					else if(rc1.kind == Address)//must be a function call
//					{
//						if((rg =GetReg(rc1.contents.name))!= NRG)
//						{
//							//do nothing
//							
//						}
//						else
//						{
//							rg = GetRegToUse();
//							if(rg == NRG)
//							{
//								sprintf(buffer,"PUSH EAX");
//								EmitAsmCode(buffer);
//								sprintf(buffer,"MOV EAX,offset %s",rc1.contents.name);
//								EmitAsmCode(buffer);
//								rc3.contents.name =Make_Temp(funName);
//								sprintf(buffer,"MOV %s,EAX",rc3.contents.name);
//								EmitAsmCode(buffer);
//								sprintf(buffer,"POP EAX");
//								EmitAsmCode(buffer);
//								rg = NRG;
//							}
//							else
//							{
//								sprintf(buffer,"MOV %s,offset %s",RegNames[rg].regName,rc1.contents.name);
//								EmitAsmCode(buffer);
//
//							}
//
//						}
//					
//					}
//					else
//					{
//						ASSERT(0);
//					}
//
//					rc3.kind = Address;
//					if(rg != NRG)
//					{
//						rc3.contents.name =RegNames[rg].regName;
//					     RegisterUseRec[rg].bIsUsed = TRUE;
//					}
//					ST_PUSH(rc3);
//				}
//				break;
//			case LOD :
//				{
//					rc.kind = Value;
//					rc.contents.name = pInst->Contents.name;
//					ST_PUSH(rc);
//				}
//				break;
//			case ADI:
//			case SBI:
//			case DVI:
//			case MPI:
//				{
//					ST_RC rc1,rc2,rc3;
//					rc1 = ST_POP();
//					rc2 = ST_POP();
//					if(rc1.kind == IntConst && rc2.kind == IntConst)
//					{
//						rc3.kind = IntConst;
//						switch(pInst->op)
//					    {
//						case ADI:
//						  rc3.contents.val = rc1.contents.val+rc2.contents.val;
//						  break;
//						case SBI:
//							rc3.contents.val = rc1.contents.val-rc2.contents.val;
//							break;
//						case DVI:
//							rc3.contents.val = rc1.contents.val/rc2.contents.val;
//							break;
//						case MPI:
//							rc3.contents.val = rc1.contents.val*rc2.contents.val;
//							break;
//						}
//					}
//					else 
//					{
//
//						BOOL r1 = rc1.kind == Value;
//						BOOL r2 = rc2.kind == Value;
//						Register rg;
//						Operation op;
//						switch(pInst->op)
//						{
//						case ADI:
//							op = ADD;
//							break;
//						case SBI:
//							op = SUB;
//							break;
//						case MPI:
//							op = MUL;
//							break;
//						case DIV:
//							op = DIV;
//							break;
//						}
//
//						rc3.kind = Value;
//						if(rc1.kind == Value || rc2.kind == Value)
//						{
//							Register rg1= NRG,rg2= NRG;
//							if(rc1.kind == Value && (rg1=GetReg(rc1.contents.name) )!= NRG)
//							{
//								rc3.contents.name = RegNames[rg1].regName;
//							}
//							else if(rc2.kind == Value && ((rg2 = GetReg(rc2.contents.name) )!= NRG))
//							{
//								rc3.contents.name = RegNames[rg2].regName;
//							}
//							if(rg1 != NRG || rg2 != NRG)
//							{
//								if(rg1 == NRG)
//								{
//								  if(rc1.kind == Value)
//								   sprintf(buffer,"%s %s,%s",OpNames[op].name,RegNames[rg2].regName,rc1.contents.name);
//								  else
//								   sprintf(buffer,"%s %s,%d",OpNames[op].name,RegNames[rg2].regName,rc1.contents.val);
//								  EmitAsmCode(buffer);
//
//								}
//								else if(rg2 == NRG)
//								{
//									if(rc1.kind == Value)
//										sprintf(buffer,"%s %s,%s",OpNames[op].name,RegNames[rg1].regName,rc2.contents.name);
//									else
//										sprintf(buffer,"%s %s,%d",OpNames[op].name,RegNames[rg1].regName,rc2.contents.val);
//									EmitAsmCode(buffer);
//								}
//								else
//								{
//									sprintf(buffer,"%s %s,%s"),OpNames[op].name,RegNames[rg1].regName,
//										                       RegNames[rg2].regName;
//									EmitAsmCode(buffer);
//								}
//								ST_PUSH(rc3);
//								break;
//							}
//						}
//
//					    if((rg =GetRegToUse())!= NRG)
//							rc3.contents.name = RegNames[rg].regName;
//						else
//						{
//							rc3.contents.name = "EAX";
//						}
//						if(rg == NRG)
//						{
//							sprintf(buffer,"%s %s",OpNames[PUSH].name,RegNames[EAX].regName);
//							EmitAsmCode(buffer);
//						}
//						sprintf(buffer,"MOV %s %s",rc3.contents.name,r1?rc1.contents.name: rc2.contents.name);
//						EmitAsmCode(buffer);
//                       
//							
//						if(rc1.kind == IntConst ||rc2.kind == IntConst)
//						{
//							sprintf(buffer,"%s %s,%d",OpNames[op].name,rc3.contents.name,rc1.kind == IntConst?rc1.contents.val:rc2.contents.val);
//							EmitAsmCode(buffer);
//						}
//						else
//						{
//							sprintf(buffer,"%s %s, %s",OpNames[op].name,rc3.contents.name,r1?rc2.contents.name:rc1.contents.name);
//							EmitAsmCode(buffer);
//						}
//						
//						if(rg == NRG)
//						{
//							rc3.contents.name = Make_Temp(funName);
//							sprintf(buffer,"MOV %s,%s",rc3.contents.name,"EAX");
//							EmitAsmCode(buffer);
//							sprintf(buffer,"%s %s",OpNames[POP].name,RegNames[EAX].regName);
//							EmitAsmCode(buffer);
//						}
//						else
//						{
//							RegisterUseRec[rg].bIsUsed = TRUE;
//						}
//						ST_PUSH(rc3);
//					}
//				}
//				break;
//			case GRT:
//			case LESS:
//			case LEQ:
//			case GEQ:
//			case NEQ:
//			case EQ:
//				{
//				
//				}
//				break;
//			case STN:
//				{
//					ST_RC rcRight,rcLeft;
//					Register rg;
//					rcRight = ST_POP();
//					rcLeft  = ST_POP();
//					ASSERT(rcLeft.kind == Address);
//					if((rg = GetReg(rcLeft.contents.name) )!= EAX && rg != NRG)
//					{
//						if(RegisterUseRec[EAX].bIsUsed)
//						{
//							sprintf(buffer,"PUSH EAX");
//							EmitAsmCode(buffer);
//						}
//						sprintf(buffer,"MOV EAX,%s",RegNames[rg].regName);
//						EmitAsmCode(buffer);
//					}
//					if(rcRight.kind != IntConst)
//					  sprintf(buffer,"MOV EAX,%s",rcRight.contents.name);
//					else
//						sprintf(buffer,"MOV EAX,%d",rcRight.contents.val);
//					EmitAsmCode(buffer);
//					sprintf(buffer,"MOV %s,EAX",rcLeft.contents.name);
//					EmitAsmCode(buffer);
//
//				}
//				break;
//			case FJP:
//				{
//					ST_RC r1,r2;
//					Register  rg;
//					int icase = -1;
//					r1 =ST_POP();//2nd oprand
//					r2 = ST_POP();//1st oprand
//					ASSERT(pInst->prev != NULL);
//					if(r2.kind == Value )
//					{
//						if((rg = GetReg(r2.contents.name) != NRG))
//						{
//							if(r1.kind == Value)
//								sprintf(buffer,"%s %s,%s",OpNames[CMP].name,RegNames[rg].regName,r1.contents.name);
//							else if(r1.kind == Address)
//							{
//								//To do .handle address
//								//sprintf(buffer,"%s %s,%d")
//								ASSERT(0);
//							}
//							else if(r1.kind == IntConst)
//							{
//								sprintf(buffer,"%s %s,%s",OpNames[CMP].name,RegNames[rg].regName,r2.contents.name);
//
//							}
//							EmitAsmCode(buffer);
//							icase = 1;
//
//						}
//						else
//						{
//							rg = GetRegToUse();
//							if(rg != NRG)
//							{
//								sprintf(buffer,"MOV %s,%s",RegNames[rg].regName,r2.contents.name);
//								EmitAsmCode(buffer);
//								sprintf(buffer,"CMP %s,%s",RegNames[rg].regName,r2.contents.name);
//								EmitAsmCode(buffer);
//								icase = 2;
//
//							}
//							else
//							{
//								//sprintf(buffer,"PUSH EAX",RegNames)
//                               
//
//							}
//						}
//					
//						
//						//break;
//					}
//					//To do: consider common case 
//					if(icase != 1 && icase != 2)
//					{
//						sprintf(buffer,"%s%s","PUSH EAX");
//						EmitAsmCode(buffer);
//						sprintf(buffer,"MOV EAX %s",r2.contents.name);
//						EmitAsmCode(buffer);
//						if(r1.kind != IntConst)
//						 sprintf(buffer,"CMP EAX, %s",r1.contents.name);
//						else
//							sprintf(buffer,"CMP EAX,%d",r1.contents.val);
//						EmitAsmCode(buffer);
//
//					}
//
//					if(icase != 1 && icase != 2)
//					{
//						sprintf(buffer,"%s","POP EAX");
//						EmitAsmCode(buffer);
//					}
//				
//
//					switch(pInst->prev->op)
//					{
//					case GRT:
//						{
//							sprintf(buffer,"%s %s",OpNames[JLE].name,pInst->Contents.name);
//						}
//						break;
//					case LESS:
//						{
//							sprintf(buffer,"%s %s",OpNames[JGE].name,pInst->Contents.name);
//						}
//						break;
//					case LEQ:
//						sprintf(buffer,"%s %s",OpNames[JGT].name,pInst->Contents.name);
//						break;
//					case GEQ:
//						sprintf(buffer,"%s %s",OpNames[JLT].name,pInst->Contents.name);
//						break;
//					case NEQ:
//						sprintf(buffer,"%s %s",OpNames[JE].name,pInst->Contents.name);
//						break;
//					case EQ:
//						sprintf(buffer,"%s %s",OpNames[JNZ].name,pInst->Contents.name);
//						break;
//					}
//					EmitAsmCode(buffer);
//
//				}
//				break;
//		
//			case UJP:
//				{
//					sprintf(buffer,"%s %s",OpNames[JMP].name,pInst->Contents.name);
//					EmitAsmCode(buffer);
//				}
//				break;
//			case MST:
//				{
//					//donothing
//					
//				}
//				break;
//			case CUP:
//				{
//					
//					TokenType retType ;
//					ParamItemList prlst = parmst_lookup(pInst->Contents.name);
//					retType=st_lookup(pInst->Contents.name,GLOBALSCOPE,FALSE);
//
//
//					if(retType != kVoid)
//					{
//						while(prlst)
//						{
//							ST_RC rc = ST_POP();
//							int offset = -1;
//							if(rc.kind != IntConst)
//							{
//								if((offset = local_varlookupOffset(funName,rc.contents.name))!= -1)
//								{
//									////It's a localvariable
//									////load id value of variable
//
//
//								}
//								else if((offset = paramst_lookupParamOffset(funName,rc.contents.name))!= -1)
//								{
//									////it's a parameter
//									//do nothing
//
//								}
//								else //it's a global variable
//								{
//								}
//							}
//							switch(rc.kind)
//							{
//							case Value:
//							case Address:
//								{
//									if(offset != -1)
//									{
//										Register rg;
//										if((rg = GetReg(rc.contents.name)) != NRG)
//										{
//											sprintf(buffer,"%s %s",OpNames[PUSH].name,RegNames[rg].regName);
//											EmitAsmCode(buffer);
//										}
//										else
//										{
//											sprintf(buffer,"%s [EBP%s%d]",OpNames[PUSH].name,offset>0?"+":"",offset);
//											EmitAsmCode(buffer);
//										}
//										
//									}
//									else
//									{
//										//global variable
//										//Pass the variable's address.
//										sprintf(buffer,"%s offset%s",OpNames[PUSH].name,rc.contents.name);
//										EmitAsmCode(buffer);
//									}
//									
//								}
//								break;
//							case IntConst:
//								{
//									sprintf(buffer,"%s %d",OpNames[PUSH].name,rc.contents.val);
//									EmitAsmCode(buffer);
//								}
//								break;
//							}
//							prlst= prlst->next;
//						}
//
//					}
//					sprintf(buffer,"%s %s",OpNames[CALL].name,pInst->Contents.name);
//					EmitAsmCode(buffer);
//					if(retType != kVoid)
//					{
//						ST_RC rc3;
//						rc3.kind = Value;
//						rc3.contents.name = "EAX";
//						ST_PUSH(rc3);
//					}
//				}
//				break;
//			case PLABEL:
//				{
//					sprintf(buffer,"%s %s;label",Directives[LABEL].name,pInst->Contents.name);
//					EmitAsmCode(buffer);
//				}
//				break;
//
//			}
//			pInst = pInst->next;
//		}
//		p = p->next;
//
//	}
//
//}
//
static int iAsmCodePos = 0;
static void EmitAsmCode(const char * str)
{
	int len = strlen(str);
	memcpy((void*)(asmcodestr+iAsmCodePos),(void*)str,len);
	iAsmCodePos += (1+len);
	asmcodestr[iAsmCodePos-1] ='\n';

}
static void  PUSHASM(const char* funname,PInstruction *p) 
{
	if(p->op == LDC)
	 sprintf(buffer,"PUSH %d",p->Contents.val);
	else
	{
		int offset ;
		//int iVariableKind = 0;
		if((offset = local_varlookupOffset(funname,p->Contents.name))!= -1)
		{
			////It's a localvariable
			////load id value of variable
			ASSERT(offset <0);
			sprintf(buffer,"PUSH [EBP%d]",offset);

		}
		else if((offset = paramst_lookupParamOffset(funname,p->Contents.name))!= -1)
		{
			////it's a parameter
			//do nothing
			sprintf(buffer,"PUSH [EBP+%d]",offset);
		}
		else //it's a global variable
		{
			sprintf(buffer,"PUSH %s",p->Contents.name);
		}

	}
	EmitAsmCode(buffer);
}
static void PUSHREG(Register rg)
{
	sprintf(buffer,"PUSH %s",RegNames[rg].regName);
	EmitAsmCode(buffer);

}

static void  POPASM(Register rg) 
{
	sprintf(buffer,"POP %s",RegNames[rg].regName);
	EmitAsmCode(buffer);
}
static void OPASM(PInstruction *pInst,Operation op)
{
	BOOL lopisarray = FALSE,ropisarray = FALSE;
	//be careful for p code .the first oprand is not the one right on the stack top
	//it's the 2nd one next to the stack top
	POPASM(ECX);
	POPASM(EAX);
	

	//If the oprand is array. we need to judge them value not address.
	//if(((int)(pInst->Contents.name)) & LISARRAY)
	//	lopisarray = TRUE;
	//if(((int)(pInst->Contents.name)) & RISARRAY)
	//	ropisarray = TRUE;

	//if(lopisarray && ropisarray)
	//{
	//	sprintf(buffer,"MOV EDX,[EAX]");
	//	EmitAsmCode(buffer);
	//	sprintf(buffer,"%s EDX,[ECX]",OpNames[op].name);
	//	EmitAsmCode(buffer);
	//	PUSHREG(EDX);
	//}
	//else if(lopisarray)
	//{
	//	sprintf(buffer,"MOV EDX,[EAX]");
	//	EmitAsmCode(buffer);
	//	sprintf(buffer,"%s EDX,ECX",OpNames[op].name);
	//	EmitAsmCode(buffer);
	//	PUSHREG(EDX);
	//}
	//else if(ropisarray)
	//{
	//	sprintf(buffer,"%s EAX,[ECX]",OpNames[op].name);
	//	EmitAsmCode(buffer);
	//	PUSHREG(EAX);
	//}
	//else
	{
		sprintf(buffer,"%s EAX,ECX",OpNames[op].name);
		EmitAsmCode(buffer);
		PUSHREG(EAX);
	}

	
}

static void ConvertPCodeBlock2AssemblyCode(PCodeBlock *p,BOOL bIsMainFunction)
{
	PInstruction *pInst = p->pInstruction;
	char         *funName = p->funName;
	BOOL bIsInFunctionCall = FALSE;
	while(pInst)
	{
		
		switch(pInst->op)
		{
		case PENTER:
			{
				sprintf(buffer,"%s %s;function enter",Directives[FBEGIN].name,funName);
				EmitAsmCode(buffer);
				sprintf(buffer,"PUSH EBP");
				EmitAsmCode(buffer);
				sprintf(buffer,"MOV EBP,ESP;Set up stack frame");
				EmitAsmCode(buffer);
				//Get all local variables and assign memory for them
				{
					int nLocalsOffset = 0;
					LocalVarList lcvars = localvar_lookup(funName);
					while(lcvars)
					{
						if(lcvars->bIsArray)
						{
							nLocalsOffset +=  lcvars->iArraySize *4;
						}
						else
						{
							nLocalsOffset +=4;
						}
						lcvars = lcvars->next;
					}
					if(nLocalsOffset != 0)
					{
						sprintf(buffer,"SUB ESP,%d",nLocalsOffset);
						EmitAsmCode(buffer);
					}
				}
				//PUSHREGS();
			}
			break;
		case PRET:
			{
				// POPREGS();
				if(st_lookup(funName,GLOBALSCOPE,FALSE) != kVoid)
				{
					POPASM(EAX);
				}
				if(!bIsMainFunction)
				{
					sprintf(buffer,"MOV ESP,EBP;Recover the stack frame");
					EmitAsmCode(buffer);
					sprintf(buffer,"POP EBP");
					EmitAsmCode(buffer);
                    sprintf(buffer,"RET");
					EmitAsmCode(buffer);
					
				}
				else
				{
					sprintf(buffer,"HALT");
					EmitAsmCode(buffer);
					
				}
				sprintf(buffer,"%s;function ret",Directives[FEND].name);
				EmitAsmCode(buffer);
			
			}
			break;
		case LDC :
			{
				PUSHASM(funName,pInst);

			}
			break;
		case LDA:
			{
				int offset =0;
				if((offset = local_varlookupOffset(funName,pInst->Contents.name))!= -1)
				{
					////It's a localvariable
					////load id value of variable
					ASSERT(offset <0);
					sprintf(buffer,"LEA EBX,[EBP%d]",offset);

				}
				else if((offset = paramst_lookupParamOffset(funName,pInst->Contents.name))!= -1)
				{
					BOOL bIsPassedInParameterArray = FALSE;
					ParamItemList parLst =parmst_lookup(funName);

					while(parLst)
					{
                       if(strcmp(pInst->Contents.name,parLst->name) == 0)
					   {
						   if(parLst->bIsArray)bIsPassedInParameterArray =TRUE;
						   break;
					   }
					}
					if(bIsPassedInParameterArray == TRUE)
					{
						sprintf(buffer,"MOV EBX,[EBP+%d]",offset);

					}
					else//do nothing
					  sprintf(buffer,"LEA EBX, [EBP+%d]",offset);
				}
				else //it's a global variable
				{
					sprintf(buffer,"LEA EBX, %s",pInst->Contents.name);
				}
				EmitAsmCode(buffer);
				PUSHREG(EBX);
			}
			break;

		case IXA:
			{
				//need to find in symbol table. then get its size
				int iElementSize = 4;//should be 4
				//if it's function call
				if(pInst->prev->op == LDA)//
				{
					//do nothing
				}
				else
				{
					POPASM(ECX);
					sprintf(buffer,"MUL ECX,%d",iElementSize);
					EmitAsmCode(buffer);
					POPASM(EAX);
					sprintf(buffer,"ADD EAX,ECX");
					EmitAsmCode(buffer);
					if(bIsInFunctionCall == FALSE)
					  PUSHREG(EAX);
					else
					{
						//Check it's a local or global variable
						//if it's a local variable we have no necessary to do that
						//
						/*BOOL bIsGlobalVar = FALSE;
						GlobalVar *q  = pGlobalVars;
						while(q)
						{
							if(strcmp(q->name,pInst->Contents.name) == 0 && q->bIsArray == TRUE)
							{
                              bIsGlobalVar = TRUE;
							}
							q = q->pnext;
						}
						if(bIsGlobalVar == TRUE)
						{*/
							//need to convert the address to value
							sprintf(buffer,"MOV EDX,[EAX]");
							EmitAsmCode(buffer);
							PUSHREG(EDX);

					/*	}
						else
							 PUSHREG(EAX);*/

					}

				}

			}
			break;
		case LOD :
			{
				PUSHASM(funName,pInst);
			}
			break;
		case ADI:
		case SBI:
		case DVI:
		case MPI:
			{


				Operation op;
				switch(pInst->op)
				{
				case ADI:
					op = ADD;
					break;
				case SBI:
					op = SUB;
					break;
				case MPI:
					op = MUL;
					break;
				case DVI:// P code is dvi
					op = DIV;
					break;
				}
				OPASM(pInst,op);
			}
			break;
		case GRT:
		case LESS:
		case LEQ:
		case GEQ:
		case NEQ:
		case EQ:
			{

			}
			break;
		case STN:
			{
				BOOL bIsArray = FALSE;
				if(pInst->prev->op == IXA)//array as rvalue.we need to get its value
				{
                    bIsArray = TRUE;
				}
				POPASM(ECX);
				POPASM(EAX);
				if(!bIsArray)
				{
					sprintf(buffer,"MOV [EAX],%s",RegNames[ECX].regName);
					EmitAsmCode(buffer);
				}
				else
				{
					sprintf(buffer,"MOV EDX,[ECX]");
					EmitAsmCode(buffer);
					sprintf(buffer,"MOV [EAX],EDX");
					EmitAsmCode(buffer);
				}
				

			}
			break;
		case FJP:
			{
				BOOL lopisarray = FALSE,ropisarray = FALSE;

				//If the oprand is array. we need to judge them value not address.
				if(((int)(pInst->prev->Contents.name)) & LISARRAY)
					lopisarray = TRUE;
				if(((int)(pInst->prev->Contents.name)) & RISARRAY)
					ropisarray = TRUE;
				

				POPASM(ECX);
				POPASM(EAX);
				if(lopisarray && ropisarray)
				{
					sprintf(buffer,"MOV EDX,[EAX]");
					EmitAsmCode(buffer);
					sprintf(buffer,"CMP EDX,[ECX]");
				}
				else if(lopisarray)
				{
				  sprintf(buffer,"MOV EDX,[EAX]");
				  EmitAsmCode(buffer);
                  sprintf(buffer,"CMP EDX,ECX");
				}
				else if(ropisarray)
				{
                    sprintf(buffer,"CMP EAX,[ECX]");
				}
				else
				{
						sprintf(buffer,"CMP EAX,ECX");
				}
			
				EmitAsmCode(buffer);
				switch(pInst->prev->op)
				{
				case GRT:
					{
						sprintf(buffer,"%s %s",OpNames[JLE].name,pInst->Contents.name);
					}
					break;
				case LESS:
					{
						sprintf(buffer,"%s %s",OpNames[JGE].name,pInst->Contents.name);
					}
					break;
				case LEQ:
					sprintf(buffer,"%s %s",OpNames[JGT].name,pInst->Contents.name);
					break;
				case GEQ:
					sprintf(buffer,"%s %s",OpNames[JLT].name,pInst->Contents.name);
					break;
				case NEQ:
					sprintf(buffer,"%s %s",OpNames[JE].name,pInst->Contents.name);
					break;
				case EQ:
					sprintf(buffer,"%s %s",OpNames[JNZ].name,pInst->Contents.name);
					break;
				}
				EmitAsmCode(buffer);

			}
			break;

		case UJP:
			{
				sprintf(buffer,"%s %s",OpNames[JMP].name,pInst->Contents.name);
				EmitAsmCode(buffer);
			}
			break;
		case MST:
			{
				//donothing
				bIsInFunctionCall = TRUE;

			}
			break;
		case CUP:
			{

				int iParmCount = 0;
				ParamItemList prlst = parmst_lookup(pInst->Contents.name);
				while(prlst)
				{
					iParmCount++;
					prlst= prlst->next;
				}
				sprintf(buffer,"%s %s",OpNames[CALL].name,pInst->Contents.name);
				EmitAsmCode(buffer);
				if(iParmCount >0)
				{
					sprintf(buffer,"ADD ESP,%d",iParmCount *4);
					EmitAsmCode(buffer);
				}
				else if(iParmCount == 0 && strcmp(OUTPUTFUNCTION,pInst->Contents.name) == 0)//for output we
					//need to adjust the stack top manually
				{
					sprintf(buffer,"ADD ESP,%d",4);
					EmitAsmCode(buffer);
				}
				//If it has retval.it is in eax.
				//push it
				if(st_lookup(pInst->Contents.name,GLOBALSCOPE,FALSE) != kVoid)
				{
					PUSHREG(EAX);
				}
				bIsInFunctionCall = FALSE;
			}
			break;
		case PLABEL:
			{
				sprintf(buffer,"%s %s;label",Directives[LABEL].name,pInst->Contents.name);
				EmitAsmCode(buffer);
			}
			break;
		default:
			{
				break;
			}

		}
		pInst = pInst->next;
	}		

}
static void ConvertPCode2AssemblyCode()
{

	PCodeBlock *p = pCodeBlockHeader;
	GlobalVar *q  = pGlobalVars;

     //Generate code for all global variables
	while(q)
	{
		//currently we only have  int type support
        if(q->bIsArray)
		{
			sprintf(buffer,"%s %s %d",Types[GDW].name,q->name,q->iArraySize);

		}
		else
		{
			sprintf(buffer,"%s %s",Types[GDW].name,q->name);
		}
		EmitAsmCode(buffer);
		q = q->pnext;
	}

	//first generate routine for main
	while(p && strcmp(p->funName,MAINFUNCTION) != 0)
	{
        p = p->next;
	}
	//first generate codes for main
	//some other issue should be generated before the main function
	//such as global variable and so on
	//To do
	//convert code for main
	{
		ConvertPCodeBlock2AssemblyCode(p,TRUE);
		p->bIsConverted2Asm = TRUE;
	}
	//convert other functions
	p = pCodeBlockHeader;
	while(p)
	{
		if(p->bIsConverted2Asm){ p = p->next; continue;}
		ConvertPCodeBlock2AssemblyCode(p,FALSE);
		p->bIsConverted2Asm = TRUE;
		p = p->next;
	}
	//add input output function into the assembly code
	{
		//input function
		sprintf(buffer,".PBEGIN input\nPUSH EBP\nMOV EBP,ESP\nINT 2\nMOV ESP,EBP\nPOP EBP\nRET\n.PEND\n");
		EmitAsmCode(buffer);
		//output function
		sprintf(buffer,".PBEGIN output\nPUSH EBP\nMOV EBP,ESP\nMOV EAX,[EBP+8]\nINT 1\nMOV ESP,EBP\nPOP EBP\nRET\n.PEND");
		EmitAsmCode(buffer);
	}
}

void GenAsmCode(TreeNode *synatxTree,FILE *codeFile)
{
    if(!pCodeBlockHeader)//make sure we have already emit p code first
	  PCodeGen(synatxTree,codeFile,FALSE);
	ConvertPCode2AssemblyCode();

	asmcodestr[iAsmCodePos] ='\0';
	printf("\n---------------generated assembly-codes----------------------------\n");
	if(bOutput2Console)
	  printf("%s",asmcodestr);
	fputs(asmcodestr,codeFile);
}


