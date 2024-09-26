#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <optional>

using namespace std;

#include "tokenizer.h"
#include "parser.h"
#include "generator.h"

int main(int argc, char** args) {

    if (argc < 2) {
        cerr << "Incorrect usage! Correct usage is: " << endl << args[0] << " <filename>" << endl;
        return EXIT_FAILURE;
    }

    string content;
    {
        stringstream sContent;
        fstream input(args[1], ios::in);
        sContent << input.rdbuf();
        content = sContent.str();
    }

    Tokenizer tokenizer(content);
    vector<Token> tokens = tokenizer.tokenize();

    Parser parser(tokens);
    Node::Program root = parser.parse();

    Generator generator(root);

    {
        fstream file("../out.asm", ios::out);
        file << generator.generate();
    }

    return EXIT_SUCCESS;

}