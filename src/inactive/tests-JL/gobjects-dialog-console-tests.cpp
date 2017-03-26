/*
 * @file gobjects-dialog-console-tests.cpp
 *
 * @author Jeff Lutgen
 *
 */

#include <iostream>
#include <unistd.h>
#include "gobjects.h"
#include "ginteractors.h"
#include "console.h"
#include "gevents.h"
#include "simpio.h"
#include "random.h"
#include "strlib.h"
#include "filelib.h"
#include "gmath.h"
#include "map.h"
#include "gtextarea.h"

using namespace std;

GWindow *gw;
string colors[13] = {"BLACK", "BLUE", "CYAN", "DARK_GRAY", "GRAY", "GREEN", "LIGHT_GRAY", "MAGENTA", "ORANGE", "PINK",
                     "RED", "WHITE", "YELLOW"};

void cycleObjects(Vector<GObject *> &objs, bool forward=true);
void moveTopDown(Vector<GObject *> & objs);
void moveBottomUp(Vector<GObject *> & objs);

void test_front_back() {
    string colors[3] = {"blue", "red", "green"};
    const int objWidth = 100;
    const int objHeight = 100;
    const int x0 = 100;
    const int y0 = 100;
    const int dx = 40;
    const int dy = 0;
    GButton *forward = new GButton("Cycle Forward");
    GButton *backward = new GButton("Cycle Backward");
    GButton *stepUp = new GButton("Backmost +1");
    GButton *stepDown = new GButton("Frontmost -1");
    Vector<GObject *> objects;
    gw->addToRegion(forward, "south");
    gw->addToRegion(backward, "south");
    gw->addToRegion(stepUp, "south");
    gw->addToRegion(stepDown, "south");
    GCompound *compound = new GCompound();
    for (int i = 0; i < 3; i++) {
        GOval *oval = new GOval(x0 + i*dx, y0 + i*dy, objWidth, objHeight);
        if (i == 1)
            oval->setLocation(oval->getLocation().getX(), oval->getLocation().getY() + 50);
        oval->setFilled(true);
        oval->setFillColor(colors[i]);
        objects.add(oval);
        compound->add(oval);
    }
    gw->add(new GLabel("Click in window to end test", 10, 30));
    gw->add(compound);
    while(true) {
        GEvent e = waitForEvent(ACTION_EVENT | CLICK_EVENT);
        if (e.getEventType() == MOUSE_CLICKED)
            break;
        else if (e.getEventType() == ACTION_PERFORMED) {
            string cmd = ((GActionEvent) e).getActionCommand();
            if (cmd == "Cycle Forward")
                cycleObjects(objects);
            if (cmd == "Cycle Backward")
                cycleObjects(objects, false);
            if (cmd == "Backmost +1")
                moveBottomUp(objects);
            if (cmd == "Frontmost -1")
                moveTopDown(objects);
        }
    }

}

void cycleObjects(Vector<GObject *> &objs, bool forward) {
    if (objs.isEmpty()) return;
    if (forward) {
        int lastIndex = objs.size() - 1;
        GObject *last = objs[lastIndex];
        for (int i = lastIndex; i>=1; i--) {
            objs[i] = objs[i-1];
        }
        objs[0] = last;
        objs[0]->sendToBack();
    } else {
        int lastIndex = objs.size() - 1;
        GObject *first = objs[0];
        for (int i = 0; i < lastIndex; i++) {
            objs[i] = objs[i + 1];
        }
        objs[lastIndex] = first;
        objs[lastIndex]->sendToFront();
    }
}

void moveBottomUp(Vector<GObject *> & objs) {
    if (objs.size() < 2) return;
    objs[0]->sendForward();
    GObject *tmp = objs[1];
    objs[1] = objs[0];
    objs[0] = tmp;
}

void moveTopDown(Vector<GObject *> & objs) {
    int lastIndex = objs.size() - 1;
    if (lastIndex < 1) return;
    objs[lastIndex]->sendBackward();
    GObject *tmp = objs[lastIndex];
    objs[lastIndex] = objs[lastIndex - 1];
    objs[lastIndex - 1] = tmp;
}

