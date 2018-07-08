#include <string>
#include <fstream>
#include <iostream>
//#include <Windows.h>
#include <map>
#include <algorithm>
#include <set>
#include <chrono>
//#include <ctime>
//#include <locale>
//#include <codecvt>


//#include <fcntl.h>
//#include <io.h>
//#include <wincon.h>

using namespace std;


#define ISRUS(a) (((a >= 'а') && (a <= 'я')) || ((a >= 'А') && (a <= 'Я')) || (a == 'ё') || (a == 'Ё'))

bool LoadWordsFromFile(map<size_t, set<string>>& LoadMap, size_t& maxLenWord, const string& filename);
size_t AddToDictionary(const map<size_t, set<string>>& MapFrom, map<size_t, set<string>>& Dictionary);


int main() {
	setlocale(LC_ALL, "Russian");
	
	
	//_setmode(_fileno(stdout), _O_U16TEXT);
	//SetConsoleOutputCP(CP_UTF8);
	//_locale_t rus = _create_locale(LC_CTYPE, ".1251");
	
	string filename_in, filename_dict;
	//size_t numWords;
	cout << "Название файла с текстом: ";
	cin >> filename_in;
	cout << "Название файла со словарем: ";
	cin >> filename_dict;
	/*cout << "Кол-во слов: ";
	cin >> numWords;*/

	map<size_t, set<string>> MM, Dictionary;

	//загрузка слов из 1-го файла
	size_t maxWordLen = 0, maxWordLenDict = 0;
	auto start_read = std::chrono::steady_clock::now();
	if (!LoadWordsFromFile(MM, maxWordLen, filename_in)) {
		cout << "Ошибка чтения файла \"" << filename_in << "\"!" << endl;
		return 1;
	}
	auto finish_read = std::chrono::steady_clock::now();
	auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(finish_read - start_read);
	cout << "Файл \"" << filename_in << "\" прочитан: " << elapsed_ms.count() << " ms" << endl;

	//загрузка слов из словаря
	start_read = std::chrono::steady_clock::now();
	LoadWordsFromFile(Dictionary, maxWordLenDict, filename_dict);
	finish_read = std::chrono::steady_clock::now();
	elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(finish_read - start_read);
	cout << "Файл \"" << filename_dict << "\" прочитан: " << elapsed_ms.count() << " ms" << endl;

	//добавление всех слов в словарь
	size_t AddedCounter = 0;
	start_read = std::chrono::steady_clock::now();
	AddedCounter = AddToDictionary(MM, Dictionary);
	finish_read = std::chrono::steady_clock::now();
	elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(finish_read - start_read);
	cout << "Слова добалены в контейнер словаря: " << elapsed_ms.count() << " ms" << endl;
	cout << "В словарь добавлено " << AddedCounter << " слов" << endl;

	//перезапись словаря
	start_read = std::chrono::steady_clock::now();
	ofstream fout(filename_dict);
	if (!fout.is_open()) {
		cout << "Ошибка открытия файла \"" << filename_dict << "\"!" << endl;
		return 1;
	}
	for (auto it = Dictionary.begin(); it != Dictionary.end(); it++) {
		for (auto it_set = it->second.begin(); it_set != it->second.end(); it_set++) {
			fout << *it_set << endl;
		}
	}
	fout.close();
	finish_read = std::chrono::steady_clock::now();
	elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(finish_read - start_read);
	cout << "Словарь перезаписан: " << elapsed_ms.count() << " ms" << endl;
	
	return 0;
}


//загружает в LoadMap слова из файла, в maxLenWord записывает длину нибольшего слова
bool LoadWordsFromFile(map<size_t, set<string>>& LoadMap, size_t& maxLenWord, const string& filename) {
	ifstream fin(filename);
	fin.unsetf(ios::skipws);
	if (!fin.is_open()) return false;

	LoadMap.clear();
	//добавление всех слов
	fin.seekg(0, ios::beg);

	size_t maxLen = 0;
	char c, c_ = ' ';
	string str = "";
	while (true) {
		fin >> c;
		if (fin.eof()) {
			if (str.length() > 0) {
				if (LoadMap.find(str.length()) == LoadMap.end()) {
					set<string> tm_set;
					tm_set.insert(str);
					LoadMap.insert(pair<size_t, set<string>>(str.length(), tm_set));
				}
				else {
					LoadMap[str.length()].insert(str);
				}
				if (str.length() > maxLen) maxLen = str.length();
			}
			break;
		}

		if (ISRUS(c)) {
			str += (char)tolower((char)c);
		}
		else if (ISRUS(c_)) {
			//cout << str << endl;
			if (LoadMap.find(str.length()) == LoadMap.end()) {
				set<string> tm_set;
				tm_set.insert(str);
				LoadMap.insert(pair<size_t, set<string>>(str.length(), tm_set));
			}
			else {
				LoadMap[str.length()].insert(str);
			}
			if (str.length() > maxLen) maxLen = str.length();
			str = "";
		}
		c_ = c;

		

	}
	fin.close();
	maxLenWord = maxLen;
	return true;
}

//добавляет слова из MapFrom в Dictionary без повторений
//возвращает кол-во добаленных слов
size_t AddToDictionary(const map<size_t, set<string>>& MapFrom, map<size_t, set<string>>& Dictionary) {
	size_t AddedCounter = 0;
	for (auto it = MapFrom.begin(); it != MapFrom.end(); it++) {
		for (auto it_set = it->second.begin(); it_set != it->second.end(); it_set++) {
			if (Dictionary.find(it->first) != Dictionary.end()) {
				size_t check_size = Dictionary[it->first].size();
				Dictionary[it->first].insert(*it_set);
				if (check_size != Dictionary[it->first].size()) AddedCounter++;
			}
			else {
				//не добавлен сет с текущей длиной
				set<string> tm_set;
				tm_set.insert(*it_set);
				Dictionary.insert(pair<size_t, set<string>>(it_set->length(), tm_set));
				AddedCounter++;
			}
		}
	}
	return AddedCounter;
}


