#pragma  once

//Target machine op definition

typedef enum 
{
	EAX = 0,
	EBX,
	ECX,
	EDX,
	EBP,
	ESP,
	EIP,
	ZF,
	CF,
	NRG,
	//Others will be coming soon
}Register;
static struct 
{
	Register reg;
	const char* regName;
}RegNames[] =
{
	{ EAX,"EAX"},
	{ EBX,"EBX"},
	{ ECX,"ECX"},
	{ EDX,"EDX"},
	{ EBP,"EBP"},
	{ ESP,"ESP"},
	{ EIP,"EIP"},
	{ ZF,"ZF"},//ZF is differnt from intel's
	{ CF,"CF"},//if a-b > 0,CF = 0,else CF =1;
	{ NRG," "},
//Others will be coming soon
};

typedef enum
{
	MOV,
	LEA,
	ADD,
	SUB,
	MUL,
	DIV,
	//logical instruction
	CMP,

	JMP,//
	JGE,//>=
	JLE,//<=
	JGT,//>
	JLT,//<
	JE,//==
	JZ,//ZF =1 JMP
	INT,//interrupt instruction
	PUSH,
	POP,
	CALL,
	RET,
	JNZ,//zf =0,not equal then jmp
	HALT,

}Operation;
static struct  {
	Operation op;
	const char* name;
}OpNames[] = 
{
	{MOV,"MOV"},
	{LEA,"LEA"},
	{ADD,"ADD"},
	{SUB,"SUB"},
	{MUL,"MUL"},
	{DIV,"DIV"},
	{CMP,"CMP"},
	{JMP,"JMP"},
	{JGE,"JGE"},
	{JLE,"JLE"},
	{JGT,"JGT"},
	{JLT,"JLT"},
	{JE,"JE"},
	{JZ,"JZ"},
	{INT,"INT"},
	{PUSH,"PUSH"},
	{POP,"POP"},
	{CALL,"CALL"},
	{RET,"RET"},
	{JNZ,"JNZ"},
	{HALT,"HALT"},
};
typedef enum
{
	FBEGIN,
	FEND,
	LABEL,
	PAR,//PARAMETER
}Directive;
typedef enum
{
	GDW ,
}DATTYPE;
static struct 
{
	Directive dir;
	const char* name;
}Directives[] = 
{
	{ FBEGIN,".PBEGIN"},
	{ FEND,".PEND"},
	{ LABEL,".LABEL"},
	{ PAR,".PAR"},
};
static struct 
{
	DATTYPE type;
	const char* name;
}Types[]=
{
	{GDW,".GDW"},
};