void test_image() {
    GImage *im = new GImage("avatar.gif"); // in images subdirectory
    GImage *webIm = new GImage("http://tikiloungetalk.com/wp-content/uploads/2010/11/speedracer-old.jpg");

#ifdef _WIN32
    GImage *absPathIm = new GImage("C:/Qt/Qt5.3.2/Tools/QtCreator/share/qtcreator/qmlicons/Qt/16x16/CountBubble.png");
#else
    GImage *absPathIm = new GImage("/usr/share/doc/cups/images/smiley.jpg");
#endif

    gw->add(im, 10, 10);
    int x = 10 + im->getWidth();
    gw->add(webIm, x, 10);
    x += webIm->getWidth();
    gw->add(absPathIm, x, 10);
}

void test_focus() {
    GWindow *gw2 = new GWindow(200, 200);
    getLine("ENTER to start focus test");
    for (int i=0; i<5; i++) {
        gw->requestFocus();
        pause(1000);
        gw2->requestFocus();
        pause(1000);
    }
    gw2->close();
}

void test_draw() {
    double width = gw->getWidth();
    double height = gw->getHeight();
    const int mod = 200;
    int shift;
    for (int i = 0; i < 1000; i++) {
        gw->clear();
        shift = i % mod - mod / 2;
        gw->drawLine(0 + shift, height / 2, width / 2 + shift, 0);
        gw->drawLine(width / 2 + shift, 0, width + shift, height / 2);
        gw->drawLine(width + shift, height / 2, width / 2 + shift, height);
        gw->drawLine(width / 2 + shift, height, 0 + shift, height / 2);
        gw->setColor("BLUE");
        gw->fillRect(width / 4 + shift, height / 4, width / 2, height / 2);
        gw->setColor("GRAY");
        gw->fillOval(width / 4 + shift, height / 4, width / 2, height / 2);
        pause(20);
    }
}

void rotate_about_center(GObject *gobj, double deg) {
    double x = gobj->getLocation().getX();
    double y = gobj->getLocation().getY();
    double cx = x + gobj->getWidth()/2;
    double cy = y + gobj->getHeight()/2;
    double dx =  (cx-x)*cosDegrees(deg) + (cy-y)*sinDegrees(deg) - (cx-x);
    double dy = -(cx-x)*sinDegrees(deg) + (cy-y)*cosDegrees(deg) - (cy-y);
    gobj->rotate(deg);
    gobj->setLocation(x - dx, y - dy);
}

void test_rotate_scale() {
    gw->add(new GLabel("scale(2,1) then rotate(30)"), 100, 200);
    gw->add(new GLabel("rotate(30) then scale(2,1)"), 400, 200);
    GRect *r1 = new GRect(100, 300, 80, 80);
    r1->setFilled(true);
    gw->add(r1);
    GRect *r1a = new GRect(100, 300, 80, 80);
    r1a->setColor("red");
    r1a->rotate(20);
    r1a->scale(2, 0.5);
    //    rotate_about_center(r1a, 20);
    gw->add(r1a);

    GRect *r2 = new GRect(400, 300, 80, 80);
    r2->setFilled(true);
    gw->add(r2);
    GRect *r2a = new GRect(400, 300, 80, 80);
    r2a->setColor("red");
    r2a->scale(2, 1);
    r2a->rotate(30);
    gw->add(r2a);
}

void test_label() {
    GLabel *label1 = new GLabel("<html>Blue Line 1<br/>Line 2</html>");
    label1->setFont("Serif-Bold-18");
    label1->setColor("blue");
    gw->addToRegion(label1, "south");
    pause(2000);
    label1->setLabel("Red italic");
    label1->setFont("Monospaced-Italic-24");
    label1->setColor("red");

    pause(2000);
    GLabel *label2 = new GLabel("<html>Blue<br/>all on one line</html>");
    label2->setFont("Monospaced-Plain-24");
    label2->setColor("blue");
    gw->add(label2, 200, 200);
    pause(2000);
    label2->setLabel("Rotate me");
    pause(2000);
    label2->rotate(45);
}

void test_add_remove_torture() {
    int width = gw->getCanvasWidth();
    int height = gw->getCanvasHeight();
    int numEachObj = 100;
    Vector<GObject *> objs;
    Vector<GObject *> added;
    for (int i = 0; i<numEachObj; i++) {
        int x = randomInteger(0, width);
        int y = randomInteger(0, height);
        int w = randomInteger(1, width/2);
        int h = randomInteger(1, width/2);
        GRect *r = new GRect(x, y, w, h);
        r->setLineWidth(2);
        r->setColor(colors[randomInteger(0, 12)]);
        r->rotate(randomReal(0,90));
        objs.add(r);
    }
    for (int i = 0; i < 2000; i++) {
        if (randomChance(0.8)) {
            int index = randomInteger(0, objs.size() - 1);
            gw->add(objs[index]);
            added.add(objs[index]);
        }
        pause(1);
        if (randomChance(0.05) && !added.isEmpty()) {
            int index = randomInteger(0, added.size() - 1);
            gw->remove(added[index]);
            added.remove(index);
        }
        pause(1);
    }
}

