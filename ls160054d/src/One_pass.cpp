#include "One_pass.h"
#include "Convert.h"
#include "Instructions.h"

#include <fstream>
#include <regex>
#include <iostream>
#include <string>
using namespace std;


int One_pass::OperandSize(char* token, bool is_destination, bool byte, bool jmps)
{
//	cout << "pozvana fja za operand" << endl;

	//check if there is an operand at all
	if (token == nullptr) {
		cout << "Nema operanda"<<endl;
		return 0;
	}

	//operands for instructions which work with data
	std::regex data_imm_lit_d{ ("([$][0-9]+)") };
	std::regex data_imm_lit_hex{ ("([$][0][x][0-9a-fA-F]+)") };
	std::regex data_imm_symbol{ ("(\\$)([a-zA-Z_][a-zA-Z0-9]*)") };
	std::regex data_regdir_low{ "%r[0 - 7]L" };
	std::regex data_regdir_high{ "%r[0 - 7]H" };
	std::regex data_regdir{ "%r[0 - 7]" };
	std::regex data_regind{ "(\\()(\\%)r[0-7](\\))" };
	std::regex data_regind_lit{ "([0-9]+)(\\()(\\%)r[0-7](\\))" };
	std::regex data_regind_sym{ "([a-zA-Z_][a-zA-Z0-9]*)(\\()(\\%)r[0-7](\\))" };
	std::regex data_pcrel{ "([a-zA-Z_][a-zA-Z0-9]*)(\\()(\\%)pc(\\))" };
	std::regex data_pcrel_r7{ "([a-zA-Z_][a-zA-Z0-9]*)(\\()(\\%)r7(\\))" };
	std::regex data_memdir_lit_d{ ("([0-9]+)") };
	std::regex data_memdir_lit_hex{ ("([0][x][0-9a-fA-F]+)") };
	std::regex data_memdir_sym{ ("([a-zA-Z_][a-zA-Z0-9]*)") };

	//operands for jump instructions 
	std::regex jump_lit_d{ ("([0-9]+)") };
	std::regex jump_lit_hex{ ("([0][x][0-9a-fA-F]+)") };
	std::regex jump_sym{ ("([a-zA-Z_][a-zA-Z0-9]*)") };
	std::regex jump_regdir{ "(\\*)(%r[0 - 7])" };
	std::regex jump_regdir_low{ "(\\*)(%r[0 - 7])L" };
	std::regex jump_regdir_high{ "(\\*)(%r[0 - 7])H" };
	std::regex jump_regind{ "(\\*)(\\()(\\%)r[0-7](\\))" };
	std::regex jump_regind_lit{ "(\\*)([0-9]+)(\\()(\\%)r[0-7](\\))" };
	std::regex jump_regind_sym{ "(\\*)([a-zA-Z_][a-zA-Z0-9]*)(\\()(\\%)r[0-7](\\))" };
	std::regex jump_pcrel{ "(\\*)([a-zA-Z_][a-zA-Z0-9]*)(\\()(\\%)pc(\\))" };
	std::regex jump_pcrel_r7{ "(\\*)([a-zA-Z_][a-zA-Z0-9]*)(\\()(\\%)r7(\\))" };
	std::regex jump_memdir_lit_d{ ("(\\*)([0-9]+)") };
	std::regex jump_memdir_lit_hex{ ("(\\*)([0][x][0-9a-fA-F]+)") };
	std::regex jump_memdir_sym{ ("(\\*)([a-zA-Z_][a-zA-Z0-9]*)") };


	bool immhex = (regex_match(token, data_imm_lit_hex) == 1 && !jmps) || (regex_match(token, jump_lit_hex) == 1 && jmps);
	bool immdec = (regex_match(token, data_imm_lit_d) == 1 && !jmps) || (regex_match(token, jump_lit_d) == 1 && jmps);
	if (immhex || immdec) {
		if (is_destination) {
			cout << "Neposredno adresiranje nije validan nacin adresiranja za destinacioni operand" << token << endl;
			exit(-1);
		}

		string s;
		if (immdec) {
			char* tokn= Convert::StringToCharArr(token, '\0', '$', '\0');
			int val = atoi(tokn);
			s = "0x"+ Convert::DecToHex(val); 
		}
		else { //if it is not decimal then is hex value
			s = Convert::StringToCharArr(token, '\0', '$', '\0'); 
		}


		char* v = Convert::StringToCharArr(s, 'x', '\0', '\0');
		if (byte) {
			currentRelList->content += "00"; //opdescr byte
			LC += 1;
			currentRelList->addByteToRLContent(v); //operand
			return 1;
		}
		else {
			currentRelList->content += "00"; //opdescr byte
			LC += 1;
			currentRelList->addWordToRLContent(v); //operand
			return 2;
		}
		cout << "Greska u neposrednom";
		return 0;
	}


	if ((regex_match(token, data_imm_symbol) == 1 && !jmps)
		|| (regex_match(token, jump_sym) == 1 && jmps)) {
//		cout << "neposredno" << token << endl;
		if (is_destination) {
			cout << "Neposredno adresiranje nije validan nacin adresiranja za destinacioni operand" << token << endl;
			exit(-1);
		}

		token = Convert::StringToCharArr(token, '\0', '$', '\0');

		Symbol_table::Element* cur;
		cur = ST.find_by_name(token);
		if (cur == nullptr) {
			cur = ST.insert(token, currentSectionST->section->sec_name, false, 0, Symbol_table::LOCAL, Symbol_table::SYMBOL, nullptr);
		}
	

		currentRelList->content += "00"; //opdescr byte
		LC += 1;

		if (cur->defined == false && cur->vsb != Symbol_table::EXTERN) {
			if (jmps) {
				cur->insert_fr(currentSectionST->section, LC, 2);
			}
			else {
				if (byte) {
					cur->insert_fr(currentSectionST->section, LC, 1);
				}
				else {
					cur->insert_fr(currentSectionST->section, LC, 2);
				}
			}
		}

		if (cur->vsb == Symbol_table::EXTERN) {
			if (cur->section->sec_name == "UND") {
				string val = Convert::DecToHex(cur->value);
				currentRelList->addWordToRLContent(Convert::StringToCharArr(val, '\0', '\0', '\0'));
				return 2;
			}
			
			Symbol_table::Element* cur_sec;
			cur_sec = ST.find_by_name(cur->section->sec_name);
			char* smb = Convert::StringToCharArr(cur_sec->name, '\0', '\0', '\0');

			addSymbolToRL(smb, "R_386_16", 2);
			return 2;
		}

		//local ili global
		addSymbolToRL(token, "R_386_16", (byte) ? 1 : 2);
		return (byte) ? 1 : 2;
	}



	bool mem_dec = (regex_match(token, data_memdir_lit_d) == 1 && !jmps) || (regex_match(token, jump_memdir_lit_d)==1 && jmps);
	bool mem_hex = (regex_match(token, data_memdir_lit_hex) == 1 && !jmps) || (regex_match(token, jump_memdir_lit_hex)==1 && jmps);
	if (mem_dec || mem_hex) {
//		cout << "memorijsko " << token << endl;
		token = Convert::StringToCharArr(token, '\0', '*', '\0');
		if (mem_dec) {
			int val = atoi(token);
			string s = Convert::DecToHex(val);
			token = Convert::StringToCharArr(s, '\0', '\0', '\0');
		}

		char* cont = Convert::StringToCharArr(token, '\0', '\0', '\0');
		
		currentRelList->content += "80"; //opdescr
		LC += 1; 

		//all addresses are 2B
		currentRelList->addWordToRLContent(cont);
		return 2; 
	}


	if ((regex_match(token, data_memdir_sym)==1 && !jmps) ||
		(regex_match(token, jump_memdir_sym)==1 && jmps)) {
//		cout << "memorijsko " << token << endl;
		token = Convert::StringToCharArr(token, '\0', '*', '\0');
		Symbol_table::Element* cur = ST.find_by_name(token);
		if (cur == nullptr) {
			ST.insert(token, currentSectionST->section->sec_name, false, 0, Symbol_table::LOCAL, Symbol_table::SYMBOL, nullptr);
		}	

		currentRelList->content += "80";//opdescr
		LC += 1;

		if (cur->defined == false && cur->vsb != Symbol_table::EXTERN) {
			cur->insert_fr(currentSectionST->section, LC, 2);
		}

		if (cur->vsb == Symbol_table::EXTERN) {
			if (cur->section->sec_name == "UND"){
				string val = Convert::DecToHex(cur->value);
				currentRelList->addWordToRLContent(Convert::StringToCharArr(val, '\0', '\0', '\0'));
				return 2;
				
			}

			Symbol_table::Element* cur_sec;
			cur_sec = ST.find_by_name(cur->section->sec_name);
			char* smb = Convert::StringToCharArr(cur_sec->name, '\0', '\0', '\0');
			addSymbolToRL(smb, "R_386_16", 2);
			return 2;
		}
		addSymbolToRL(token, "R_386_16", 2);
		return 2;
	}

	//regdir for 2 bytes
	if ((regex_match(token, data_regdir) == 1 && !jmps)
		|| (regex_match(token, jump_regdir)==1 && jmps)) {
		string am = "001";
	
		char* val = Convert::StringToCharArr(token, 'r', '\0', '\0');

		string r = Convert::DecToBin1Digit(atoi(val));
		string op_descr_bin = am + r + "0";
		string op_descr = Convert::BinToHex(op_descr_bin);
		currentRelList->content += op_descr;
		LC += 1;
		return 0;
	}


	bool low = regex_match(token, data_regdir_low) == 1 && !jmps;
	bool high = regex_match(token, data_regdir_high) == 1 && !jmps;
	bool j_low = regex_match(token, jump_regdir_high) == 1 && jmps;
	bool j_high = regex_match(token, jump_regdir_low) == 1 && jmps;

	if (low || high || j_high || j_low) {
		string am = "001";
		
		char* val;
		
		if (low || j_low) val = Convert::StringToCharArr(token, 'r', 'L', '\0');
		else val = Convert::StringToCharArr(token, 'r', 'H', '\0');
		string r = Convert::DecToBin1Digit(atoi(val));
		string hl = (low || j_low) ? "0" : "1";
		string op_descr_bin = am + r + hl;
		string op_descr = Convert::BinToHex(op_descr_bin);
		currentRelList->content += op_descr;
		LC += 1;

		return 0; //there is no im/di/ai bytes
	}

	if ((regex_match(token, data_regind) == 1 && !jmps) ||
		(regex_match(token, jump_regind) == 1 && jmps)) {
		char* reg = Convert::StringToCharArr(token, 'r', '\0', ')');
		
		string am = "010"; 
		string r = Convert::DecToBin1Digit(atoi(reg));
		string opdescr_bin = am + r + "0";
		string opdescr = Convert::BinToHex(opdescr_bin);

		currentRelList->content += opdescr;
		LC += 1;

		return 0;
	}

	if ((regex_match(token, data_regind_lit)==1 && !jmps) ||
		(regex_match(token, jump_regind_lit)==1 && jmps)) {
		char* disp;
		disp = Convert::StringToCharArr(token, '\0', '*', '(');
	
		string am = "011";
		char* reg = Convert::StringToCharArr(token, 'r', '\0', ')');
		int val_ = atoi(reg);
		string r = Convert::DecToBin1Digit(val_);
		char* op_descr_bin = Convert::StringToCharArr(am + r + '0', '\0', '\0', '\0');
		string op_descr = Convert::BinToHex(op_descr_bin);

		int val = atoi(disp);
		if (val == 0) {
			string am = "010";
			char* reg = Convert::StringToCharArr(token, 'r', '\0', ')');
			int val_ = atoi(reg);
			string r = Convert::DecToBin1Digit(val_);
			char* op_descr_bin = Convert::StringToCharArr(am + r + '0', '\0', '\0', '\0');
			string op_descr = Convert::BinToHex(op_descr_bin);
			currentRelList->content += op_descr;
			LC += 1;
			return 0;
		}
		else {
				string am = "011";

				char* reg = Convert::StringToCharArr(token, 'r', '\0', ')');
				
				int val_ = atoi(reg);
				
				string r = Convert::DecToBin1Digit(val_);
				char* op_descr_bin = Convert::StringToCharArr(am + r + '0', '\0', '\0', '\0');
				string op_descr = Convert::BinToHex(op_descr_bin);
				currentRelList->content += op_descr;
				LC += 1;

				string s = Convert::DecToHex(val);
				char* s_ = Convert::StringToCharArr(s, '\0', '\0', '\0');
				currentRelList->addWordToRLContent(s_);
				return 2;  
		}
	}


	if ((regex_match(token, data_regind_sym)==1 && !jmps) ||
		(regex_match(token, jump_regind_sym) == 1 && jmps)) {
		char* disp = Convert::StringToCharArr(token, '\0', '*', '(');
		char* reg = Convert::StringToCharArr(token, 'r', '\0', ')');
		
		//FALI TI SLUCAJ KADA JE DEFINISANO EQU DIREKTIVOM 
		
		Symbol_table::Element* cur = ST.find_by_name(disp);
		if (cur == nullptr) {
			cur = ST.insert(disp, currentSectionST->section->sec_name, false, 0, Symbol_table::LOCAL, Symbol_table::SYMBOL, nullptr);
		}
		
		if (reg != "7") { //r7=pc
			string am = "011";
			string r = Convert::DecToBin1Digit(atoi(reg));
			string opdescr_bin = am + r + "0";
			string opdescr = Convert::BinToHex(opdescr_bin);
			currentRelList->content += opdescr;
			LC += 1;

			if (cur->defined == false && cur->vsb != Symbol_table::EXTERN) {
				cur->insert_fr(currentSectionST->section, LC, 2);
			}
				
			addSymbolToRL(disp, "R_386_PC16", 2);
			return 2;
		}
	}

	if ((regex_match(token, data_pcrel) == 1 && !jmps) ||
		(regex_match(token, data_pcrel_r7) == 1 && !jmps) ||
		(regex_match(token, jump_pcrel) == 1 && jmps) || 
		(regex_match(token, jump_pcrel_r7) == 1 && jmps)) 
	{	
		char* disp = Convert::StringToCharArr(token, '\0', '*', '(');
		Symbol_table::Element* cur = ST.find_by_name(disp);
		if (cur == nullptr) {
			cur = ST.insert(disp, currentSectionST->section->sec_name, false, 0, Symbol_table::LOCAL, Symbol_table::SYMBOL, nullptr);
			
		}

		if (cur->vsb == Symbol_table::EXTERN) {
			string val = Convert::DecToHex(cur->value);
			currentRelList->addWordToRLContent(Convert::StringToCharArr(val, '\0', '\0', '\0'));
			return 2;
		}
		string am = "011";
		string r = "0111"; //r7
		string opdescr_bin = am + r + "0";
		string opdescr = Convert::BinToHex(opdescr_bin);
		currentRelList->content += opdescr;
		LC += 1;

		//local ili global je svakako
		if (cur->defined) {
			cur->insert_fr(currentSectionST->section, LC, 2);
		}

		addSymbolToRL(disp, "R_386_PC16", 2);
		return 2;
	}
	
	cout << "GRESKA U ADRESIRANJU " << token << endl;
	return 0;
}

