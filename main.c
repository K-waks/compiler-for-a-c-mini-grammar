#include <stdio.h>
#include "compiler/compiler.h"

int main()
{
	compile_c_file("./sample/program.c", "./sample/program", 0);
	return 0;
}