void test_checkbox_selected_torture() {
    GCheckBox *cb = new GCheckBox("checky");
    cb->setLocation(gw->getWidth()/2, gw->getHeight()/2);
    gw->add(cb);
    for (int i = 0; i < 4000; i++) {
        bool state = i % 2;
        cb->setSelected(state);
        bool gotState = cb->isSelected();
        if (state != gotState) {
            cout << "oops" << endl;
            break;
        }
    }
}

void test_slider_torture() {
    GSlider *slider = new GSlider(0, 500, 250);
    slider->setLocation(gw->getWidth()/2, gw->getHeight()/2);
    gw->add(slider);
    for (int i = 0; i < 10000; i++) {
        int value = randomInteger(0, 500);
        slider->setValue(value);
        int gotValue = slider->getValue();
        if (value != gotValue) {
            cout << "oops" << endl;
            break;
        }
    }
}

string randomString(int length) {
    string result = "";
    for (int i = 0; i < length; i++) {
        result += 'a' + randomInteger(0, 25);
    }
    return result;
}

void test_textfield_torture() {
    GTextField *textField = new GTextField(60);
    textField->setLocation(gw->getWidth()/2-textField->getSize().getWidth()/2, gw->getHeight()/2);
    gw->add(textField);
    for (int i = 0; i < 1000; i++) {
        string value = randomString(80);
        string rgbString = colors[randomInteger(0,12)];
        textField->setColor(rgbString);
        string gotColor = textField->getColor();
        if (convertColorToRGB(gotColor) != convertColorToRGB(rgbString)) {
            cout << "oops" << endl;
            break;
        }
        textField->setText(value);
        string gotValue = textField->getText();
        if (value != gotValue) {
            cout << "oops" << endl;
            break;
        }
    }
}

void test_textarea() {
    for (int i = 0; i < 1000; i++) {
        GTextArea *textArea = new GTextArea(10, 10, gw->getWidth() - 20, gw->getHeight() - 20);
        gw->add(textArea);
        textArea->setFont("Serif-Plain-24");
        textArea->setBackgroundColor("#9060c0");
        string txt = "Here is\nsome text.";
        textArea->setText(txt);
        //textArea->setEditable(false);
        string gotText = textArea->getText();
        if (txt != gotText) {
            cout << "oops" << endl;
            cout << "    txt:" << txt << endl;
            cout << "gotText:" << gotText << endl;
            break;
        }
        gw->remove(textArea);
        delete textArea;
    }
}

void test_chooser_torture() {
    const int numItems = 100;
    GChooser *chooser = new GChooser();
    Vector<string> items;
    for (int i=0; i<numItems; i++) {
        string item = randomString(8);
        items.add(item);
        chooser->addItem(item);
    }
    chooser->setLocation(gw->getWidth()/2 - chooser->getSize().getWidth()/2, gw->getHeight()/2);
    gw->addToRegion(chooser, "north");
    for (int i = 0; i < 10000; i++) {
        int index = randomInteger(0, numItems-1);
        chooser->setSelectedItem(items[index]);
        string item = chooser->getSelectedItem();
        if (item != items[index]) {
            cout << "oops" << endl;
            break;
        }
    }
}

void test_region_alignment() {
    string align[3] = { "LEFT", "CENTER", "RIGHT" };
    GButton *b1 = new GButton("Life's");
    GButton *b2 = new GButton("Rich");
    GButton *b3 = new GButton("Pageant");
    GButton *b4 = new GButton("Fables");
    GButton *b5 = new GButton("of the");
    GButton *b6 = new GButton("Reconstruction");
    gw->addToRegion(b1, "NORTH");
    gw->addToRegion(b2, "NORTH");
    gw->addToRegion(b3, "NORTH");
    gw->addToRegion(b4, "SOUTH");
    gw->addToRegion(b5, "SOUTH");
    gw->addToRegion(b6, "SOUTH");
    for (int i = 0; i < 1000; i++) {
        gw->setRegionAlignment("NORTH", align[randomInteger(0, 2)]);
        pause(1);
        gw->setRegionAlignment("SOUTH", align[randomInteger(0, 2)]);
        pause(1);
    }
}