void One_pass::writeRL(bool before)
{
	RLforSection* tmp = RLhead;
	ofstream myfile;
	string s = (before) ? "output/before_backpatch.txt" : output;

	myfile.open(s);

	for (; tmp != nullptr; tmp = tmp->next) {
		cout << endl << endl;
		cout << endl << "name: "<<tmp->RL->name;
		cout << endl << tmp->RL->content;
			if (tmp->RL->length() == 0) {
				myfile << "";
			}
			else {
				myfile << "#.ret";
				if (tmp->RL->name == "text" || tmp->RL->name == "data" || tmp->RL->name == "bss")
					myfile << ".";
				myfile << tmp->RL->name.c_str() << "\n";
				myfile << "Offset|" << "Tip|" << "vrednost[" << tmp->RL->name.c_str() << "]:\n";


				for (Relocation_list::Element* cur = tmp->RL->head; cur != nullptr; cur = cur->next) {
					cout << "\n" << cur->offset << "| " << cur->type.c_str() << "| " << cur->value << "\n";
					myfile << cur->offset << "| " << cur->type.c_str() << "| " << cur->value << "\n";
				}

			//	myfile << "#end\n";
			/*	myfile << "#";
				if (tmp->RL->name == ".text" || tmp->RL->name == ".data" || tmp->RL->name == ".bss")
				{
					myfile << ".";
				}
				myfile << tmp->RL->name.c_str() << "\n";
				int i = 0;
				char c = tmp->RL->content[i];
				while (c != '\0') {
					myfile << c;
					i++;
					if (i % 20 == 0) myfile << '\n';
					if (i % 2 == 0 && i % 20 != 0) myfile << ' ';
					c = tmp->RL->content[i];
				}
				myfile << "\n#end\n\n";
				*/
			}

	}
	tmp = RLhead;
	
	for (; tmp != nullptr; tmp = tmp->next) {
		cout << endl << endl;
		cout << endl << "name: " << tmp->RL->name;
		cout << endl << tmp->RL->content;
		
			myfile << '#';
			if (tmp->RL->name == "text" || tmp->RL->name == "data" || tmp->RL->name == "bss")
				myfile << ".";
			myfile << tmp->RL->name.c_str() << "\n";
		//	myfile << "Offset|" << "Tip|" << "vrednost[" << tmp->RL->name.c_str() << "]:\n";
			/*

			for (Relocation_list::Element* cur = tmp->RL->head; cur != nullptr; cur = cur->next) {
				cout << "\n" << cur->offset << "| " << cur->type.c_str() << "| " << cur->value << "\n";
				myfile << cur->offset << "| " << cur->type.c_str() << "| " << cur->value << "\n";
			}

			myfile << "#end\n";
			myfile << "#";
			
			if (tmp->RL->name == ".text" || tmp->RL->name == ".data" || tmp->RL->name == ".bss")
			{
				myfile << ".";
			}
			myfile << tmp->RL->name.c_str() << "\n";
			*/
			int i = 0;
			char c = tmp->RL->content[i];
			while (c != '\0') {
				myfile << c;
				i++;
				if (i % 20 == 0) myfile << '\n';
				if (i % 2 == 0 && i % 20 != 0) myfile << ' ';
				c = tmp->RL->content[i];
			}
			myfile << "\n";
		//	myfile << "\n#end\n\n";

		

	}
	myfile << "#.end\n\n";
	myfile.close();
}

