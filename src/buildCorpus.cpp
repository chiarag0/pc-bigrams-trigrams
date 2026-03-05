#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 4) {
        cout << "Uso: ./buildCorpus <input.txt> <output.txt> <repliche>" << endl;
        return 1;
    }

    string inputFile  = argv[1];
    string outputFile = argv[2];
    int replicas      = stoi(argv[3]);

    ifstream input(inputFile);
    if (!input.is_open()) {
        cout << "Error opening " << inputFile << endl;
        return 1;
    }

    string content((istreambuf_iterator<char>(input)),
                    istreambuf_iterator<char>());
    input.close();

    ofstream output(outputFile);
    if (!output.is_open()) {
        cout << "Error opening " << outputFile << endl;
        return 1;
    }

    for (int i = 0; i < replicas; i++)
        output << content;

    cout << "Done: " << replicas << "x -> " << outputFile << endl;
    return 0;
}