void test_window_torture() {
    for (int i = 0; i < 10; i++) {
        GWindow *window = new GWindow(200, 200);
        GOval oval(100, 100);
        window->setColor("red");
        window->drawOval(10, 10, 150, 80);
        //pause(100);
        window->close();
        delete window;
    }
}

void test_file_dialog() {
    string type = getLine("(o)pen or (s)ave? ");
    cout << "currentDirectory is: " << getCurrentDirectory() << endl;
    string startDir = getLine("Starting directory (blank for current dir): ");
    string filter = getLine("file filter(s): ");
    if (!endsWith(startDir, '/') && !endsWith(startDir, '\\'))
        startDir += '/';
    string result;
    if (type == "o") {
        result = openFileDialog("Pick a file!", startDir + filter);
    } else {
        result = saveFileDialog("Pick a file!", startDir + filter);
    }
    if (result.empty())
        cout << "User canceled dialog or file could not be opened for reading" << endl;
    else
        cout << "Dialog result: " <<  result << endl;
}

void test_nested_compounds_with_interactors() {
    GCompound *c1 = new GCompound();
    GCompound *c11 = new GCompound();
    GCompound *c12 = new GCompound();
    GCompound *c111 = new GCompound();
    GCompound *c112 = new GCompound();
    GButton *b0 = new GButton("0");
    GButton *b1 = new GButton("1");
    GButton *b11 = new GButton("1-1");
    GButton *b12 = new GButton("1-2");
    GButton *b111 = new GButton("1-1-1");
    GButton *b112 = new GButton("1-1-2");

    c1->add(new GRect(500, 500));
    c11->add(new GRect(240, 450), 5, 5);
    c12->add(new GRect(240, 450), 5, 5);
    c1->add(c11, 0, 40);
    c1->add(c12, 250, 40);
    c111->add(new GRect(230, 190), 5, 5);
    c112->add(new GRect(230, 190), 5, 5);

    c1->add(b1, 25, 2);
    c11->add(b11, 25, 7);
    c12->add(b12, 25, 7);
    c111->add(b111, 25, 7);
    c112->add(b112, 25, 7);

    c11->add(c111, 5, 50);
    c11->add(c112, 5, 245);

    gw->add(b0, 2, 2);
    gw->add(c1, 50, 50);

    pause(2000);
    c12->move(0, -35);
    pause(2000);
    b12->move(0, 35);
    pause(2000);
    c11->remove(b11);
    pause(2000);
    c11->add(b11, 100, 7); // shifted right
    pause(2000);
    c11->remove(c112);
    pause(2000);
    c11->add(c112, 5, 245);
    pause(2000);
    for (int i=0; i<100; i++) {
        c1->move(1, 0);
        pause(40);
    }


}

void test_console() {
    int p = 100;
    clearConsole();
    for (int i = 8; i < 72; i += 3) {
        string font = string("Monospaced-") + integerToString(i);
        setConsoleFont(font);
        cout << font << endl;
        pause(p);
        clearConsole();
    }
    for (int i = 8; i < 72; i += 3) {
        string font = string("Serif-") + integerToString(i);
        setConsoleFont(font);
        cout << font << endl;
        pause(p);
        clearConsole();
    }
    for (int i = 8; i < 72; i += 3) {
        string font = string("SansSerif-") + integerToString(i);
        setConsoleFont(font);
        cout << font << endl;
        pause(p);
        clearConsole();
    }
    setConsoleFont("Monospaced-11");
    cout << "Console test ended" << endl;
}

