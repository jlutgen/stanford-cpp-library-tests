#include <fstream>
#include <cmath>
#include <cctype>
#include <sstream>
#include <iomanip>
#include "stegoViewer.h"
#include "gevents.h"
#include "filelib.h"
#include "goptionpane.h"
#include "stego.h"

using namespace std;


static string topLabelPrefix[3] = { "Carrier: ", "Payload: ", "Package: " };
static const int kButtonPadding = 1;
static const int kPanePadding = 2;
static const string kTextAreaColor = "#f2f2f2"; // very light gray
static const string kPanelGray = "0xffeeeeee"; // same as JPanel background

static GBufferedImage *sizer(const GBufferedImage *im, int maxWidth, int maxHeight);


StegoViewer::StegoViewer() :
    StegoViewer(0.85 * getScreenWidth(), 0.80 * getScreenHeight()) {
}

StegoViewer::StegoViewer(int width, int height) {
    this->width = width;
    this->height = height;
    gw = new GWindow(width, height, false);
    bits = 3;
    mode = "Image"; // will be switched to Text before user sees
    state = 0;
    cout << stateToString() << endl;
    gw->setWindowTitle("The Steganator");
    createWidgets();
    switchMode();
}

StegoViewer::~StegoViewer() {
    delete gw;
}

void StegoViewer::run() {
    gw->setVisible(true);
    setConsoleLocation(10, gw->getHeight() + 40);
    setConsoleSize(500, 100);
    while (true) {
          GActionEvent e = waitForEvent(ACTION_EVENT);
          if (!e.isValid()) // perhaps a window closed
              continue;
          string cmd = e.getActionCommand();
          cout << "Action command: " << cmd << endl;

          if (cmd == "load 0") {
              loadCarrier();
          } else if (cmd == "load 1") {
              loadPayload();
          } else if (cmd == "load 2") {
              loadPackage();
          } else if (cmd == "extract") {
              extractPayload();
          } else if (cmd == "encrypt") {
              createPackage();
          } else if (cmd == "save 1") {
              saveExtracted();
          } else if (cmd == "save 2") {
              savePackage();
          } else if (cmd == "load text") {
              loadText();
          } else if (cmd == "save text") {
              saveText();
          } else if (cmd == "diff") {
              showDiff();
          } else if (cmd == "switch mode") {
              switchMode();
          } else if (cmd == "set bits") {
              setBits();
          }
       }
}

