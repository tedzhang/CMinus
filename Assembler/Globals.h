#pragma  once 
#include <stdio.h>//for file operation
#include <ctype.h>
#include <assert.h>
#include "Code.h"
#define ASSERT(x) assert(x)
typedef enum
{
  TOK_ID,//identider
  TOK_REG,//such as EAX,EBX,...
  TOK_DIRECTIVE,
  TOK_OPERATION,
  TOK_INT_CONST,
  TOK_COMMA,//","
  TOK_SEMI,//";"
  TOK_ERROR,
  TOK_NOMORE,//the line code is end.just as '\n'
  TOK_LPAREN,//[
  TOK_RPAREN,//]
  TOK_ADD,//+
  TOK_SUB,//-
  TOK_ENDFILE,//file end
  TOK_BEGINFILE,
}TokenType;

typedef enum 
{
	SZ_BYTE = 1,
	SZ_WORD = 2,
	SZ_DWORD= 4,
	SZ_QWORD= 8,
}kDataType;
//all save as  little endian.for intel platform 

//32bytes
inline void DWord2Bytecode(int iVal,char arr[])
{
	char* buffer = (char*)&iVal;
	arr[0] = buffer[0];
	arr[1] = buffer[1];
	arr[2] = buffer[2];
	arr[3] = buffer[3];

}
//32bytes
inline int  Bytecode2Dword(char arr[])
{
	int a ;
	char* buffer = (char*)&a;
	buffer[0] =arr[0];
	buffer[1] = arr[1];
	buffer[2] = arr[2];
	buffer[3] = arr[3];
	return a;
}
//16 bytes
inline void Word2Bytecode(short iVal,char arr[])
{
	char* buffer = (char*)&iVal;
	arr[0] = buffer[0];
	arr[1] = buffer[1];
}
//16bytes
inline short  Bytecode2Word(char arr[])
{
	short a;
	char *buffer = (char*)&a;
	buffer[0] = arr[0];
	buffer[1] = arr[1];
	return a;
}