void One_pass::backpatch()
{

	calculate_equ_values();

	Symbol_table::Element* cur = ST.head;
	while (cur != nullptr) {

		//ako je neki simbol nedefinisan prijavi gresku 
		if (cur->defined == false && cur->type!=Symbol_table::EQU)
		{
			cout << "Greska! Simbol " << cur->name << " nedefinisan!" << endl;
			exit(-1);
		}

		if (cur->flink != nullptr) {
			cout << cur->value << endl;
			string hex = Convert::DecToHex(cur->value);
			char* v = Convert::StringToCharArr(hex, '\0', '\0', '\0');
		
			ST_forwardrefs* temp = cur->flink;
			while (temp != nullptr) {
				//treba da odes u odgovarajucu sekciju, i u njoj zamenis odgovarajuce bajtove
				RLforSection* curRl = getRL(temp->section->sec_name);
				cout <<"simbol: " << cur->name << ", sekcija: " << curRl->RL->name << " vrednost "<< v << endl;
				
				
				curRl->RL->changeTheContent(v, temp->LC, temp->size);
				temp = temp->next;
			}
		}

		cur = cur->next;
	}
}

void One_pass::test_fr()
{
	Symbol_table::Element* cur = ST.head;
	while (cur != nullptr) {
		cout << endl << cur->name << endl;
		if (cur->type == Symbol_table::SYMBOL) {
			ST_forwardrefs* tmp = cur->flink;
			while (tmp != nullptr) {
				cout << "forward_reference: " << tmp->section->sec_name << " " << tmp->LC << endl;
				tmp = tmp->next;
			}
		}
		cur = cur->next;

	}

}

