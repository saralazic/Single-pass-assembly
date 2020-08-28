#include "Symbol_table.h"
#include <iostream>
#include <fstream>
#include <ostream>
#include <iomanip>

using namespace std;

Symbol_table::Symbol_table() {
	head = nullptr;
	tail = nullptr;
	temp = nullptr;
	len = 0;
}


Symbol_table::~Symbol_table() {
	if (tail) {
		tail->next = nullptr;
	}
	while (head!=nullptr)
	{
		Symbol_table::Element* old = head;
		head = head->next;
		delete old;
	}
	len = 0;
	head = tail = nullptr;
}

int Symbol_table::length() {
	return len;
}

Symbol_table::Element* Symbol_table::insert(string nm, string sec, bool def, int val, int vis, int t, Element* n)
{
//	cout << endl << "ulazi za " << nm << endl;

	int ord = len+1;
	Section_info* section = (t == SECTION) ? new Section_info(ord, sec) : findSectionInfo(sec);

	//cout << section->sec_name << " " << section->sec_num;


	if (section == nullptr) {
	//	cout << "Greska, sekcija nepoznata?!";
		return nullptr;
	}

	Symbol_table::Element* tmp = new Element(nm, section, def, val, vis, ord, t, nullptr);
	len++;

	if (head == nullptr) {
		head = tail = tmp;
	}
	else
	{
		tail->next = tmp;
		tail = tail->next;
	}
	return tmp;
}


Section_info* Symbol_table::findSectionInfo(string name)
{
	Element* tmp = find_by_name(name);
	return (tmp) ? tmp->section : nullptr;
}

Symbol_table::Element* Symbol_table::find_by_name(string name){
	//cout << "Uslo se u find_by_name za " << name << endl;
	Symbol_table::Element* my_temp = head;
	while (my_temp) {
		if (name.compare(my_temp->name)==0){
		//	cout << "nadjen" <<name<< endl;
			return my_temp;
		}
		my_temp = my_temp->next;
	}
//	cout << "Nije nadjen " << name << endl;
	return nullptr;
};

Symbol_table::Element* Symbol_table::find_by_ordinal(int ord) {
	Symbol_table::Element* my_temp = head;
	while (my_temp) {
		if (ord == my_temp->ordinal) {
			return my_temp;
		}
		my_temp = my_temp->next;
	}
	return nullptr;
};

void Symbol_table::write_the_table() {

	ofstream myfile;
	myfile.open("output/symbol_table.txt");

	if (length() == 0) {
		myfile << "Tabela simbola je prazna";
	}
	else {
		myfile << "#Tabela simbola\n";
		myfile << setw(12) << "Redni broj | " << setw(15)<< "Ime | " << setw(15) << "Sekcija | " << setw(15) << "Vrednost | " << setw(15) << "Velicina | " << setw(15) << "Vidljivost | " << setw(9) << "Tip ";
		myfile << endl;
		
		for (Symbol_table::Element* curr = head; curr != nullptr; curr = curr->next) {
			myfile << setw(10) << curr->ordinal << " | " << setw(12)<< curr->name << " | " << setw(12) << curr->section->sec_num << " | " << setw(12) << curr->value << " | " << setw(12) << curr->size << " | " ;

			if (curr->vsb == LOCAL) myfile<< setw(12) << 'l' << " | ";
			else if (curr->vsb == GLOBAL) myfile << setw(12) << 'g' << " | ";
			else myfile << setw(12) << 'e' << " | ";

			

			if (curr->type == SECTION) myfile << setw(9) << "sekcija ";
			else if (curr->type == SYMBOL) myfile << setw(9) << "simbol ";
			else myfile << "equ ";
			myfile << endl;
		}
			myfile << "#end\n";
	}

	myfile.close();

}



