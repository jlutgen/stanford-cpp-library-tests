#ifndef STEGOVIEWER_H
#define STEGOVIEWER_H

#include "gobjects.h"
#include "gbufferedimage.h"
#include "gtextarea.h"
#include "ginteractors.h"
#include "grid.h"
#include "gtypes.h"

using namespace std;


typedef enum {
    IMAGE_MODE = 0,
    TEXT_MODE  = 1
} ViewerMode;

typedef enum {
    CARRIER = 1,
    PAYLOAD = 2,
    PACKAGE = 4
} PanelMode;

struct ImagePair {
    GBufferedImage *orig;   // full-size version
    GBufferedImage *scaled; // scaled version that fits in pane
};

class StegoViewer {

public:

    StegoViewer(); // 85% of screen width by 80% of screen height
    StegoViewer(int width, int height);
    ~StegoViewer();
    void run();

private:

    void createWidgets();
    void switchMode();
    void createPackage();
    void extractPayload();
    void updatePhoto(int i);
    void loadCarrier();
    void loadPayload();
    void loadPackage();
    bool loadImage(int i, string title);
    void loadText();
    void saveText();
    void savePackage();
    void saveExtracted();
    void createBottomGrid(int cellHeight);
    void addToGrid(GObject *gobj, int row, int col);
    void updateButtons();
    void clearPanel(int i);
    void showDiff();
    string stateToString();
    bool occupied(int i);
    void setOccupied(int i, bool occupied);
    void setBits();
    void startPleaseWait(int i);
    void endPleaseWait();

    int width;
    int height;
    int canvasHeight;
    int imagePaneY;
    int paneHeight;
    Vector<GPoint> paneLocations;
    int paneWidth;
    int bits;
    string mode;
    int state;
    GWindow *gw;
    Vector<GLabel *> labels;
    Vector<ImagePair> imagePairs;

    GTextArea *textArea;
    GButton *leftLoadButton;
    GButton *centerLoadButton;
    GButton *rightLoadButton;
    GButton *centerSaveButton;
    GButton *rightSaveButton;
    GButton *encryptButton;
    GButton *extractButton;
    GButton *loadTextButton;
    GButton *saveTextButton;
    GButton *diffButton;
    GChooser *modeChooser;
    GChooser *bitsChooser;
    GLabel *bitsLabel;
    GLabel *modeLabel;
    GLabel *messageLabel;
    Grid<GRectangle> bottomGrid;
    GCompound *buttonCompound;
    string savedLabelText;
};

#endif // STEGOVIEWER_H