void One_pass::process_equ_statement(string token, string sym_name)
{
	cout << token << endl;
	
	if (token[token.size() - 1] == '+' || token[token.size() - 1] == '-' || token[0] == '+' || token[0] == '-') {
		cout << "Greska, izraz u equ je nekorektan!" << endl;
		exit(-1);
	}

	EQU_info* n = new EQU_info(sym_name);


	int i = 0;
	//razdvoj izraz na simbole i operatore
	while(i!=token.size()){
		string sym = "";
		while (i < token.size() && token[i] != '+' && token[i] != '-') {
			sym += token[i++];
		}
		n->symbols[n->s_cnt++] = sym;
		if(i < token.size()) n->operators[n->o_cnt++] = token[i++];
	}
	
	//proveri gresku
	if ((n->s_cnt - n->o_cnt) != 1) {
		cout << "Greska, izraz u equ je nekorektan!" << endl;
		exit(-1);
	}

	//ulancaj
	if (equs == nullptr) {
		equs = n;
		n_equ = 1;
	}
	else {
		n->next = equs;
		equs = n;
		n_equ++;
	}
	
	cout << "EQU" << endl;
	for (int i = 0; i < n->s_cnt; i++) cout << n->symbols[i] << " ";
	cout<<endl;
	for (int i = 0; i < n->o_cnt; i++) cout << n->operators[i] << " ";
	cout << endl;
}

