#include "lexicon.h"
#include "console.h"

using namespace std;

int main() {
    Lexicon lex("EnglishWords.txt");
    cout << "size: " << lex.size() << endl;

    return 0;
}


