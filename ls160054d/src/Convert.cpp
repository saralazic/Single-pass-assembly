#include "Convert.h"
#include <iostream>
using namespace std;

string Convert::BinToHex(string bin) {
	string h_digit;
	string ret="";

	for (int i = 0, j = 4 * i; i < 2; i++) {
		h_digit = bin.substr(j, 4);
		if (h_digit == "0000") ret += "0";
			else if (h_digit == "0001") ret += "1";
			else if (h_digit == "0010") ret += "2";
			else if (h_digit == "0011") ret += "3";
			else if (h_digit == "0100") ret += "4";
			else if (h_digit == "0101") ret += "5";
			else if (h_digit == "0110") ret += "6";
			else if (h_digit == "0111") ret += "7";
			else if (h_digit == "1000") ret += "8";
			else if (h_digit == "1001") ret += "9";
			else if (h_digit == "1010") ret += "A";
			else if (h_digit == "1011") ret += "B";
			else if (h_digit == "1100") ret += "C";
			else if (h_digit == "1101") ret += "D";
			else if (h_digit == "1110") ret += "E";
			else if (h_digit == "1111") ret += "F";
	}
	return ret;
}

string Convert::DecToHex(int dec) {
	string ret = "";

	int v;
	
	if (dec == 0) return "00";

	while (dec != 0) {
		if (dec % 16 < 10) v = 48;
		else v = 55;

		ret = char(dec % 16 + v) + ret;
		dec = dec / 16;
	}
	return ret;
}

string Convert::DecToBin1Digit(int dec)
{
	string res = "XXXX";
	switch (dec)
	{
		case 0: res = "0000"; break;
		case 1: res = "0001"; break;
		case 2:	res = "0010"; break;
		case 3: res = "0011"; break;
		case 4: res = "0100"; break;
		case 5: res = "0101"; break;
		case 6: res = "0110"; break;
		case 7: res = "0111"; break;
		case 8: res = "1000"; break;
		case 9: res = "1001"; break;
		default:
			cout << "Nije jedna cifra! Greska u konverziji";
			break;
	}
	return res;
}

char* Convert::StringToCharArr(string str, char from, char without, char to)
{

	//cout << str.size()<<endl;
	
	char* res = new char[str.size()];
	bool flag = (from == '\0') ? true : false;
	bool nw = (without == '\0')? true: false;
	int i, j;
	char c;

	j = 0;
	c = str[0];
	for (i = 0; i < str.size(); i++, c = str[i]) {
		if (flag) {  //from
			if (c == to) break; //to
			//without
			if (nw) res[j++] = c;
			else {
				if (c != without) res[j++] = c;
			}
			
		}	
		else {
			if (c == from) flag = true; 
		}
	
	}
	res[j] = '\0';

	return res;
	
/*	char* c = new char[5];
	c[0] = '3';
	return c;*/
}