void One_pass::calculate_equ_values()
{
	std::regex lit_d{ ("([0-9]+)") };
	std::regex lit_hex{ ("([0][x][0-9a-fA-F]+)") };

	EQU_info* tmp, * prev;

	
	int cnt = n_equ;

	while (cnt > 0 && equs != nullptr) {
		cnt--;

		prev = nullptr;
		tmp = equs;

		while (tmp != nullptr) {
			cout << "Pokusaj razresavanja simbola " << tmp->name << endl;
			tmp->operators[tmp->o_cnt] = 'x';
			int res = 0;
			char o = '+';
			bool flag = true;

			for (int i = 0; i < tmp->s_cnt; i++) {
				if (regex_match(tmp->symbols[i], lit_d) == 1) {
					int v = atoi(Convert::StringToCharArr(tmp->symbols[i], '\0', '\0', '\0'));
					if (o == '+') res = res + v;
					else res = res - v;
					o = tmp->operators[i];
				}
				else {
					if (regex_match(tmp->symbols[i], lit_hex) == 1) {
						char* s = Convert::StringToCharArr(tmp->symbols[i], 'x', '\0', '\0');
						int v = atoi(s);
						if (o == '+') res = res + v;
						else res = res - v;
						o = tmp->operators[i];
					}
					else
					{//simbol je
						Symbol_table::Element* cur = ST.find_by_name(tmp->symbols[i]);
						if (cur == nullptr) {
							cout << "Greska! Simbol " << tmp->symbols[i] << "u izrazu equ direktive je nedefinisan!" << endl;
							exit(-1);
						}
						if (cur->defined == false && cur->type != Symbol_table::EQU) {
							cout << "Greska! Simbol " << tmp->symbols[i] << "u izrazu equ direktive je nedefinisan!" << endl;
							exit(-1);
						}
						if (cur->defined == false && cur->type == Symbol_table::EQU) {
							flag = false;
						}
						if (cur->defined == true) {
							int v = cur->value;
							if (o == '+') res = res + v;
							else res = res - v;
							o = tmp->operators[i];
						}

					}
				}
			}

			Symbol_table::Element* tmpST = ST.find_by_name(tmp->name);
			if (tmpST == nullptr) {
				cout << "Greska u obradi equ direktive!" << endl;
				exit(-1);
			}

			if (flag == true) {
				//ako je simbol resen izbaci ga iz liste
				tmpST->value = res;
				tmpST->defined = true;
				cout << "RAZRESEN EQU " << res << endl;
				if (prev == nullptr) {
					tmp = tmp->next;
					equs = tmp;
				}
				else {
					prev->next = tmp->next;
					tmp = tmp->next;
				}
			}
			else {
				//ako simbol nije resen predji na sledeci u listi
				prev = tmp;
				tmp = tmp->next;
			}
		}
	}
	if (equs == nullptr) cout << "Razresene sve equ"<<endl;
	else {
		cout << "Nemoguce razresiti sve equ direktive (KRUZNA ZAVISNOST)!";
		exit(-1);
	}

}

One_pass::One_pass(string inp, string out)
{
	currentSectionRL = nullptr;
	input = inp;
	output = out;
	cout << input << " " << output << endl;
}

