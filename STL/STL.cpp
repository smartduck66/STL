//
// Book : chapitres 20 & 21 de l'ouvrage
// "Programming -- Principles and Practice Using C++" de Bjarne Stroustrup (2ème édition : 2014)
// Commit initial : 10/08/2018 
// Commit en cours : 10/08/2018 - Exercice n°2 de la page 754
// Caractères spéciaux : [ ]   '\n'   {  }   ||   ~   _     @

// Librairie indispensable sous Windows et VC++
#include "stdafx.h"

#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<array>
#include <type_traits>	//pour is_same
#include "MyHeader.h"

using namespace std;

double* get_from_jack(int* count)
{  

	// Ouverture en lecture du fichier 
	string filename1 = "jack.txt";
	ifstream ist1{ filename1 };
	ist1.exceptions(ist1.exceptions() | ios_base::badbit);
	if (!ist1)error("Impossible d'ouvrir le fichier ", filename1);

	double* a = new double[10];				// C-style array
	double* p = &a[0];

	double d = 0;
	string s{};
	while (!ist1.eof()) {					// Tant que la fin du fichier n'est pas atteinte, on continue à extraire de nouveaux enregistrements
		getline(ist1, s);					// On lit une ligne terminée par un NEWLINE
		d = stod(s);						// Conversion de la chaîne en double
		*p = d;								// Affectation de l'array
		++p;								// On avance l'itérateur
		++ *count;							// On incrémente également le nombre de valeurs (passé en argument)
	}

	ist1.close();							// Fermeture du fichier
	
	return &a[0];							// On retourne l'array créée
}


vector<double>* get_from_jill()
{  
	// Ouverture en lecture du fichier 
	string filename1 = "jill.txt";
	ifstream ist1{ filename1 };
	ist1.exceptions(ist1.exceptions() | ios_base::badbit);
	if (!ist1)error("Impossible d'ouvrir le fichier ", filename1);

	string s{};
	auto numbers=new vector <double>;		// auto évite de répéter le type
	
	double d = 0;
	while (!ist1.eof()) {					// Tant que la fin du fichier n'est pas atteinte, on continue à extraire de nouveaux enregistrements
		getline(ist1, s);					// On lit une ligne terminée par un NEWLINE
		d = stod(s);
		numbers->push_back(d);
		
	}

	ist1.close();							// Fermeture du fichier

	return &numbers[0];						// On retourne la référence en mémoire du vecteur créé

}

double* high(double* first, double* last)
{  

	/* 20.1.1
	int jack_count = 0;
	double* jack_data = get_from_jack(&jack_count);
	vector<double>* jill_data = get_from_jill();
	double h = -1;
	double* jack_high;
	double* jill_high;
	for (int i=0;i<jack_count;++i)
		if (h<jack_data[i]) {
			jack_high = &jack_data[i];
			h= jack_data[i];		
		}

	h = -1;
	for (int i=0;i<jill_data->size();++i)
		if (h<(*jill_data)[i]) {
			jill_high = &(*jill_data)[i];
			h = (*jill_data)[i];
		}

	*/

	// New code générique : 20.1.2
	double h = -1;
	double* high{};
	for (double* p=first;p!=last;++p)
		if (h < *p) { high = p; h = *p; }
	return high;

}

// Overloading de l'opérateur << pour imprimer un vecteur : exo 5 page 754
template <typename T>
ostream& operator << (ostream& os, const vector<T>& v)
{
	os << "[";
	for (typename vector<T>::const_iterator ii = v.begin(); ii != v.end(); ++ii)
	{
		os << " " << *ii;
	}
	os << "]"<<endl;
	return os;
}

template <typename T>
istream& operator>>(istream& is, vector<T>& v)
{
	
	T val{};
	while (is>>val) {
		if (!is) return is;
		if (!is_same<T, decltype(val)>::value)	// On teste si le type de la valeur saisie est bien celui du type T attendu pour correctement remplir le vecteur
			error("Mauvaise saisie !! ");
		else
			v.push_back(val);
	}
	
	return is;
	
}

int main()
{
	try
	{
		// A mettre dans la 1ère ligne du "main" pour afficher les caractères accentués sous la console Windows
		std::locale::global(std::locale("fr-FR"));

		// Exo 5 page 754
		vector <int> i1{ 1,2,3,4,5 };
		cout << i1;

		// Exo 2 page 754
		int jack_count = 0;
		double* jack_data = get_from_jack(&jack_count);
		vector<double>* jill_data = get_from_jill();
		
		double* jack_high = high(jack_data,jack_data+jack_count);
		vector<double>&v = *jill_data;
		double* jill_high = high(&v[0], &v[0] + v.size());

		cout << "Jack's max: " << *jack_high <<endl;
		cout << "Jill'l max: " << *jill_high <<endl;
		cout << *jill_data;		// On utilise l'overloading de l'exo 5

		// On détruit les containers créés sur la free store via new (page 713)
		delete jack_data;
		delete jill_data;
		
		// Exo 5bis : saisie de valeurs dans un vecteur via l'overloading
		cout << "Saisir un vecteur d'int (CTRL Z pour sortir ; Retour Chariot après chaque valeur saisie)" << endl;
		vector <int> i2{};
		cin >> i2;
		cout << "Le vecteur i2 est composé des valeurs suivantes :";
		cout << i2;

		keep_window_open();

		return 0;
	}
	

	catch (runtime_error& e)
	{	// this code is to produce error messages
		cout << e.what() << '\n';
		keep_window_open("q");	// For some Windows(tm) setups
	}
}
