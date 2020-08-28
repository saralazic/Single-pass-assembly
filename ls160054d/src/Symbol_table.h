//#pragma once
using namespace std;
#include <string>
#include <iostream>

using namespace std;

struct Section_info{
	int sec_num;
	string sec_name;
	Section_info(int num, string name) {
		sec_num = num;
		sec_name = name;
	}
};


struct ST_forwardrefs {
	Section_info *section;
	int LC;
	int size;
	ST_forwardrefs* next;

	ST_forwardrefs(Section_info* sec, int pLC, int sz) {
		section = sec;
		size = sz;
		LC = pLC;
		next = nullptr;
		//cout << "inserted: " << sec->sec_name << " " << LC <<endl;
		
		
	}
};


class Symbol_table
{
public:

	//static ofstream myfile;

	struct Element {
		string name;
		Section_info* section;
		int value;
		int vsb;
		int ordinal; 
		int size; //only for section
		int type; //section, symbol, equ
		//needed for backpatch
		bool defined; // is the symbol defined or not
		ST_forwardrefs* flink;

		Element* next;
		Element(string nm, Section_info* sec, bool def, int val, int vis, int ord, int t, Element *n = nullptr){
			name = nm;
			defined = def;
			section = sec;
			value = val;
			vsb = vis;
			ordinal = ord;
			size = 0; //at first size is 0, later we set section size
			type = t;
			next = n;
			flink = nullptr; //we initialize it with null and later add elements
		}

		void insert_fr(Section_info* sec, int LC, int size) {
			//cout << "insert fr: "<<LC << endl;
			if (flink == nullptr) {
				flink = new ST_forwardrefs(sec, LC, size);
			}
			else {
				ST_forwardrefs* temp = new ST_forwardrefs(sec, LC, size);
				temp->next = flink;
				flink = temp;
			}
		}
	};

	Element* head;
	Element* tail;
	Element* temp;
	int len;

	//type
	static const int SECTION = 0;
	static const int SYMBOL = 1;
	static const int EQU = -999;

	//visibility constants
	static const int LOCAL = 0;
	static const int GLOBAL = 1;
	static const int EXTERN = 2;

	
	Symbol_table();
	~Symbol_table();

	int length();

	

	Element* insert(string nm, string sec, bool def, int val, int vis, int t, Element* n);
	


	Section_info* findSectionInfo(string name);

	Element* find_by_name(string name);
	Element* find_by_ordinal(int ord);

	void write_the_table();

};