void One_pass::addSymbolToRL(char* token, string type, int sz)
{
	Symbol_table::Element* cur = ST.find_by_name(token);
	if (cur==nullptr) {
		cout << "Greska" << token << endl;
		return;
	}

	if (cur->vsb == Symbol_table::EXTERN && cur->section->sec_name != "UND") {
		//eksterni u sekciji
		cur = ST.find_by_name(cur->section->sec_name);
	}

	if (cur->vsb == Symbol_table::LOCAL)
	{
		currentRelList->insert(LC, type, cur->section->sec_num);
	
		char* c;
		if (cur->defined==true) {
			int val = cur->value;
			if (type == "R_386_PC16") {
				val = val - 2;
			}
			string s = Convert::DecToHex(val);
			c = Convert::StringToCharArr(s, '\0', '\0', '\0');
			if (sz==1) 
				currentRelList->addByteToRLContent(c);
			else
				currentRelList->addWordToRLContent(c);
		}
		else {
			if (sz == 1) {
				c = Convert::StringToCharArr("xx", '\0', '\0', '\0');
				currentRelList->addByteToRLContent(c);
			}
			else {
				c = Convert::StringToCharArr("xxxx", '\0', '\0', '\0');
				currentRelList->addWordToRLContent(c);
			}
		}
	}
	else {
		//globalni nedefinisan
		if (cur->defined == false) {
			char* c;
			if (sz == 1) {
				c = Convert::StringToCharArr("xx", '\0', '\0', '\0');
				currentRelList->addByteToRLContent(c);
			}
			else {
				c = Convert::StringToCharArr("xxxx", '\0', '\0', '\0');
				currentRelList->addWordToRLContent(c);
			}
			return;
		}
		if (sz == 2) {
			currentRelList->insert(LC, type, cur->section->sec_num);
			if (type == "R_386_PC16") {
				currentRelList->content += "FEFF"; //-2
			}
			else currentRelList->content += "0000";
		}
		else {
			if (type == "R_386_PC16") {
				currentRelList->content += "FE"; //-2
			}
			else currentRelList->content += "00";
		}
	}
}

RLforSection* One_pass::getRL(string name)
{
	RLforSection* temp;
	for (temp = RLhead; temp != nullptr; temp = temp->next) {
		if (temp->RL->name == name) {
			return temp;
		}
	}
	return nullptr;
}

RLforSection* One_pass::addNewRL(string name)
{
	RLforSection* temp = new RLforSection(name);
	if (RLtail == nullptr) {
		RLhead = RLtail = temp;
	//	cout << endl << "dodat: " << name;
	}
	else {
		RLtail->next = temp;
		RLtail = temp;
	//	cout << endl << "dodat: " << name;
	}
	return temp;
}