void StegoViewer::createWidgets() {
    modeLabel = new GLabel("Payload type:");
    modeChooser = new GChooser();
    modeChooser->setActionCommand("switch mode");
    modeChooser->addItem("Image");
    modeChooser->addItem("Text");
    modeChooser->setSelectedItem("Text");

    bitsLabel = new GLabel("Bits per pixel used for encoding:");
    bitsChooser = new GChooser();
    bitsChooser->setActionCommand("set bits");
    for (int i=1; i<=8; i++) {
        bitsChooser->addItem(integerToString(i));
    }
    bitsChooser->setVisible(false);
    bitsChooser->setSelectedItem("3");
    bits = 3;

    GLabel *spacerLabel = new GLabel("               ");
    gw->setRegionAlignment("north", "left");
    gw->addToRegion(modeLabel, "north");
    gw->addToRegion(modeChooser, "north");
    gw->addToRegion(spacerLabel, "north");
    gw->addToRegion(bitsLabel, "north");
    gw->addToRegion(bitsChooser, "north");


    for (string s : topLabelPrefix)
        labels.add(new GLabel(s));
    savedLabelText = labels[1]->getLabel();
    for (int i=0; i<3; i++) {
        gw->add(labels[i], i*width/3.0 + kPanePadding, labels[0]->getFontAscent() + kPanePadding);
    }
    imagePaneY = labels[0]->getFontAscent() + 2*kPanePadding + labels[0]->getFontDescent();
    for (int i=0; i<3; i++) {
        paneLocations.add(GPoint(i*width/3 + kPanePadding, imagePaneY));
    }

    leftLoadButton = new GButton("Load image...");
    leftLoadButton->setActionCommand("load 0");
    centerLoadButton = new GButton("Load image...");
    centerLoadButton->setActionCommand("load 1");
    rightLoadButton = new GButton("Load image...");
    rightLoadButton->setActionCommand("load 2");
    encryptButton = new GButton("Generate package ->");
    encryptButton->setActionCommand("encrypt");
    centerSaveButton = new GButton("Save image...");
    centerSaveButton->setActionCommand("save 1");
    rightSaveButton = new GButton("Save image...");
    rightSaveButton->setActionCommand("save 2");
    extractButton = new GButton("<- Extract payload");
    extractButton->setActionCommand("extract");
    loadTextButton = new GButton("Load text...");
    loadTextButton->setActionCommand("load text");
    saveTextButton = new GButton("Save text...");
    saveTextButton->setActionCommand("save text");
    diffButton =new GButton("Show carrier/package diff");
    diffButton->setActionCommand("diff");
    int cellHeight = leftLoadButton->getHeight() + kButtonPadding;
    createBottomGrid(cellHeight);
    buttonCompound = new GCompound();

    addToGrid(leftLoadButton, 0, 0);
    addToGrid(rightLoadButton, 0, 2);
    addToGrid(rightSaveButton, 1, 2);
    addToGrid(encryptButton, 2, 1);
    addToGrid(extractButton, 2, 2);
    addToGrid(diffButton, 2, 0);

    canvasHeight = gw->getCanvasHeight();
    buttonCompound->setLocation(0, canvasHeight - 3*cellHeight);
    gw->setColor(kPanelGray); // same as JPanel color
    gw->fillRect(0, canvasHeight - 3*cellHeight, width, 3*cellHeight); // fake panel for buttons
    gw->add(buttonCompound);

    /* Calculate dimensions of image/text panes */
    paneWidth = width/3 - 2*kPanePadding;
    paneHeight = canvasHeight - 3*cellHeight - kPanePadding - imagePaneY;

    /* Draw rectangles around panes */
    gw->setColor("light gray");
    for (int i=0; i<3; i++) {
        gw->drawRect(paneLocations[i].getX()-1, paneLocations[i].getY()-1,
                     paneWidth+2, paneHeight+2);
    }

    /* Initialize image objects */
    for (int i=0; i<3; i++) {
        imagePairs.add(*(new ImagePair));
        imagePairs[i].orig = imagePairs[i].scaled = NULL;
    }

    /* Initialize text area */
    textArea = new GTextArea(paneWidth, paneHeight);
    textArea->setLocation(paneLocations[1]);
    textArea->setBackground(kTextAreaColor);
    textArea->setFont("Monospaced-9");

    /* "Please wait" label */
    messageLabel = new GLabel("Working...please wait");
    messageLabel->setFont("SansSerif-Italic-16");
    messageLabel->setColor("#a00000"); // dark red
}

