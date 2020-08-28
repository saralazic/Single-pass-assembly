//#pragma once
#include <string>
using namespace std;

class Instructions
{
public:

	static const char* NO_OPERANDS[3];
	static const char* ONE_OPERAND[8];
	static const char* TWO_OPERANDS[14];

	static const int nop_l[3];
	static const int one_op_l[8];
	static const int two_op_l[14];

	static string getOpcode(char* instruction, bool byte);
	static bool is_jump_instruction(const char* token);

};

