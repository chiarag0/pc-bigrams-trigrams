#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <stdexcept>
#include <vector>
#include <sstream>
#include "CharNgrams.h"
#include "WordNgrams.h"
#include "Benchmark.h"
#include "Validation.h"

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

    system("mkdir -p ../results");
    ofstream csv("../results/benchmark.csv");
    csv << "type,n,threads,schedule,chunk,wall_mean,wall_std,wall_min,wall_max,cpu_mean,speedup\n";

    // sequential baselines
    vector<int> histogram;
    unordered_map<string,int> wordHistogram;

    cout << "\n=== Sequential baselines ===" << endl;
    double seqChar2 = meanSeqChar(text, 2, histogram);
    double seqChar3 = meanSeqChar(text, 3, histogram);
    double seqWord2 = meanSeqWord(words, 2, wordHistogram);
    double seqWord3 = meanSeqWord(words, 3, wordHistogram);

    // parallel char benchmark
    cout << "\n=== Char n-gram parallel benchmark ===" << endl;
    for (int threads : {1, 2, 4, 8, 14, 16}) {
        runCharBenchmark(text, 2, threads, seqChar2, csv);
        runCharBenchmark(text, 3, threads, seqChar3, csv);
    }

    // parallel word benchmark
    cout << "\n=== Word n-gram parallel benchmark ===" << endl;
    for (int threads : {1, 2, 4, 8, 14, 16}) {
        for (int sched : {0, 1, 2}) {
            for (int chunk : {0, 64, 512, 4096}) {
                if (sched == 2 && chunk != 0) continue;
                runWordBenchmark(words, 2, threads, sched, chunk, seqWord2, csv);
                runWordBenchmark(words, 3, threads, sched, chunk, seqWord3, csv);
            }
        }
    }

    csv.close();
    cout << "\nResults saved to ../results/benchmark.csv" << endl;

    return 0;
}