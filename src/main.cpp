#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <stdexcept>
#include <vector>
#include <sstream>
#include "CharNgrams.h"

using namespace std;

/*
Process the entire file in memory instead of streaming
or using intermediate files in order to reduce I/O overhead
and improve cache locality.
*/
string cleanText(const string& filename) {
    ifstream input(filename, ios::binary);
    if (!input.is_open()) {
        throw runtime_error("Error opening file");
    }

    // Read the entire file into a string
    string text(
        (istreambuf_iterator<char>(input)),
        istreambuf_iterator<char>()
    );

    string result;
    result.reserve(text.size());  // reserve space to avoid multiple reallocations

    bool last_was_space = true;  // avoid multiple spaces

    for (char c : text) {
        unsigned char uc = static_cast<unsigned char>(c); // handle signed char as following functions expect unsigned char

        if (uc > 127) continue;

        if (ispunct(uc) || isspace(uc)) {  // punctuation and whitespace are 
            if (!last_was_space) {  // avoid double spaces
                result += '_';
                last_was_space = true;
            }
        }
        else if (isalpha(uc)) {
            result += static_cast<char>(tolower(uc)); // convert to lowercase
            last_was_space = false;
        }
        else if (isdigit(uc)) { // keep digits
            result += c;
            last_was_space = false;
        }
    }

    return result;
}

vector<string> getWords(const string& text) {
    vector<string> words;
    stringstream ss(text);
    string word;
    while (getline(ss, word, '_')) {  // split on '_'
        if (!word.empty())
            words.push_back(word);
    }
    return words;
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <input.txt>" << endl;
        return 1;
    }

    string text = cleanText(argv[1]);
    vector<string> words = getWords(text);

    cout << "N. of characters: " << text.size() << endl;
    cout << "N. of words:      " << words.size() << endl;

    vector<int> histogram;

    double t = extractCharNgrams(text, 2, histogram);
    cout << "\nTime for BIGRAMS of characters: " << t << "s" << endl;
    printTopKChar(histogram, 2, 3);

    t = extractCharNgrams(text, 3, histogram);
    cout << "\nTime for TRIGRAMS of characters: " << t << "s" << endl;
    printTopKChar(histogram, 3, 3);

    return 0;
}