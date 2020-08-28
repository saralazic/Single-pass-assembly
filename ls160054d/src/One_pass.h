//#pragma once
#include <string>
#include "Symbol_table.h"
#include "Relocation_list.h"


using namespace std;

struct RLforSection
{
	Relocation_list* RL;
	RLforSection* next;
	RLforSection(string ls, RLforSection* n = nullptr) {
		RL = new Relocation_list(ls);
		next = n;
	}
};

struct EQU_info {
	string name;
	string symbols[100];
	char operators[100];
	int s_cnt;
	int o_cnt;
	EQU_info* next;

	EQU_info(string t) {
		name = t;
		s_cnt = 0;
		o_cnt = 0;
		next = nullptr;
	}
};

class One_pass
{
	public:
		string input;
		string output;
	
		Symbol_table ST;
		Symbol_table::Element* currentSectionST; //row in ST which is current section
		int LC; //localCounter

		
		Relocation_list* currentRelList;
		Relocation_list::Element* currentSectionRL;
		//for every section we have one relocation list
		RLforSection* RLhead;
		RLforSection* RLtail;

		EQU_info* equs;
		int n_equ;

		One_pass(string input, string output);


//		void AddSymbolInRL(char* token, string type, int len);

		void addSymbolToRL(char *token, string type, int sz);

		RLforSection* getRL(string name);
		RLforSection* addNewRL(string name);

		void pass();

		int OperandSize(char* token, bool dst, bool byte, bool jmps);

		void writeRL(bool before);

		void backpatch();

		void test_fr();

		void process_equ_statement(string token, string sym_name);
		
		void calculate_equ_values();

		

};