void StegoViewer::switchMode() {
    string newMode = modeChooser->getSelectedItem();
    if (newMode == mode)
        return;
    mode = newMode;
    if (mode == "Text") {
        bitsLabel->setVisible(false);
        bitsChooser->setVisible(false);
        buttonCompound->remove(centerSaveButton);
        buttonCompound->remove(centerLoadButton);
        addToGrid(loadTextButton, 0, 1);
        addToGrid(saveTextButton, 1, 1);
        if (occupied(1))
            gw->remove(imagePairs[1].scaled);
        gw->add(textArea);
        savedLabelText = labels[1]->getLabel();
        labels[1]->setLabel(topLabelPrefix[1] + " (text)");
        setOccupied(1, true);
    } else {
        bitsLabel->setVisible(true);
        bitsChooser->setVisible(true);
        buttonCompound->remove(loadTextButton);
        buttonCompound->remove(saveTextButton);
        addToGrid(centerLoadButton, 0, 1);
        addToGrid(centerSaveButton, 1, 1);
        gw->remove(textArea);
        if (imagePairs[1].scaled != NULL) {
            gw->add(imagePairs[1].scaled);
            setOccupied(1, true);
        } else {
            setOccupied(1, false);
        }
        labels[1]->setLabel(savedLabelText);
    }
    updateButtons();
    cout << stateToString() << endl;
}
void StegoViewer::createPackage() {
    if (mode == "Image") {
        if (imagePairs[1].orig->getWidth() > imagePairs[0].orig->getWidth() ||
                imagePairs[1].orig->getHeight() > imagePairs[0].orig->getHeight()) {
            GOptionPane::showMessageDialog(gw, "Sorry, the payload is too large for the carrier", "Error", GOptionPane::PLAIN_MESSAGE);
            return;
        }
        clearPanel(2);
        startPleaseWait(2);
        imagePairs[2].orig = encodeImageInImage(imagePairs[1].orig, imagePairs[0].orig, bits);
        imagePairs[2].scaled = sizer(imagePairs[2].orig, paneWidth, paneHeight);
        imagePairs[2].scaled->setLocation(paneLocations[2]);
        endPleaseWait();
        gw->add(imagePairs[2].scaled);
        string label = topLabelPrefix[2] + "(untitled)" + " ("
                + integerToString(imagePairs[2].orig->getWidth()) + "x"
                + integerToString(imagePairs[2].orig->getHeight()) + ")";
        labels[2]->setLabel(label);
    } else {
        GBufferedImage *carrier = imagePairs[0].orig;
        string payload = textArea->getText();
        if (7*(payload.length() + 1) > carrier->getWidth()*carrier->getHeight()) {
            GOptionPane::showMessageDialog(gw, "Sorry, the payload is too large for the carrier", "Error",
                                           GOptionPane::PLAIN_MESSAGE);
            return;
        }
        clearPanel(2);
        startPleaseWait(2);
        pause(200);
        GBufferedImage *im = encodeTextInImage(payload, carrier);
        pause(200);
        endPleaseWait();
        imagePairs[2].orig = im;
        imagePairs[2].scaled = sizer(im, paneWidth, paneHeight);
        imagePairs[2].scaled->setLocation(paneLocations[2]);
        gw->add(imagePairs[2].scaled);
        string label = topLabelPrefix[2] + "(untitled) ("
                + integerToString(imagePairs[2].orig->getWidth()) + "x"
                + integerToString(imagePairs[2].orig->getHeight()) + ")";
        labels[2]->setLabel(label);
    }
    state |= PACKAGE;
    cout << stateToString() << endl;
    updateButtons();
}
void StegoViewer::extractPayload() {
    clearPanel(0);
    clearPanel(1);
    if (mode == "Image") {
        startPleaseWait(1);
        imagePairs[1].orig = decodeImageFromImage(imagePairs[2].orig, bits);
        imagePairs[1].scaled = sizer(imagePairs[1].orig, paneWidth, paneHeight);
        imagePairs[1].scaled->setLocation(paneLocations[1]);
        endPleaseWait();
        gw->add(imagePairs[1].scaled);
        string label = topLabelPrefix[1] + "(untitled)" + " ("
                + integerToString(imagePairs[1].orig->getWidth()) + "x"
                + integerToString(imagePairs[1].orig->getHeight()) + ")";
        labels[1]->setLabel(label);
    } else {
        gw->remove(textArea);
        startPleaseWait(1);
        pause(200);
        string payload = decodeTextFromImage(imagePairs[2].orig);
        pause(200);
        textArea->setText(payload);
        endPleaseWait();
        gw->add(textArea);
    }
    state |= PAYLOAD;
    cout << stateToString() << endl;
    updateButtons();
}

void StegoViewer::updatePhoto(int i) {

}

void StegoViewer::loadCarrier() {
    if (loadImage(0, "Choose original image")) {
        clearPanel(2);
        state |= CARRIER;
        cout << stateToString() << endl;
        updateButtons();
    }
}

void StegoViewer::loadPayload() {
    if (loadImage(1, "Choose image to hide")) {
        clearPanel(2);
        state |= PAYLOAD;
        cout << stateToString() << endl;
        updateButtons();
    }
}
void StegoViewer::loadPackage() {
    if (loadImage(2, "Choose package image")) {
        clearPanel(0);
        clearPanel(1);
        state |= PACKAGE;
        cout << stateToString() << endl;
        updateButtons();
    }
}

bool StegoViewer::loadImage(int i, string title) {
    string filter = "images/*.jpg;*.jpeg;*.png;*.gif;*.tif;*.tiff;*.bmp";
    string filename = openFileDialog(title, filter, "Image files");
    if (filename.empty())
        return false;
    startPleaseWait(i);
    cout << filename << endl;
    if (occupied(i)) {
        gw->remove(imagePairs[i].scaled);
        delete imagePairs[i].orig;
        delete imagePairs[i].scaled;
    }
    imagePairs[i].orig = new GBufferedImage(1, 1);
    imagePairs[i].orig->load(filename);
    imagePairs[i].scaled = sizer(imagePairs[i].orig, paneWidth, paneHeight);
    imagePairs[i].scaled->setLocation(paneLocations[i]);
    endPleaseWait();
    gw->add(imagePairs[i].scaled);
    string label = topLabelPrefix[i] + getTail(filename) + " ("
            + integerToString(imagePairs[i].orig->getWidth()) + "x"
            + integerToString(imagePairs[i].orig->getHeight()) + ")";
    labels[i]->setLabel(label);
    cout << stateToString() << endl;
    return true;
}