void One_pass::pass() {

	cout << input << endl;

	LC = 0;
	currentSectionST = ST.insert("UND", "UND", true, LC, Symbol_table::LOCAL, Symbol_table::SECTION, nullptr);
	

	ifstream infile(input);
	if (!infile) {
		cout << "Nema ulaznog fajla!" << endl;
		return;
	}
	string line;
	const char* delimiter = " \t\n\r,";
	int indicator = 0;
	bool end = false, found;
	char* c;
	char* token;
	Symbol_table::Element* cur; //to get symbol table row if exists


	while (getline(infile, line)) {
		
		//copy line`
		c = new char[line.size() + 1];
		for (int i = 0; i < line.size(); i++) {
			c[i] = line[i];
		}
		c[line.size()] = '\0';

	//	cout << "red: "<<line << endl;

		token = strtok(c, delimiter);

		while (token) {
	//			cout << "token: " << token << endl;
			
			
			//first check is there a LABEL
			if (token[strlen(token) - 1] == ':') {
				if (token[0] == ':') {
					cout << "Labela nema naziv!" << endl;
					exit(-1);
				}
			
				token[strlen(token) - 1] = '\0'; 

				if (currentSectionST->name == "UND" || currentSectionST == nullptr) {
					cout << "Labela mora biti definisana u sekciji" << endl;
					exit(-1);
				}

				cur = ST.find_by_name(token);
				
				if (cur) { //if row in ST is found
					if (cur->vsb == Symbol_table::LOCAL && cur->defined == true) {
						cout << "Lokalni simbol vec definisan";  //local symbol is already defined
						exit(-1);
					}
					if  (cur->vsb == Symbol_table::GLOBAL && cur->defined == true) {
						//symbol is global or extern
						cout << "Globalni simbol vec definisan";
						exit(-1);
					}
					if (cur->vsb == Symbol_table::EXTERN) {
						cout << "Simbol je eksterni!";
						exit(-1);
					}
					cur->section = currentSectionST->section;
					cur->value = LC;
					cur->defined = true;
					token = strtok(0, delimiter);
	//				if (token == nullptr) cout << "NEMA TOKENA POSLE LABELE" << endl;
		//			else cout << "token: " << token << endl;
				}
				else {
					ST.insert(token, currentSectionST->name, true, LC, Symbol_table::LOCAL, Symbol_table::SYMBOL, nullptr);
					token = strtok(0, delimiter);
	//				cout << "token: " << token << endl;
				}
			}

			if (token == nullptr) {
				cout << "nema tokena" << endl;
				break;
			}

			found = false;

			//DIRECTIVES
			if (token[0] == '.') {

				//.equ
				if (!found && strncmp(token, ".equ", 4) == 0) {

					found = true;
					
					token = strtok(0, delimiter);
					if (token == nullptr) {
						cout << "Nema izraza u equ!" << endl;
						break;
					}
					
					string symbol_name = token;
					
					Symbol_table::Element* help = ST.find_by_name(token);
					if (help) {
						if (help->defined == true) {
							cout << "Vec je definisan simbol " << token << endl;
							exit(-1);
						}
						else {
							help->section = ST.find_by_name("UND")->section;
							help->type = Symbol_table::EQU;
						}
					}
					else ST.insert(token, "UND", false, 0, Symbol_table::LOCAL, Symbol_table::EQU, nullptr);

					//sada je dohvacen izraz 
					token = strtok(0, delimiter);
					if (token == nullptr) break;
	//				cout << "token: " << token << endl;
					process_equ_statement(token, symbol_name);
					
				}



				//.global
				if (!found && strncmp(token, ".global", 7) == 0) {
					found = true;
					while (token) {
						token = strtok(0, delimiter);
						if (token == nullptr) break;
		//				cout << "token: " << token << endl;
						cur = ST.find_by_name(token);
						if (cur) {
							cur->vsb = Symbol_table::GLOBAL;
						}
						else ST.insert(token, currentSectionST->section->sec_name, false, 0, Symbol_table::GLOBAL, Symbol_table::SYMBOL, nullptr);
					}
				}

				//.extern 
				if (!found && strncmp(token, ".extern", 7) == 0) {
					found = true;

					while (token) {
						token = strtok(0, delimiter);
						if (token == nullptr) break;
						cur = ST.find_by_name(token);
				//		cout << "token: " << token << endl;

						if (cur) {
							cout << "Vec je definisan simbol" << token;
							exit(-1);
						}
						else ST.insert(token, currentSectionST->section->sec_name, true, 0, Symbol_table::EXTERN, Symbol_table::SYMBOL, nullptr);
					}
				}


				//.skip
				if (!found && strncmp(token, ".skip", 5) == 0) {
					found = true;

					token = strtok(0, delimiter);
		//			cout << "token: " << token << endl;
					int v = atoi(token);
					LC +=v;
					for (int i = 0; i < v; i++)
						currentRelList->content += "00";
				}

				//.align
				if (!found && strncmp(token, ".align", 6) == 0) {
					found = true;

					token = strtok(0, delimiter);
			//		cout << "token: " << token << endl;
					int tok = atoi(token);
					int v = tok - LC % tok;
					LC += v;
					for (int i = 0; i < v; i++)
						currentRelList->content += "00";
				}

				//.end
				if (!found && strncmp(token, ".end", 4) == 0) {
					found = true;

					currentSectionST->size = LC;
					currentRelList->LC = LC;
					end = true;
					break;
				}

				//.word
				if (!found && strncmp(token, ".word", 5) == 0) {
					found = true;
					
					while (token) {
						token = strtok(0, delimiter);
						if (token == nullptr) break;

						//hex literal
						if (strncmp(token, "0x", 2) == 0) {
						
							char* v = Convert::StringToCharArr(token, 'x', '\0', '\0');
							currentRelList->addWordToRLContent(v);
						}
						else {
							std::regex number{ ("([0-9]+)") };
							//decimal literal
							if (regex_match(token, number) == 1) {
								int value = atoi(token);
								string s = Convert::DecToHex(value);
								
								char* v = Convert::StringToCharArr(s, '\0', '\0', '\0');
								currentRelList->addWordToRLContent(v);
							}
							else
							{
								//symbol
								Symbol_table::Element* cur = ST.find_by_name(token);
								if (cur == nullptr) 
									cur = ST.insert(token, currentSectionST->section->sec_name, false, 0, Symbol_table::LOCAL, Symbol_table::SYMBOL, nullptr);
								if (cur->defined == false) cur->insert_fr(currentSectionST->section, LC, 2);
								addSymbolToRL(token, "R_386_16", 2);
							}
						}

		//				cout << "token: " << token << endl;
						LC += 2;
					}
				}

				//.byte
				if (!found && strncmp(token, ".byte", 5) == 0) {
					found = true;

					while (token) {
						token = strtok(0, delimiter);
						if (token == nullptr) break;
						  
						//hex literal
						if (strncmp(token, "0x", 2) == 0) {
							
							char* v = Convert::StringToCharArr(token, 'x', '\0', '\0');
							currentRelList->addByteToRLContent(v);
						}
						else {
							std::regex number{ ("([0-9]+)") };
							
							//dec literal
							if (regex_match(token, number) == 1) {
								int value = atoi(token);
								string s = Convert::DecToHex(value);
								
								char* v = Convert::StringToCharArr(s, '\0', '\0', '\0');
								currentRelList->addByteToRLContent(v);
							}
							else {
								//symbol
								Symbol_table::Element* cur = ST.find_by_name(token);
								if (cur == nullptr)
									cur = ST.insert(token, currentSectionST->section->sec_name, false, 0, Symbol_table::LOCAL, Symbol_table::SYMBOL, nullptr);
								if (cur->defined == false) cur->insert_fr(currentSectionST->section, LC, 1);
								addSymbolToRL(token, "R_386_16", 1);
							}
						}

	//					cout << "token: " << token << endl;
						LC += 1;
					}
				}


				const char* arr[3] = { ".text", ".data", ".bss" };
				int l[3] = { 5, 5, 4 };

				//SECTIONS
				for (int i = 0; i < 3 && !found; i++) {
					if (strncmp(token, arr[i], l[i]) == 0) {
						//						cout << "SEKCIJA " << token << endl;
						found = true;
						if (currentRelList) {
							currentRelList->LC = LC;
						}
						//cout << "sekcija " << token << endl;
						token = Convert::StringToCharArr(token, '\0', '.', '\0');

						RLforSection* rel = getRL(token);
						if (rel == nullptr) rel = addNewRL(token);
						
						currentRelList = rel->RL;
						//cout << "dohvacena RL za sekciju: " << rel->RL->name << endl;
						currentSectionST->size = LC;

						cur = ST.find_by_name(token);
						if (cur == nullptr) 
							cur = ST.insert(token, token, true, 0, Symbol_table::LOCAL, Symbol_table::SECTION, nullptr);
						if (cur->vsb == Symbol_table::EXTERN || cur->vsb == Symbol_table::GLOBAL) {
							cout << "Simbol " << token << " je vec u upotrebi" << endl;
							exit(-1);
						}
						cur->defined = true;
						cur->type = Symbol_table::SECTION;
					
						//cout << "dohvacen red tabele za sekciju :" << cur->name << endl;
						LC = cur->size;
						currentSectionST = cur;
					}
				}

				if (!found && strncmp(token, ".section", 8) == 0) {
					found = true;

					if (currentRelList) {
						currentRelList->LC = LC;
					}

					token = strtok(0, delimiter);
					if (!token) {
						cout << "Nedostaje ime sekcije!";
						exit(-1);
					}
		//			cout << "token: " << token << endl;
					currentSectionST->size = LC;

					cur = ST.find_by_name(token);
					if (cur == nullptr) 
						cur = ST.insert(token, token, true, 0, Symbol_table::LOCAL, Symbol_table::SECTION, nullptr);
					if (cur->vsb == Symbol_table::EXTERN || cur->vsb == Symbol_table::GLOBAL) {
						cout << "Simbol " << token << " je vec u upotrebi" << endl;
						exit(-1);
					}
					cur->defined = true;
					cur->type = Symbol_table::SECTION;

					LC = cur->size;
					currentSectionST = cur;
			
					RLforSection* tmp = getRL(token);
					if (tmp) {
						currentRelList = tmp->RL;
					}
					else {
						currentRelList = addNewRL(token)->RL;
					}
				
				}

			}

			//INSTRUCTIONS
			//no operands
			for (int i = 0; i < 3 && !found; i++) {
				if (strncmp(token, Instructions::NO_OPERANDS[i], Instructions::nop_l[i]) == 0) {
					LC += 1;
					found = true;
					currentRelList->content += Instructions::getOpcode(token, false); 
				}
			}


			//one operand
			for (int i = 0; !found && i < 8; i++) {
				if (strncmp(token, Instructions::ONE_OPERAND[i], Instructions::one_op_l[i]) == 0) {
		//			cout <<"instrukcija 1op: "<< token << " " << i <<endl;
					
					found = true;
					int byte = (token[strlen(token) - 1] == 'b') ? true : false;
					if (byte || token[strlen(token) - 1] == 'w')
						token[strlen(token) - 1] = '\0';

					LC += 1;
					currentRelList->content += Instructions::getOpcode(token, byte);

					bool jmps = Instructions::is_jump_instruction(token);

					token = strtok(0, delimiter);
					if (token == nullptr) {
						cout << "Losa instrukcija, fali jedini operand" << endl;
						exit(-1);
					}
		//			cout << "token: " << token << endl;
					bool dst = true;
					if (strncmp(token, "push", 4)) dst = false; //only push has source operand, all the others have dst
					LC += OperandSize(token, dst, byte, jmps);
				}
			}

			//two operands
			for (int i = 0; !found && i < 14; i++) {
				if (strncmp(token, Instructions::TWO_OPERANDS[i], Instructions::two_op_l[i]) == 0) {
		//			cout << "instrukcija 2op: " << token  << endl;
					found = true;

					bool byte = false;
					if (token[strlen(token) - 1] == 'b' && !strncmp(token, "sub", 3) == 0) byte = true;
					if (strncmp(token, "subb", 4) == 0) byte = true;

					if (byte || token[strlen(token)-1]=='w') {
						token[strlen(token) - 1] = '\0';
					}
					
					LC += 1;
					currentRelList->content += Instructions::getOpcode(token, byte);

					//source
					token = strtok(0, delimiter);
					if (token == nullptr) {
						cout << "Losa instrukcija, fali source operand!";
						exit(-1);
					}
	//				cout << "token: " << token << endl;
		
					LC += OperandSize(token, false, byte, false);

					//destination
					//destination
					token = strtok(0, delimiter);
					if (token == nullptr) {
						cout << "Losa instrukcija, fali destination operand!!";
						exit(-1);
					}
	//				cout << "token: " << token << endl;
			
					LC += OperandSize(token, true, byte, false);
		//			cout << "Ocitana cela instrukcija uspesno" << endl;
				}
			}

			if (!found) {
				cout << "Nepoznata instrukcija/direktiva u liniji " << line;
				exit(-1);
			}

			token = strtok(0, delimiter);
		}

		if (end) break;
		token = strtok(0, delimiter);                                                                     
			
	}
}















