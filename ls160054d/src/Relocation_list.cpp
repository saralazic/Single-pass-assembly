#include "Relocation_list.h"

#include <iostream>
#include <fstream>
#include <ostream>
#include <string>
#include <cstring>

using namespace std;

Relocation_list::Relocation_list() {
	head = tail = nullptr;
	len = 0;
	LC = 0;
}

Relocation_list::Relocation_list(string nm) {
	name = nm;
	head = tail = nullptr;
	len = 0;
	LC = 0;

	//cout << "ime: " << name.c_str();
}

Relocation_list::~Relocation_list() {
	if (tail) tail->next = nullptr;
	while (head != nullptr)
	{
		Relocation_list::Element* old = head;
		head = head->next;
		delete old;
	}
	len = 0;
	head = tail = nullptr;
}

Relocation_list::Element* Relocation_list::insert(const int &off, const string &t, const int &val) {
	
//	cout << "\n" << off << t << val;

	Relocation_list::Element* tmp;
	tmp = new Element(off, t, val);

//	cout << tmp->offset << tmp->type << tmp->value;

	if (!head) {
		head = tail = tmp;
		len=1;
		return head;
	}
	else {
		len++;
		tail->next = tmp;
		tail = tmp;
		return tail;
	}
	cout << "greskaaa";
}

int Relocation_list::length() {
	return len;
}

Relocation_list::Element* Relocation_list::find_by_value(int value) {
	Relocation_list::Element* my_temp;
	my_temp = head;
	while (my_temp) {
		if (value==my_temp->value) {
			return my_temp;
		}
	}
	return nullptr;
}

Relocation_list::Element* Relocation_list::find_by_offset(int offset) {
	Relocation_list::Element* my_temp;
	my_temp = head;
	while (my_temp) {
		if (offset == my_temp->offset) {
			return my_temp;
		}
	}
	return nullptr;
};

void Relocation_list::write_the_list(ofstream myfile) {
	
	//cout << "\nime: " << name.c_str();

	if (length() == 0) {
		myfile << "";
	}
	else {
		myfile << "#.ret";
		if (name == "text" || name == "data" || name == "bss")
			myfile << ".";
		myfile << name.c_str() << "\n";
		myfile << "Offset|" << "Tip|" << "vrednost[" << name.c_str() << "]:\n";


		for (Relocation_list::Element* cur = head; cur != nullptr; cur = cur->next) {
			cout << "\n" << cur->offset << "| " << cur->type.c_str() << "| " << cur->value << "\n";
			myfile << cur->offset << "| " << cur->type.c_str() << "| " << cur->value << "\n";
		}

		myfile << "#end\n";
		myfile << "#";
		if (name == "text" || name == "data" || name == "bss")
		{
			myfile << ".";
		}
		myfile << name.c_str() << "\n";
		int i = 0;
		char c = content[i];
		while (c != '\0') {
		 myfile << c;
			i++;
			if (i % 20 == 0) myfile << '\n';
			if (i % 2 == 0 && i % 20 != 0) myfile << ' ';
			c = content[i];
		}
		myfile << "\n#end\n";

	}
}

void Relocation_list::addByteToRLContent(char* v)
{
	switch (strlen(v)) {
	case 0:
		content += "00";
		break;
	case 1:
		content += "0";
		content += v[0];
		break;
	case 2:
		content += v[0];
		content += v[1];
		break;
	case 3:
		content += v[1];
		content += v[2];
		break;
	case 4:
		content += v[2];
		content += v[3];
		break;
	default:
		break;
	}
}

void Relocation_list::addWordToRLContent(char* v)
{
	switch (strlen(v))
	{
	case 0:
		content += "0000";
		break;
	case 1:
		content += "0";
		content += v[0];
		content += "00";
		break;
	case 2:
		content += v[0];
		content += v[1];
		content += "00";
		break;
	case 3:
		content += v[1];
		content += v[2];
		content += "0";
		content += v[0];
		break;
	case 4:
		content += v[2];
		content += v[3];
		content += v[0];
		content += v[1];
		break;
	default:
		break;
	}
}

void Relocation_list::changeTheContent(char* v, int LC, int size)
{
	if (LC < 0) {
		cout << endl<< "Greska u backpatch-u, LC u forward reference je manji od 0" << endl;
		cout << "LC = " << LC << endl;
	}
/*
	cout << "LC = " << LC << endl;
	if (size == 2) {
		content[2 * LC + 0] = 'W';
		content[2 * LC + 1] = 'W';
		content[2 * LC + 2] = 'W';
		content[2 * LC + 3] = 'W';
		return;
	}


	content[2 * LC] = 'M';
	content[2 * LC + 1] = 'M';
	return;
	*/
	cout <<"Backpatch u sekciji "<<name<< " na lokaciji "<< LC<< ": "<< v << endl;
	
	if (size == 1) {
		switch (strlen(v)) {
		case 0:
			content[2*LC] = '0';
			content[2*LC + 1] = '0';
			break;
		case 1:
			content[2*LC] = '0';
			content[2*LC + 1] = v[0];
			break;
		case 2:
			content[2*LC] = v[0];
			content[2*LC + 1] = v[1];
			break;
		case 3:
			content[2*LC] = v[1];
			content[2*LC + 1] = v[2];
			break;
		case 4:
			content[2*LC] = v[2];
			content[2*LC + 1] = v[3];
			break;
		default:
			break;
		}
	}

	if (size == 2) {
		switch (strlen(v))
		{
		case 0:
			content[2*LC + 0] = '0';
			content[2*LC + 1] = '0';
			content[2*LC + 2] = '0';
			content[2*LC + 3] = '0';
			break;
		case 1:
			content[2*LC + 0] = '0';
			content[2*LC + 1] = v[0];
			content[2*LC + 2] = '0';
			content[2*LC + 3] = '0';
			break;
		case 2:
			content[2*LC + 0] = v[0];
			content[2*LC + 1] = v[1];
			content[2*LC + 2] = '0';
			content[2*LC + 3] = '0';
			break;
		case 3:
			content[2*LC + 0] = v[1];
			content[2*LC + 1] = v[2];
			content[2*LC + 2] = '0';
			content[2*LC + 3] = v[0];
			break;
		case 4:
			content[2*LC + 0] = v[2];
			content[2*LC + 1] = v[3];
			content[2*LC + 2] = v[0];
			content[2*LC + 3] = v[1];
			break;
		default:
			break;
		}
	}

}
