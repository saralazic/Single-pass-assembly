#include "Instructions.h"
#include <stdio.h>
#include <string.h>


const char* Instructions::NO_OPERANDS[3] = { "halt", "ret", "iret" };
const int Instructions::nop_l[3] = { 4,3,4 };

const char* Instructions::ONE_OPERAND[8] = { "int", "call", "jmp", "jeq", "jne", "jgt", "push", "pop" };
const int Instructions::one_op_l[8] = { 3, 4, 3, 3, 3, 3, 4, 3 };

const char* Instructions::TWO_OPERANDS[14] = { "xchg", "mov", "add", "sub", "mul", "div", "cmp", "not", "and", "or", "xor", "test", "shl", "shr" };
const int Instructions::two_op_l[14] = { 4, 3, 3, 3, 3, 3, 3, 3, 3, 2, 3, 4, 3, 3 };


bool Instructions::is_jump_instruction(const char* token) {
	const char* jumps[6] = { "int", "call", "jmp", "jeq", "jne", "jgt" };
	int l[6] = { 3, 4, 3, 3, 3, 3 };

	for (int i = 0; i < 6; i++) {
		if (strncmp(token, jumps[i], l[i]) == 0) {
			return true;
		}
	}
	return false;
}

string Instructions::getOpcode(char* instruction, bool byte)
{
	if (strncmp(instruction, "halt", 4) == 0) return "00";
	if (strncmp(instruction, "iret", 4) == 0) return "08";
	if (strncmp(instruction, "ret", 3) == 0) return "10";

	if (strncmp(instruction, "int", 3) == 0)
		if (byte) return "18";
		else return "1C";
	if (strncmp(instruction, "call", 4) == 0)
		if (byte) return "20";
		else return "24";
	if (strncmp(instruction, "jmp", 3) == 0)
		if (byte) return "28";
		else return "2C";
	if (strncmp(instruction, "jeq", 3) == 0)
		if (byte) return "30";
		else return "34";
	if (strncmp(instruction, "jne", 3) == 0)
		if (byte) return "38";
		else return "3C";
	if (strncmp(instruction, "jgt", 3) == 0)
		if (byte) return "40";
		else return "44";
	if (strncmp(instruction, "push", 4) == 0)
		if (byte) return "48";
		else return "4C";
	if (strncmp(instruction, "pop", 3) == 0)
		if (byte) return "50";
		else return "54";

	if (strncmp(instruction, "xchg", 4) == 0)
		if (byte) return "58";
		else return "5C";
	if (strncmp(instruction, "mov", 3) == 0)
		if (byte) return "60";
		else return "64";
	if (strncmp(instruction, "add", 3) == 0)
		if (byte) return "68";
		else return "6C";
	if (strncmp(instruction, "sub", 3) == 0)
		if (byte) return "70";
		else return "74";
	if (strncmp(instruction, "mul", 3) == 0)
		if (byte) return "78";
		else return "7C";
	if (strncmp(instruction, "div", 3) == 0)
		if (byte) return "80";
		else return "84";
	if (strncmp(instruction, "cmp", 3) == 0)
		if (byte) return "88";
		else return "8C";
	if (strncmp(instruction, "not", 3) == 0)
		if (byte) return "90";
		else return "94";
	if (strncmp(instruction, "and", 3) == 0)
		if (byte) return "98";
		else return "9C";
	if (strncmp(instruction, "or", 2) == 0)
		if (byte) return "A0";
		else return "A4";
	if (strncmp(instruction, "xor", 3) == 0)
		if (byte) return "A8";
		else return "AC";
	if (strncmp(instruction, "test", 4) == 0)
		if (byte) return "B0";
		else return "B4";
	if (strncmp(instruction, "shl", 3) == 0)
		if (byte) return "B8";
		else return "BC";
	if (strncmp(instruction, "shr", 3) == 0)
		if (byte) return "C0";
		else return "C4";
	return "XX"; //nije nadjen opcod, instrukcija nekorektna
}
