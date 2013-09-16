#pragma  once 
#include "Globals.h"
//Generate the p code from the tree
void PCodeGen(TreeNode *synatxTree,FILE *pcodeFile,BOOL output2File);
void GenAsmCode(TreeNode *synatxTree,FILE *asmcodeFile);
//This routine's intention is to tell the generator to
//generate parameter push stack sequence keep consistent with
//p code convention. Because I use p code to generate assembly
//code. P & assem's function args push stack sequence is different.
void SetGenPcodeOnly();
void SetOutput2Console();