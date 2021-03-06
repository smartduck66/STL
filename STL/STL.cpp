//
// Book : chapitres 20&21 de l'ouvrage
// "Programming -- Principles and Practice Using C++" de Bjarne Stroustrup (2ème édition : 2014)
// Commit initial : 10/08/2018 
// Commit en cours : 16/08/2018 - refactoring before vf
// Caractères spéciaux : [ ]   '\n'   {  }   ||   ~   _     @

// Librairie indispensable sous Windows et VC++
#include "stdafx.h"

#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<array>
#include<list>
#include<map>
#include<type_traits>	// pour is_same
#include<algorithm>		// pour find
#include<cctype>		// pour isalpha
#include<chrono>		// pour test de timing
#include "MyHeader.h"

using namespace std;

double* get_from_jack(int* count)
{  
	// Exo 2 page 754
	ifstream ist1 = open_file_read("jack.txt");
	
	double* a = new double[10];				// C-style array
	double* p = &a[0];						// Pointeur sur le début de l'array

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
	
	return &a[0];							// On retourne la référence de l'array créée
}


vector<double>* get_from_jill()
{  
	// Exo 2 page 754
	ifstream ist1 = open_file_read("jill.txt");

	string s{};
	auto numbers=new vector <double>;		// auto évite de répéter le type
	
	double d = 0;
	while (!ist1.eof()) {					// Tant que la fin du fichier n'est pas atteinte, on continue à extraire de nouveaux enregistrements
		getline(ist1, s);					// On lit une ligne terminée par un NEWLINE
		d = stod(s);						// Conversion
		numbers->push_back(d);				// Stockage
		
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

// Overloading de l'opérateur >> pour saisir des valeurs dans un vecteur : exo 5 page 754 (perfectionnable au niveau de la gestion des erreurs)
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

// Text Editor (20.6) ************************************************************************************************************************************************ {  }
using Line = vector<char>;

class Text_iterator {
	list<Line>::iterator ln;
	Line::iterator pos;
public:
	Text_iterator(list<Line>::iterator ll, Line::iterator pp) :ln{ ll }, pos{ pp } {  }

	char& operator* () { return *pos; }
	Text_iterator& operator++();
	bool operator==(const Text_iterator& other) const { return ln == other.ln && pos == other.pos; }
	bool operator!=(const Text_iterator& other) const { return !(*this == other); }

};


Text_iterator& Text_iterator::operator++()
{
	++pos;
	if (pos == (*ln).end()) {
		++ln;
		pos = (*ln).begin();
	}
	return *this;
}


struct Document {  
	list<Line> line;
	Document() { line.push_back(Line{}); }

	Text_iterator begin() { return Text_iterator(line.begin(), (*line.begin()).begin()); }
	Text_iterator end() 
	{  
		auto last = line.end();
		--last;
		return Text_iterator(last, (*last).end());
	
	}
};

istream& operator>>(istream& is, Document& d)
{
	for (char ch; is.get(ch);) {
		d.line.back().push_back(ch);
		if (ch == '\n')
			d.line.push_back(Line{});

	}
	if (d.line.back().size()) d.line.push_back(Line{});
	return is;
}

void print(Document& d)
{  

	cout << "************************************************************************************************" << endl;

	for (auto p : d) cout << p;
	
	cout << endl<<"************************************************************************************************" << endl;
	
}

template<typename Iter>
void advance(Iter& p, int n)
{  

	while (0 < n) { ++p; --n; }

}


void erase_line(Document& d, unsigned int n)
{  

	if (n < 0 || d.line.size() - 1 <= n) return;
	auto p = d.line.begin();
	advance(p, n);
	d.line.erase(p);
}

tuple<int, int> stats(Document& d)
{
	// Fonction Helper qui retourne le nombre de caractères dans le document passé en argument ainsi que son nombre de lignes : exo 8 page 754
	int n = 0;
	int l = 0;

	for (auto p = d.begin(); p != d.end(); ++p) ++n;
	for (auto p = d.line.begin(); p != d.line.end(); ++p) ++l;
	
	return make_tuple(n, l);
	
}

string transform_vc_str(Line l)
{
	// Fonction Helper qui transforme un vecteur de chars en string : exo 6 page 754
	string s{};
	for (auto p = l.begin(); p != l.end(); ++p) s += *p;
	return s;

}

Line transform_str_vc(string s)
{
	// Fonction Helper qui transforme un string en un vecteur de chars : exo 6 page 754
	Line l{};
	for (unsigned int i = 0; i < size(s); ++i) l.push_back(s[i]);
	return l;

}

int find_and_replace(Document& d, string sfind, string sreplace)
{
	// Fonction Helper qui retourne le nombre de mots remplacés dans le document : exo 6 page 754
	int nb_occurences = 0;
	string s{};
	
	for (auto p = d.line.begin(); p != d.line.end(); ++p) {
				
		s = transform_vc_str(*p);
		
		const string::size_type t1 = s.rfind(sfind);
		if (t1 != string::npos) {
			s.replace(t1, sfind.size(), sreplace);
			*p = transform_str_vc(s);
			++nb_occurences;
		}
				
	}
	return nb_occurences;
}


bool match(Text_iterator first, Text_iterator last, const string& s)
{
Text_iterator p = first;

for (unsigned int i = 0; i<size(s); ++i) {
	if (s[i] != *p) return false;
	++p;
}

return true;

}

Text_iterator find_txt(Text_iterator first, Text_iterator last, const string& s)
{
	if (s.size() == 0) return last;
	char first_char = s[0];
	while(true) {
		auto p = find(first, last, first_char);
		if (p == last || match(p, last, s)) return p;
	first = ++p;

	}
}


bool is_ponctuation(char c)	noexcept	// Helper pour l'exercice 10 page 755 : on vérifie si le char passé est une ponctuation
{
	for (const char w : "“”:!()/.;,?-'<>[]{}")
		if (c == w)return true;
	return false;

}

int words_specific_whitespaced(Document& d)
{
	// Fonction Helper qui retourne le nombre de mots dans un document, séparés par une ponctuation : exo 10 page 755
	int nb_words = 0;
	int word_size = 0;
	auto p = d.begin();

	while (p != d.end()) {
		if (is_ponctuation(*p) && word_size > 0) {
			++nb_words;
			word_size = 0;
		}
		else {
			++word_size;

		}
		++p;

	}
	return nb_words;
}

void cleanup_punctuation(Document& d)
{
	// Fonction Helper qui remplace les ponctuations par un espace : exo 13 page 801
	string s{};

	for (auto p = d.line.begin(); p != d.line.end(); ++p) {

		s = transform_vc_str(*p);
		for (char& x : s) if (is_ponctuation(x))x = ' ';	// On remplace toute ponctuation trouvée par un espace
		*p = transform_str_vc(s);							// on réécrit la chaîne transformée

	}
	
}

void cleanup_plurals(Document& d)
{
	// Fonction Helper qui passe les mots au singulier : exo 13 page 801
	string s{};

	for (auto p = d.line.begin(); p != d.line.end(); ++p) {

		s = transform_vc_str(*p);
			
		for (auto p1=s.begin(); p1!=s.end();++p1)
		{
			// Règle : le caractère 's' trouvé ne doit pas être en 1ère position de la ligne... 
			if (*p1 == 's' && p1 > s.begin())

				// ... et le caractère d'avant doit être alphabétique, différent de 's' (business) ou de 'i' (is) et celui d'après un espace (fin de mot)
				// NB : static_cast<unsigned char> doit être utilisé avec isalpha, cf. CREF : 
				// "the behavior of std::isalpha is undefined if the argument's value is neither representable as unsigned char nor equal to EOF"
				if (isalpha(static_cast<unsigned char>(*(p1 - 1))) && *(p1 - 1) != 's' && *(p1 - 1) != 'i' && *(p1 + 1)==' ')
					*p1 = ' ';
				
		}
				
		*p = transform_str_vc(s);							// on réécrit la chaîne transformée

	}

}

void cleanup_minuscules(Document& d)
{
	// Fonction Helper qui passe tous les caractères en minuscules : exo 13 page 801
	string s{};

	for (auto p = d.line.begin(); p != d.line.end(); ++p) {

		s = transform_vc_str(*p);
		for (char& x : s) x = tolower(x);	// Tout en minuscules
		*p = transform_str_vc(s);			// on réécrit la chaîne transformée

	}

}


tuple<int, string> words_whitespaced(Document& d)
{
	// Fonction Helper qui retourne le nombre de mots dans un document, séparés par un espace : exo 9 page 755
	const int ESPACE = 32;
	int nb_words = 0;
	int word_size = 0;
	int longuest = 0;
	string word{};
	string longuest_word{};
	auto p = d.begin();

	while (p != d.end()){
		if (*p == ESPACE && word_size > 0) {
			++nb_words;
			if (word_size > longuest) 
				{ longuest = word_size; longuest_word = word; }	// On détermine le mot le plus long trouvé
			word_size = 0; word = "";
		}
		else {
			++word_size;
			word += *p;
				
		}
		++p;
		
	}
	
	return make_tuple(nb_words, longuest_word);
}


tuple<int, int,string> alphabetic_words(Document& d)
{
	// Fonction Helper qui retourne le nombre de mots alphabétiques dans un document : exo 9 page 755
	int nb_words = 0;
	int word_size = 0;
	auto p = d.begin();
	string word{};
	map<string, int> words;
	string word_most_present{};

	while (p != d.end()) {
		if (!isalpha(static_cast<unsigned char>(*p)) && word_size > 0) // On teste la fin d'un mot comme un caractère non alphabétique (pas le début...)
		{
			++nb_words;
			++words[word];	// Page 777 pour l'explication
			word_size = 0;
			word="";
		}
		else {
			++word_size;
			word += *p;
		}
		++p;
		
	}
	
	// Exo 14 page 801
	int max_frequency = 0;
	
	// Version avec l'utilisation de l'algorithme STL max_element
	map<string, int>::iterator result;
	result = max_element(words.begin(), words.end(), [](const pair<string, int>& a, const pair<string, int>& b) {return a.second < b.second; });
	max_frequency = result->second; word_most_present = result->first;
	
	// Version classique : même résultat
	// for (const auto& p : words) {if (p.second > max_frequency) max_frequency = p.second; word_most_present = p.first;}
	
	return make_tuple(nb_words, max_frequency, word_most_present);
}

int alphabetic_words_sized(Document& d, int size)
{
	// Fonction Helper qui retourne le nombre de mots alphabétiques dans un document d'une taille passée en argument : exo 14 page 801
	int nb_words = 0;
	int word_size = 0;
	auto p = d.begin();

	while (p != d.end()) {
		if (!isalpha(static_cast<unsigned char>(*p)) && word_size > 0) // On teste la fin d'un mot comme un caractère non alphabétique (pas le début...)
		{
			if (word_size==size) ++nb_words;
			word_size = 0;
		}
		else {
			++word_size;

		}
		++p;

	}
	return nb_words;
}

string last_string(const vector<string>& v)
{
	// Exo n°7 page 754 : renvoit la dernière chaîne d'un vecteur non trié
	
	string s{};
	string high{};
	for (auto p = v.begin(); p != v.end(); ++p)
		if (s < *p) { high = *p; s = *p; }
	return high;

}

void copy_list_vector(const list<int>& l)
{
	// Exo n°11 page 755
	vector <double> v{};
	for (auto p = l.begin(); p != l.end(); ++p)
		v.push_back(*p);
	sort(v.begin(), v.end());
	for (auto x : v)
		cout << x << ',';
	cout << endl;
	
}

template<typename Iterator>
Iterator highest_value(Iterator first, Iterator last)
{

	Iterator high = first;
	for (Iterator p = first; p != last; ++p)
		if (*high < *p) high = p;
	return high;

}

template<typename T>
void container_speed(T& c)
{  
	// Exo 20 page 755 : vérification de la vitesse d'insertion de N éléments (= limite) dans un container
	// Les types vector et list possèdent les mêmes fonctions, donc on peut facilement templatiser : push_back, insert, etc.
	// NB : tester en mode Release car les informations de DEBUG ralentissent beaucoup le test
	
	const int limite = 5000;
	srand((int)time(0));	// Seeding du moteur de génération aléatoire en partant de l'heure
	
	auto start1 = chrono::system_clock::now();
	// On remplit le container en prenant soin de le laisser trié
	for (int i = 0; i<limite; ++i) {

		int a = random_number(0, limite);

		// On recherche la 1ère valeur immédiatement supérieure au tirage aléatoire
		auto upper = upper_bound(c.begin(), c.end(), a);

		// Si aucune valeur n'est trouvée, on insère la valeur à la fin du vecteur (vrai quand le vecteur est vide)
		if (upper == c.end())
			c.push_back(a);
		else
			c.insert(upper, a);

	}

	auto end1 = chrono::system_clock::now();

	chrono::duration<double> diff1 = end1 - start1;
	cout << "Time to fill the sorted vector of " << size(c) << " ints : " << diff1.count() << " s" << endl;
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
		
		
		// 20.6.2 : on ouvre un fichier afin de créer un document
		Document texte;

		ifstream ist = open_file_read("masterminds.txt");
		
		while (!ist.eof()) {					// Tant que la fin du fichier n'est pas atteinte, on continue à extraire des caractères
			for (char ch; ist.get(ch);) {
				texte.line.back().push_back(ch);
				if (ch == '\n')
					texte.line.push_back(Line{});

			}
			if (texte.line.back().size()) texte.line.push_back(Line{});

		}
		
		ist.close();							// Fermeture du fichier

		auto bs = find_txt(texte.begin(),texte.end(),"Bjarne Stroustrup");
		if (bs == texte.end())
			cout << "Chaîne introuvable !" << endl;
		else
			cout << "Bjarne Stroustrup a été trouvé !" << endl;
		
		// Exo 6 page 754
		cout << "Le mot memory a été remplacé " << find_and_replace(texte, "memory", "RAM") << " fois par le mot RAM" << endl;
		
		// Exo 8 à 10 page 754/755
		auto s = stats(texte);
		cout << "Le texte compte " << get<0>(s) << " caractères et "<<get<1>(s)<<" lignes" << endl;
		
		auto w = words_whitespaced(texte);
		cout << "Le texte contient " << get<0>(w) << " mots séparés par un espace, dont le plus long est "<< get<1>(w) << endl;
		
		auto z = alphabetic_words(texte);
		cout << "Le texte contient " << get<0>(z) << " mots alphabétiques, dont "<< alphabetic_words_sized(texte, 8) <<" de 8 caractères ; le mot "
			<< get<2>(z) <<" apparait le plus souvent ("<< get<1>(z)<< " fois)"<< endl;
		
		cout << "Le texte contient " << words_specific_whitespaced(texte) << " mots séparés par une ponctuation définie" << endl;
				
		// Exo 13 page 801
		cleanup_punctuation(texte);
		cleanup_minuscules(texte);
		cleanup_plurals(texte);
		print(texte);
				
		vector<string> s1{ "David", "Charles", "André", "Max", "Zoulou", "Louis", "Anabelle", "Xavier", "Zoé" };
		cout << "La chaîne la plus 'grande' dans le vecteur s1 non trié est "<<last_string(s1) << endl;

		// Exo 11 page 755
		list<int> l1 = { 2, 10, 7, 25, 44, 15, 68, 6, 99, 12 };
		copy_list_vector(l1);

		// Exo 12 page 755
		list<int>::iterator p = highest_value(l1.begin(), l1.end());
		cout << "La plus grande valeur de la liste l1 est "<<*p<<endl;
				
		// Exo 20 page 755
		// Conclusion : le vecteur est toujours plus rapide, quelle que soit a priori le nombre d'éléments à générer avec l'utilisation des algos de la STL
		vector <int> exp1{ };
		list<int> exp2{};
		container_speed(exp1);
		container_speed(exp2);
			
		keep_window_open();

		return 0;
	}
	

	catch (runtime_error& e)
	{	// this code is to produce error messages
		cout << e.what() << '\n';
		keep_window_open("q");	// For some Windows(tm) setups
	}

	catch (out_of_range)
	{	// this code is to produce error messages
		cout << "Bad index !" << '\n';
		keep_window_open("q");	// For some Windows(tm) setups
	}
}