void test_interactors_as_objects() {
    GButton *b = new GButton("button");
    b->rotate(90);
    b->scale(2);
    b->setColor("red");
    b->setSize(100, 100);
    b->setLineWidth(20);
    gw->add(b, 10, 10);
    gw->drawRect(b->getBounds());

    GSlider *s = new GSlider(5, 40, 20);
    s->rotate(90);
    s->scale(2);
    s->setColor("red");
    s->setSize(100, 100);
    s->setLineWidth(20);
    gw->add(s, 120, 10);
    gw->drawRect(s->getBounds());

    GCheckBox *c = new GCheckBox("checkbox");
    c->rotate(90);
    c->scale(2);
    c->setColor("red");
    c->setSize(100, 100);
    c->setLineWidth(20);
    gw->add(c, 230, 10);
    gw->drawRect(c->getBounds());

    GChooser *ch = new GChooser;
    ch->addItem("beef");
    ch->addItem("veal");
    ch->rotate(90);
    ch->scale(2);
    ch->setColor("red");
    ch->setSize(100, 100);
    ch->setLineWidth(20);
    gw->add(ch, 340, 10);
    gw->drawRect(ch->getBounds());

    GTextField *t = new GTextField(40);
    t->rotate(90);
    t->scale(2);
    t->setColor("red");
    t->setSize(100, 100);
    t->setLineWidth(20);
    t->setText("text field");
    gw->add(t, 450, 10);
    gw->drawRect(t->getBounds());

    GButton *b2 = new GButton("button");
    b2->rotate(90);
    b2->scale(2);
    b2->setColor("red");
    b2->setSize(100, 100);
    b2->setLineWidth(20);
    gw->addToRegion(b2, "SOUTH");

    GSlider *s2 = new GSlider(5, 40, 20);
    s2->rotate(90);
    s2->scale(2);
    s2->setColor("red");
    s2->setSize(100, 100);
    s2->setLineWidth(20);
    gw->addToRegion(s2, "SOUTH");

    GCheckBox *c2 = new GCheckBox("checkbox");
    c2->rotate(90);
    c2->scale(2);
    c2->setColor("red");
    c2->setSize(100, 100);
    c2->setLineWidth(20);
    gw->addToRegion(c2, "SOUTH");

    GChooser *ch2 = new GChooser;
    ch2->addItem("beef");
    ch2->addItem("veal");
    ch2->rotate(90);
    ch2->scale(2);
    ch2->setColor("red");
    ch2->setSize(100, 100);
    ch2->setLineWidth(20);
    gw->addToRegion(ch2, "SOUTH");

    GTextField *t2 = new GTextField;
    t2->rotate(90);
    t2->scale(2);
    t2->setColor("red");
    t2->setSize(100, 100);
    t2->setLineWidth(20);
    t2->setText("text field");
    gw->addToRegion(t2, "SOUTH");
}

void drawGrid() {
    gw->setColor("green");
    for (int i=0; i<gw->getCanvasWidth(); i+=10)
        gw->drawLine(i, 0, i, gw->getCanvasHeight());
    for (int j=0; j<gw->getCanvasHeight(); j+=10)
        gw->drawLine(0, j, gw->getCanvasWidth(), j);
}

