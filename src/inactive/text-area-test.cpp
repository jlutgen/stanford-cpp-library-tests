#include "gtextarea.h"
#include "gwindow.h"
#include "console.h"

using namespace std;

int main() {
    GWindow gw(500, 500);
    GTextArea ta(300, 50);
    ta.setText("lsdkjflkdsf\nlksdflskdjf\nlksdjflsdkfjsdlfk.");
    gw.addToRegion(&ta, "NORTH");
    cout << "Done." << endl;
    return 0;
}