void StegoViewer::loadText() {
    string filename = openFileDialog("Choose a plain text file");
    cout << "loadText: filename: " << filename << endl;
    if (filename.empty())
        return;
    gw->remove(textArea);
    messageLabel->setLabel("Working...please wait");
    gw->add(messageLabel, paneLocations[1].getX() + 20, paneLocations[1].getY() + 50);
    string text = readEntireFile(filename);
    gw->remove(messageLabel);
    gw->add(textArea);
    if (text.empty()) {
        GOptionPane::showMessageDialog(gw, "Sorry, the chosen file does not any valid text.", "Error",
                                       GOptionPane::PLAIN_MESSAGE);
        return;
    }
    for (int i = 0; i < text.length(); i++) {
        if ((unsigned) text[i] > 127) {
            GOptionPane::showMessageDialog(gw, "Sorry, the chosen file is not a plain text file.", "Error",
                                           GOptionPane::PLAIN_MESSAGE);
            return;
        }
    }
    textArea->setText(text);
    clearPanel(2);
    state |= PAYLOAD;
    cout << stateToString() << endl;
    updateButtons();
}

void StegoViewer::savePackage() {
    string filename = saveFileDialog("Save image as...", "/*.png", "PNG files");
    if (filename.empty())
        return;
    imagePairs[2].orig->save(filename);
    string label = topLabelPrefix[2] + getTail(filename) + " ("
            + integerToString(imagePairs[2].orig->getWidth()) + "x"
            + integerToString(imagePairs[2].orig->getHeight()) + ")";
    labels[2]->setLabel(label);
}

void StegoViewer::saveExtracted() {
    string filename = saveFileDialog("Save image as...", "/*.png", "PNG files");
    if (filename.empty())
        return;
    imagePairs[1].orig->save(filename);
    string label = topLabelPrefix[1] + getTail(filename) + " ("
            + integerToString(imagePairs[1].orig->getWidth()) + "x"
            + integerToString(imagePairs[1].orig->getHeight()) + ")";
    labels[1]->setLabel(label);
}

void StegoViewer::saveText() {
    ofstream ofs;
    string filename = saveFileDialog("Save text as");
    cout << "saveText: filename: " << filename << endl;
    if (filename.empty())
        return;
    if (fileExists(filename)) {
        int response = GOptionPane::showConfirmDialog(gw, "File exists. Overwrite? Current file contents will be erased.",
                                                      "", GOptionPane::YES_NO);
        if (response != GOptionPane::YES)
            return;
    }
    ofs.open(filename.c_str());
    if (ofs.fail()) {
        GOptionPane::showMessageDialog(gw, "File could not be opened for writing", "Error", GOptionPane::PLAIN_MESSAGE);
        return;
    }
    ofs << textArea->getText();
    ofs.close();
}

void StegoViewer::createBottomGrid(int cellHeight) {
    // just a grid of abstract rectangles for layout purposes
    bottomGrid.resize(3, 3);
    int cellWidth = width / 3;
    for (int col = 0; col < 3; col++)
        for (int row = 0; row < 3; row++) {
            GRectangle *rect = new GRectangle(col*cellWidth, row*cellHeight, cellWidth, cellHeight);
            bottomGrid[row][col] = *rect;
        }
}

void StegoViewer::addToGrid(GObject *gobj, int row, int col) {
    // center object within cell and add it to window
    int objWidth = gobj->getWidth();
    int objHeight = gobj->getHeight();
    GRectangle cell = bottomGrid[row][col];
    gobj->setLocation(cell.getX() + (cell.getWidth() - objWidth) / 2,
                      cell.getY() + (cell.getHeight() - objHeight) / 2);
    buttonCompound->add(gobj);
}

void StegoViewer::updateButtons() {
    encryptButton->setEnabled(state & CARRIER && state & PAYLOAD);
    extractButton->setEnabled(state & PACKAGE);
    rightSaveButton->setEnabled(state & PACKAGE);
    centerSaveButton->setEnabled(state & PAYLOAD);
    diffButton->setEnabled(state & CARRIER && state & PACKAGE);
}