void test_contains_and_getBounds() {
    bool useCompounds = false;
    int x0 = 350;
    int y0 = 300;
    Map<string, GObject*> shapeMap;
    GOval *oval = new GOval(x0, y0, 200, 100);
    GRoundRect *roundRect = new GRoundRect(x0, y0, 200, 100, 300);
    roundRect->setLineWidth(20);
    G3DRect *rect3d = new G3DRect(x0, y0, 200, 100, true);
    //rect3d->setLineWidth(5);
    rect3d->setFillColor("green");
    rect3d->setFilled(true);
    GPolygon *poly = new GPolygon;
    poly->addVertex(0, 0);
    poly->addEdge(200, 100);
    poly->addEdge(-200, 0);
    poly->setLocation(x0, y0);
    GPolygon *cpoly = new GPolygon;
    cpoly->addVertex(0, 0);
    cpoly->addEdge(200, 100);
    cpoly->addEdge(0, -100);
    cpoly->addEdge(-200, 100);
    cpoly->setLocation(x0, y0);
    GRect *rect = new GRect(x0, y0, 200, 100);
    GLine *line = new GLine(x0, y0, x0 + 200, y0 + 100);
    GLabel *label = new GLabel("Ostromantus", x0, y0);
    GArc *arc = new GArc(x0, y0, 350, 350, 0, 90);
    //arc->setLineWidth(5);
    arc->setColor("#44000000");
    GArc *filledArc = new GArc(x0, y0, 350, 100, 45, 225);
    filledArc->setFillColor("#88e0e0e0");
    filledArc->setFilled(true);
    GCompound *comp1 = new GCompound;
    comp1->setLocation(x0, y0);
    comp1->add(new GLabel("compound", 0, 15));
    GRect *bgRect1 = new GRect(0, 0);
    gw->add(bgRect1);
    bgRect1->setFillColor("#55dddddd");
    bgRect1->setFilled(true);
    GImage *image = new GImage("homer-transparent.png");
    image->setLocation(x0, y0);
    GCompound *comp = new GCompound;
    comp->setLocation(x0, y0);
    GRect *compRect = new GRect(20, 20, 100, 100);
    GOval *compOval = new GOval(90, 90, 150, 70);
    comp->add(compRect);
    comp->add(compOval);
    GButton *button = new GButton("Testo");
    button->setSize(200, 100);
    button->setLocation(x0, y0);
    shapeMap.put("oval", oval);
    shapeMap.put("rounded rectangle", roundRect);
    shapeMap.put("3D rectangle", rect3d);
    shapeMap.put("polygon", poly);
    shapeMap.put("crazy polygon", cpoly);
    shapeMap.put("rectangle", rect);
    shapeMap.put("line", line);
    shapeMap.put("arc", arc);
    shapeMap.put("filled arc", filledArc);
    shapeMap.put("label", label);
    shapeMap.put("image", image);
    shapeMap.put("compound", comp);
    shapeMap.put("button", button);

    GObject *currObj;
    GChooser *ch = new GChooser;
    ch->setActionCommand("chooser");
    ch->addItem("oval");
    ch->addItem("rounded rectangle");
    ch->addItem(("3D rectangle"));
    ch->addItem("polygon");
    ch->addItem("crazy polygon");
    ch->addItem("rectangle");
    ch->addItem("line");
    ch->addItem("arc");
    ch->addItem("filled arc");
    ch->addItem("label");
    ch->addItem("image");
    ch->addItem("compound");
    ch->addItem("button");
    ch->setSelectedItem("rectangle");
    currObj = rect;

    GButton *endButton = new GButton("End test");
    GButton *fillButton = new GButton("Auto-fill");
    GButton *rotateButton = new GButton("Rotate");
    GButton *scaleButton = new GButton("Scale");

    GCheckBox *compCheckbox = new GCheckBox("compounds");
    compCheckbox->setActionCommand("compounds");
    gw->addToRegion(compCheckbox, "north");
    gw->addToRegion(ch, "north");
    gw->addToRegion(rotateButton, "north");
    gw->addToRegion(scaleButton, "north");
    gw->addToRegion(fillButton, "north");
    gw->addToRegion(endButton, "north");

    while (true) {
        GEvent e = waitForEvent(ACTION_EVENT | MOUSE_EVENT);
        if (!e.isValid())
            continue;
        if (e.getEventClass() == ACTION_EVENT) {
            if (((GActionEvent) e).getActionCommand() == "End test")
                break;
            if (((GActionEvent) e).getActionCommand() == "compounds") {
                bgRect1->setVisible(compCheckbox->isSelected());
                useCompounds = compCheckbox->isSelected();
            }
            if (((GActionEvent) e).getActionCommand() == "Testo") {
                GPoint pt = button->getLocation();
                button->setLocation(pt.getX()-button->getWidth()-10, pt.getY());
                pause(1000);
                button->setLocation(pt);
            }
            if (((GActionEvent) e).getActionCommand() == "Auto-fill") {
                GRectangle bds = currObj->getBounds();
                int xmin = bds.getX();
                int ymin = bds.getY();
                int xmax = bds.getX() + bds.getWidth();
                int ymax = bds.getY() + bds.getHeight();
                int dx = useCompounds ? comp1->getX(): 0;
                int dy = useCompounds ? comp1->getY(): 0;
                for (int y = ymin; y < ymax; y+=1)
                    for (int x = xmin; x < xmax; x+=1) {
                        if (currObj->contains(x, y)) {
                            gw->setColor("red");
                            gw->fillOval(x + dx, y + dy, 1, 1);
                        } else {
                            gw->setColor("green");
                            gw->fillOval(x + dx, y + dy, 1, 1);
                        }
                    }
            }

            if (((GActionEvent) e).getActionCommand() == "Rotate") {
                currObj->rotate(45);
                if (useCompounds) {
                    bgRect1->setBounds(comp1->getBounds());
                }
            }
            if (((GActionEvent) e).getActionCommand() == "Scale") {
                currObj->scale(1.2, 0.8);
                if (useCompounds) {
                    bgRect1->setBounds(comp1->getBounds());
                }
            }
            if (((GActionEvent) e).getActionCommand() == "chooser") {
                string shape = ch->getSelectedItem();
                if (useCompounds) {
                    comp1->remove(currObj);
                    gw->remove(comp1);
                } else {
                    gw->remove(currObj);
                }
                gw->setColor("white");
                gw->fillRect(0, 0, gw->getCanvasWidth(), gw->getCanvasHeight());
                //drawGrid();
                gw->setColor("black");
                currObj = shapeMap.get(shape);
                if (useCompounds) {
                    gw->add(comp1);
                    comp1->add(currObj, 50, 50);
                    bgRect1->setBounds(comp1->getBounds());
                } else {
                    gw->add(currObj);
                }
                gw->drawOval(currObj->getX()-2, currObj->getY()-2, 4, 4);
            }
        } else if (e.getEventType() == MOUSE_CLICKED) {
            double x = ((GMouseEvent) e).getX();
            double y = ((GMouseEvent) e).getY();
            if (currObj->contains(x, y)) {
                gw->setColor("red");
                gw->fillOval(x, y, 1, 1);
            }
        }
    }
}

