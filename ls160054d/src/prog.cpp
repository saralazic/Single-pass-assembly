// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
using namespace std;
#include <iostream>
#include <fstream>
#include <ostream>

#include "One_pass.h"
#include <regex>

int main(int argc, char* argv[])
{
    //std::cout << "Hello World!\n";
  
/*
    char c;
  //  cout << argv[1];
    ifstream in(argv[1]);
    if (!in)
    {
        cout << "Error! File Does not Exist";
        return 0;
    }
    cout << endl << endl;
    while (in.eof() == 0)
    {
        in.get(c);
        cout << c;
    }

    /*
    ofstream file;
    file.open(argv[2]);
    file << "Izlazni fajl!";
    file.close();
    */
    
   /*
    Symbol_table* ST = new Symbol_table();
    ST->insert("sekcija", "sekcija", 5, Symbol_table::LOCAL, Symbol_table::SECTION, nullptr);
    
    ST->insert("simbol", "sekcija", 8, Symbol_table::LOCAL, Symbol_table::SYMBOL, nullptr);

    ST->write_the_table();
    */

   // cout << argv[1] << " " << argv[2] << endl;

    cout << "Pocetak " << endl;

    cout << argv[1] << " " << argv[2] << endl;
  
   One_pass* op = new One_pass(argv[1], argv[2]);
    op->pass();
    op->writeRL(true);
    op->backpatch();
    op->writeRL(false);
    op->ST.write_the_table();
 //   int x;
 //   cin >> x;
    return 0;

}

