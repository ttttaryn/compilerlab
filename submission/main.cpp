#include "utils.h"

vector<Token> tokens;
Token thisToken;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input_file>\n";
        return 1;
    }

    // cout << "entering lexical analysis...\n" << endl;
    analyze_lexical(argv[1]);

    // cout << "initializing parser...\n" << endl;
    Parser parser(tokens);

    // cout << "parsing...\n" << endl;
    parser.parse();

    // cout << "print mips code...\n" << endl;
    cout << parser.getMIPSCode() << endl;
    
    return 0;
}