int main() {
    setConsolePrintExceptions(true);
    setConsoleSize(getScreenWidth()-710-10, 500);
    setConsoleLocation(720, 100);
    cout << "Ready." << endl;
    getLine("ENTER to go");
    gw = new GWindow(710, 610);
    gw->setVisible(false);
    while (true) {
        cout << "f) send front/back" << endl;
        cout << "i) image" << endl;
        cout << "F) focus" << endl;
        cout << "d) draw" << endl;
        cout << "R) rotate/scale" << endl;
        cout << "a) add/remove" << endl;
        cout << "c) checkbox" << endl;
        cout << "s) slider" << endl;
        cout << "t) text field" << endl;
        cout << "T) text area" << endl;
        cout << "l) label" << endl;
        cout << "h) chooser" << endl;
        cout << "r) region alignment" << endl;
        cout << "D) file dialog" << endl;
        cout << "w) window create/destroy" << endl;
        cout << "n) nested compounds with interactors" << endl;
        cout << "C) console" << endl;
        cout << "I) interactors as objects" << endl;
        cout << "co) contains/getBounds" << endl;
        string cmd = getLine("Command (Enter to quit)?");
        if (cmd.empty()) {
            break;
        } else if (cmd == "f") {
            gw->setVisible(true);
            gw->clear();
            test_front_back();
        } else if (cmd == "i") {
            gw->setVisible(true);
            gw->clear();
            test_image();
        } else if (cmd =="F") {
            gw->setVisible(true);
            gw->clear();
            test_focus();
        } else if (cmd == "d") {
            gw->setVisible(true);
            gw->clear();
            test_draw();
        } else if (cmd == "R") {
            gw->setVisible(true);
            gw->clear();
            test_rotate_scale();
        } else if (cmd == "a") {
            gw->setVisible(true);
            gw->clear();
            test_add_remove_torture();
        } else if (cmd == "l") {
            gw->setVisible(true);
            gw->clear();
            test_label();
        } else if (cmd == "c") {
            gw->setVisible(true);
            gw->clear();
            test_checkbox_selected_torture();
        } else if (cmd == "s") {
            gw->setVisible(true);
            gw->clear();
            test_slider_torture();
        } else if (cmd == "t") {
            gw->setVisible(true);
            gw->clear();
            test_textfield_torture();
        } else if (cmd == "T") {
            gw->setVisible(true);
            gw->clear();
            test_textarea();
        } else if (cmd == "h") {
            gw->setVisible(true);
            gw->clear();
            test_chooser_torture();
        } else if (cmd == "r") {
            gw->setVisible(true);
            gw->clear();
            test_region_alignment();
        } else if (cmd == "D") {
            gw->setVisible(false);
            test_file_dialog();
        } else if (cmd == "w") {
            gw->setVisible(false);
            test_window_torture();
        } else if (cmd == "n") {
            gw->setVisible(true);
            gw->clear();
            test_nested_compounds_with_interactors();
        } else if (cmd == "C") {
            gw->setVisible(false);
            test_console();
            gw->setVisible(true);
        } else if (cmd == "I") {
            gw->setVisible(true);
            gw->clear();
            test_interactors_as_objects();
        } else if (cmd == "co") {
            gw->setVisible(true);
            gw->clear();
            test_contains_and_getBounds();
        }
    }

    getLine("ENTER to exit");
    exitGraphics();
    return 0;
}
