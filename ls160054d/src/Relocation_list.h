//#pragma once
using namespace std;
#include<string>
#include <iostream>

class Relocation_list
{
public:
	struct Element {
		int offset;
		string type;
		int value;
		Element* next;
		Element(int off, string t, int val) {
			offset = off;
			type = t;
			value = val;
			next = nullptr;
		//	cout << "\n" << offset << type << value << "\n";
		}
	};

public:
	Element* head;
	Element* tail;
	int len;
	string name;
	string content;
	//string temp;
	int LC;

	Relocation_list();
	Relocation_list(string name); 
	~Relocation_list();
	
	int length();

	Element* find_by_value(int value);
	Element* find_by_offset(int offset);

	Element* insert(const int& off, const string& t, const int& vr);

	void addByteToRLContent(char* v);
	void addWordToRLContent(char* v);

	void changeTheContent(char* v, int LC, int size);

	void write_the_list(ofstream myfile);

};

