#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include "Web_Graph.h"
using namespace std;

void ReadGraph(string name, Graph& websites) {
	ifstream file(name);
	string str;
	while (!file.eof()) {
		file >> str;
		int i = 0;
		while (str[i] != ',') { i++; }
		websites.addEdge(str.substr(0, i), str.substr(i + 1, str.size() - i));
	}
	file.close();
}

void ReadKeyword(string name, Graph& websites) {
	ifstream file(name);
	string str;
	while (!file.eof()) {
		file >> str;
		websites.add_keywords(str);
	}
	file.close();
}

void ReadImpressions(string name, Graph& websites) {
	ifstream file(name);
	string str;
	while (!file.eof()) {
		getline(file, str, ',');
		Node* node = websites.vertix(str);
		getline(file, str, '\n');
		node->impressions = stoi(str);
	}
	file.close();
}

void ReadClicks(string name, Graph& websites) {
	cout << "here";
	ifstream file(name);
	string str;
	while (!file.eof()) {
		getline(file, str, ',');
		Node* node = websites.vertix(str);
		getline(file, str, '\n');
		node->clicks = stod(str);
		websites.update_CTR(node->index);
	}
	file.close();
}


void quotation_OR_space(Graph& websites, vector<string>& search, vector<int>& output) {
	vector<int> ind = websites.mapping();
	for (int i = 0; i < websites.V_(); ++i)
		for (int j = 0; j < websites.vertix(ind[i])->keywords.size(); ++j)
			for (int s = 0; s < search.size(); ++s)    //This loop is because the user can search for more than 2 words
				if (search[s] == websites.vertix(ind[i])->keywords[j]) {
					output.push_back(ind[i]);
					websites.displayed(ind[i]);
					s = search.size(); j = websites.vertix(ind[i])->keywords.size(); 
					//This line is for once I found the keyword in a website, I will check the next node
				}
}

void AND(Graph& websites, vector<string>& search, vector<int>& output) {
	vector<int> ind = websites.mapping();
	int count = 0;
	for (int i = 0; i < websites.V_(); ++i) {
		count = 0;
		for (int s = 0; s < search.size(); ++s) {
			for (int j = 0; j < websites.vertix(ind[i])->keywords.size(); ++j)
				if (search[s] == websites.vertix(ind[i])->keywords[j])
					count++;

			if (count == search.size()) {
				output.push_back(ind[i]);
				websites.displayed(ind[i]);
			}
		}
	}
}

void read_OR_AND(Graph& websites, string input, vector<string>& search, vector<int>& output, bool OR) {
	int n; string str1, str2;
	if (OR) { n = 4; str1 = " OR "; str2 = " or "; }
	else { n = 5; str1 = " AND "; str2 = " or "; }
	int start = 0;
	for (int i = 0; i < input.size() - (n - 1) && input.size()>n; ++i) 
		if (input.substr(i, n) == str1|| input.substr(i, n) == str2) {
			search.push_back(input.substr(start, i - start));
			start = i + n;
			i += n;
		}
	
	if (start) 
		search.push_back(input.substr(start, input.size() - start));
	
	if (OR) 
		quotation_OR_space(websites, search, output);
	else 
		AND(websites, search, output);
	
}

void read_space(Graph& websites, string input, vector<string>& search, vector<int>& output) {
	int start = -1;
	for (int i = 0; i < input.length(); ++i) {
		if (input[i] != ' ' && (i == 0 || input[i - 1] == ' ')) {
			start = i;
		}

		if (input[i] == ' ' && start != -1) {
			search.push_back(input.substr(start, i - start));
			start = -1;
		}
	}
	if (start != -1)
		search.push_back(input.substr(start, input.length() - start));
	if (!search.size())
		search.push_back(input);

	quotation_OR_space(websites, search, output);
}



int main() {
	Graph websites;
	ReadGraph("Web graph file.txt", websites);
	//websites.function(); 

	ReadKeyword("Keyword file.txt", websites);
	//websites.keywords();

	ReadImpressions("Number of impressions file.txt", websites);
	//websites.impressions();

	ReadClicks("clicks.txt", websites);

	websites.calculate_PR();

	//

	cout << "Welcome!\nWhat would you like to do?\n1. New search\n2. Exit\n\nType in your choice: ";
	int c;
	cin >> c;
	if (c == 2) exit(1);

	string input;
	vector<string> search;       //stores the keywords the user is searching for
	vector<int> output;          //stores the indeces of the webpages 
	while (c <= 3 && c >= 1) {

		cout << "Search for: ";
		getline(cin >> ws, input);   //getline(cin, input);

		if (input[0] == '"') { //&& input[input.size() - 1] == '"'
			search.push_back(input.substr(1, input.size() - 2));
			quotation_OR_space(websites, search, output);
		}
		else read_OR_AND(websites, input, search, output, true);  //check OR

		if (!search.size())
			read_OR_AND(websites, input, search, output, false);   //check AND

		if (!search.size())
			read_space(websites, input, search, output);        //plain search string

		cout << "\nSearch results:\n";
		if (!output.size()) {
			cout << "There are no results for your search\n\nWhat would you like to do?\n1. New search\n2. Exit\n\nType in your choice: ";
			cin >> c;
			if (c == 2)
				exit(1);
			continue;
		}
		int order = 1;
		for (int i = 0; i < output.size(); ++i) {
			cout << order++ << ". " << websites.vertix(output[i])->link << endl;
		}
		cout << "\nWhat would you like to do?\n1. Open a webpage\n2. New search\n3. Exit\n\nType in your choice: ";
		cin >> c;
		//
		

		while (c <= 3 && c >= 1) {
			if (c == 1) {
				int order = 1;
				for (int i = 0; i < output.size(); ++i) {
					cout << order++ << ". " << websites.vertix(output[i])->link << endl;
				}
				cout << "\nChoose the number of the webpage: ";
				cin >> c; c--;
				if (c >= output.size()) {   //out of range choice
					cout << "This number is out of range, please try again.\n";
					c = 1;                  //to enter the condition of the webpages
					continue;
				}
				websites.clicked(output[c]);
				cout << "\nYou are now viewing " << websites.vertix(output[c])->link << ".\nWhat would you like to do?\n1. Back to search results\n2. New search\n3. Exit\n\nType in your choice: ";
				cin >> c;
				while (c > 3) {
					cout << "This number is out of range, please try again.\n";
					cin >> c;
				}
				continue;
			}
			if (c == 2) {
				search.clear(); output.clear();
				websites.mergeSort(0, websites.V_() - 1);
				break;
			}
			if (c == 3)
			{
				ofstream fele("clicks.txt");
				for (int i = 0; i < websites.V_(); ++i) {
					fele << websites.vertix(i)->link << "," << websites.vertix(i)->clicks;
					if (i != websites.V_() - 1)
						fele << endl;
				}
				fele.close();

				fele.open("Number of impressions file.txt");
				for (int i = 0; i < websites.V_(); ++i) {
					fele << websites.vertix(i)->link << "," << websites.vertix(i)->impressions;
					if (i != websites.V_() - 1)
						fele << endl;
				}
				fele.close();
				exit(1);
			}
		}
	}
	
	return 0;
}

//The keywords could have been sorted to search for them in binary search