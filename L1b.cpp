#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <vector>
#include <thread>
#include <mutex>
#include <cstdio>
#include <algorithm>
#include <omp.h> 

using namespace std;

/** Klasė studento duomenis saugoti
@class Student*/

class Student
{
public: string name; // vardas ir pavardė
public: string tilte;  // fakulteto pavadinimas
public: int course; // kursas
		int length; // studentų skaičius
public: double average; // pažymių vidurkis

public: Student() {}

		/* Studento duomenys
		@param til - nauja fakulteto reikšmė
		@param len - nauja skaičiaus reikšmė*/
public: Student(string til, int len)
{
	tilte = til;
	length = len;
}

		/* Studento duomenys
		@param nam - nauja vardo ir pavardės reikšmė
		@param cour - nauja kurso reikšmė
		@param ave - nauja vidurkio reikšmė*/
public: Student(string nam, int cour, double ave)
{
	name = nam;
	course = cour;
	average = ave;
}


};

/** Klasė fakulteto saugoti studentų duomenis
@class Faculty*/
class Faculty
{
public: string tilte; // fakulteto pavadinimas
		int stucout; // studentu skaicius
		vector<Student> Students;

		/* Grąžina nurodyto indekso studentų objektą.
		@param i - studentų indeksas*/
public:
	Student getStudent(int i) {
		return Students[i];
	}
	
	/* Dėti į studentų objektų masyvą naują studentą
	ir masyvo dydį.
	@param i - indeksas
	@param s - studentų objektas*/
	void setStudent(int i, Student s) {
		Students[i] = s;
	}

	/* Dėti į studentų objektų masyvą naują studentą.
	@param s - studentų objektas*/
	void setStudent(Student s) {
		Students.push_back(s);
	}

	/* Grąžina visą studentų objektą.*/
	vector<Student> getStudent() {
		return Students;
	}

	void setStudent(vector<Student> s) {
		Students = s;
	}

	/* Fakulteto duomenys
	@param p - nauja fauklteto pavadinimo reikšmė
	@param sk - nauja studentu skaiciaus reikšmė
	@param s - nauja objektu reikšmė*/
	Faculty(string p, int sk, vector<Student> s) {
		tilte = p;
		stucout = sk;
		Students = s;
	}

	/* Fakulteto numatytas duomenys*/
	Faculty() {
		tilte = "";
		stucout = 0;
		Students = vector<Student>();
	}
};

/** Klasė bendras saugoti strukturu duomenis
@class Overall*/
class Overall
{
	public: int i, course;
	public: string name;
	public: double average;

	/* Bendro numatytas duomenys*/
	Overall() {
		i = 0;
		name = "";
		course = 0;
		average = 0.00;
	}

	/* Bendro duomenys
	@param ii - nauja gijos pavadinimo reikšmė
	@param nam - nauja studento vardo reikšmė
	@param cour - nauja studijos kurso reikšmė
	@param aver - nauja studijos vidurkio reikšmė*/
	Overall(int ii, string nam, int cour, double aver)
	{
		i = ii; name = nam; course = cour; average = aver;
	}

};

/* Skaito duomenis iš failo
@param File - failo vardas
@param P - objektų vardas*/
void ReadData(ifstream & File, Faculty & P)
{
	string faculty, name, line;
	int studCout, course;
	double average;
	File >> P.tilte >> P.stucout;
	for (int i = 0; i < P.stucout; i++) {
		File >> name >> course >> average;
		Student s = Student(name, course, average);
		P.setStudent(s);
	}
}

/* Spausdina duomenis į failą
@param File - failo vardas
@param P - objektų vardas*/
void WriteData(FILE * File, string *S, int *I, double *D, int length)
{
	fprintf(File, "    Vardas    kursas     vid\n");
	for (int i = 0; i < length; i++) {
		fprintf(File, " %i) %s \t %i \t %g \n", i + 1, S[i].c_str(), I[i], D[i]);
	}
}

/* Spausdina duomenis į failą
@param File - failo vardas
@param P - objektų vardas*/
void RezWriteData(FILE * File, Overall * P, int length)
{
	for (int i = 0; i < length; i++) {
		fprintf(File, " gija_%i %s \t %i \t %g \n", P[i].i + 1, P[i].name.c_str(), P[i].course, P[i].average);
	}
}

bool sortbyName(Student &a, Student &b) {return (a.name < b.name || (a.name == b.name  && a.course < b.course && a.average < b.average));}
void PutDataIntoArray(Faculty F, string *P, int *I, double *D, int length);
void Execute(Overall * P, string S, int I, double D);
FILE * File;
int countsk;
mutex theLock;
omp_lock_t mylock;


int main()
{
	string filename = "IFF-5-1_GuzauskasL_L1b_dat.txt";
	string printname = "IFF-5-1_GuzauskasL_L1b_rez.txt";

	Faculty P1 = Faculty();
	
	vector<Student> Stud;
	ifstream infile;
	infile.open(filename);
	
	ReadData(infile, P1);
	Stud = P1.getStudent();
	sort(Stud.begin(), Stud.end(), sortbyName);
	P1.setStudent(Stud);
	
	string S[50];
	int I[50];
	double D[50];

	PutDataIntoArray(P1, S, I, D, P1.stucout);
	File = freopen(printname.c_str(), "w", stderr);

	WriteData(File, S, I, D, P1.stucout);
	fprintf(File, "-----------------------------------------\n");
	
	int gijuSk = P1.stucout;
	Overall P[50];
	int gijosNr = 0;
    countsk = 0;
	
	omp_set_num_threads(gijuSk);
	#pragma omp parallel private (gijosNr)
	{		
		gijosNr = omp_get_thread_num();
		Execute(P, S[countsk],I[countsk], D[countsk]);

	}
	
	RezWriteData(File, P, P1.stucout);
	fprintf(File, "Pabaiga\n");
	infile.close();
	return 0;
}

void PutDataIntoArray(Faculty F,string *P, int *I, double *D, int length)
{	
	for (int i = 0; i < length; i++)
	{
		Student S = F.getStudent(i);
		P[i] = S.name; I[i] = S.course; D[i] = S.average;
	}
}

void Execute(Overall * P, string S, int I, double D) 
{
	Overall L = Overall(countsk, S, I, D);
	P[countsk] = L;
	countsk++;
}
