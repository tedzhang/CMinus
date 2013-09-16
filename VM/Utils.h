#pragma  once 


//Exe file's header's magic number
#define MAGIC_NUM1 0x44
#define MAGIC_NUM2 0x58

#define EXE_HEADERSIZE (2+4+4+4)

typedef struct exeHeaderRec
{
	char magic[2];//should be 0x44 0x58
	int  iSymTblSize;//symbol table size
	int  iStrTblSize;//string table size
	int  iBytecodeSize;//byte code size
}ExeHeaderRec;