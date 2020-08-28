//#pragma once
using namespace std;
#include <string>

class Convert
{
public :
	static string BinToHex(string bin);
	static string DecToHex(int dec);
	static string DecToBin1Digit(int dec);
	static char* StringToCharArr(string str, char from, char without, char to);
};