void StegoViewer::clearPanel(int i) {
    if (mode == "Text") {
        if (i == 1) {
            textArea->setText("");
        } else {
            if (occupied(i)) {
                gw->remove(imagePairs[i].scaled);
                delete imagePairs[i].orig;
                delete imagePairs[i].scaled;
                imagePairs[i].orig = NULL;
                imagePairs[i].scaled = NULL;
            }
            labels[i]->setLabel(topLabelPrefix[i]);
            setOccupied(i, false);
        }
    } else {
        if (occupied(i)) {
            gw->remove(imagePairs[i].scaled);
            delete imagePairs[i].orig;
            delete imagePairs[i].scaled;
            imagePairs[i].orig = NULL;
            imagePairs[i].scaled = NULL;
        }
        labels[i]->setLabel(topLabelPrefix[i]);
        setOccupied(i, false);
    }
    cout << stateToString() << endl;
}

void StegoViewer::showDiff() {
    GBufferedImage *left = imagePairs[0].orig;
    GBufferedImage *right = imagePairs[2].orig;
    GBufferedImage *diff = left->diff(*right);
    int w = diff->getWidth();
    int h = diff->getHeight();
    GLabel *keyLabel1 = new GLabel("Identical:");
    GLabel *keyLabel2 = new GLabel("Different:");
    keyLabel1->setFont("SansSerif-10");
    keyLabel2->setFont("SansSerif-10");
    keyLabel1->setLocation(0, keyLabel1->getHeight());
    keyLabel2->setLocation(0, keyLabel2->getHeight() + keyLabel1->getHeight());
    GCompound *comp = new GCompound();
    comp->add(keyLabel1);
    comp->add(keyLabel2);
    GRect *keyRect1 = new GRect(5, 5);
    GRect *keyRect2 = new GRect(5, 5);
    keyRect1->setFilled(true);
    keyRect2->setFilled(true);
    keyRect1->setColor("black");
    keyRect2->setColor("#dd00dd");
    keyRect1->setFillColor("black");
    keyRect2->setFillColor("#dd00dd");
    GLabel *numDiffLabel = new GLabel("");
    ostringstream oss;
    int differences = left->countDiffPixels(*right);
    double totalPixels = left->getWidth() * left->getHeight();
    oss << "Different/Total: " << differences << "/" << (int) totalPixels;
    oss << " (" << fixed << 100*differences/totalPixels << "%)";
    numDiffLabel->setFont("SansSerif-10");
    numDiffLabel->setLabel(oss.str());
    comp->add(keyRect1, keyLabel1->getWidth() + 5, keyLabel1->getLocation().getY() - 5);
    comp->add(keyRect2, keyLabel1->getWidth() + 5, keyLabel2->getLocation().getY() - 5);
    comp->add(numDiffLabel, 0, keyLabel2->getLocation().getY() + numDiffLabel->getHeight());
    GWindow *window = new GWindow(max((double) w + 50, comp->getWidth() + 50), 25 + h + comp->getHeight() + 10);
    window->setWindowTitle("Diff");
    window->setColor("#dddddd");
    window->add(diff, 25, 25);
    window->fillRect(0, 0, window->getWidth(), window->getHeight());
    window->add(comp, 25, 25 + h);
}

string StegoViewer::stateToString() {
    string result = "";
    if (state & CARRIER)
        result += "CARRIER ";
    else
        result += "carrier ";
    if (state & PAYLOAD)
        result += "PAYLOAD ";
    else
        result += "payload ";
    if (state & PACKAGE)
        result += "PACKAGE ";
    else
        result += "package ";
    return result;
}

bool StegoViewer::occupied(int i) {
    return state & 1 << i;
}

void StegoViewer::setOccupied(int i, bool occupied) {
    if (occupied) {
        state |= 1 << i;
    } else {
        state &= ~(1 << i);
    }
}

void StegoViewer::setBits() {
    bits = stringToInteger(bitsChooser->getSelectedItem());
}

void StegoViewer::startPleaseWait(int i) {
    gw->add(messageLabel, paneLocations[i].getX() + 20, paneLocations[i].getY() + 50);
}
void StegoViewer::endPleaseWait() {
    gw->remove(messageLabel);
}

/*   Returns largest rescaled image that will fit in
 *   maxWidth x maxHeight rectangle, maintaining original
 *  aspect ratio.
 */
GBufferedImage *sizer(const GBufferedImage *im, int maxWidth, int maxHeight) {
    double origW = im->getWidth();
    double origH = im->getHeight();
    if (origW <= maxWidth && origH <= maxHeight)
        return im->rescale(origW, origH);
    double widthR = maxWidth / origW;
    double heightR = maxHeight / origH;
    double scaleFactor = min(widthR, heightR);
    int newW = lround(scaleFactor * origW);
    int newH = lround(scaleFactor * origH);
    return im->rescale(newW, newH);
}



