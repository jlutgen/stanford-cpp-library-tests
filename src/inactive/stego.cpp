#include <iostream>
#include <fstream>
#include "console.h"
#include "gbufferedimage.h"
#include "gwindow.h"
#include "simpio.h"
#include "set.h"
#include "filelib.h"
#include "stegoViewer.h"

using namespace std;

struct RGB {
    int r;
    int g;
    int b;
};

int rgbToInt(int r, int g, int b) {
    if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255)
        error("rgbToInt: arguments must be between 0 and 255, inclusive");
    return (r << 16) + (g << 8) + b;
}

RGB intToRGB(int color) {
    RGB rgb;
    rgb.b = color & 0xff;
    rgb.g = color >> 8 & 0xff;
    rgb.r = color >> 16 & 0xff;
    return rgb;
}

/*
 * Client must call delete on returned pointer when no longer needed.
 */
GBufferedImage *makeGradientImage() {
    GBufferedImage *im = new GBufferedImage(256, 256);
    for (int x=0; x<256; x++)
        for (int y=0; y<256; y++)
            im->setRGB(x, y, rgbToInt(x, y, 0));
    return im;
}

/*
 * Saves an image in GIF, JPEG, and PNG formats, as
 * images/[basename].[extension]
 */
void saveImageIn3Formats(GBufferedImage *im, string basename) {
    im->save("images/" + basename + ".gif");
    im->save("images/" + basename + ".jpg");
    im->save("images/" + basename + ".png");
}

/*
 * Returns the number of distinct colors in the given image.
 */
int countColors(GBufferedImage *im) {
    Set<int> colorsSeen;
    int h = im->getWidth();
    int w = im->getHeight();
    for (int x = 0; x < w; x++)
        for (int y = 0; y < h; y++) {
            int color = im->getRGB(x, y);
            if (!colorsSeen.contains(color))
                colorsSeen.add(color);
        }
    return colorsSeen.size();
}

// Assumes text is hidden in blue channel.
string decodeTextFromImage(GBufferedImage *im) {
    string message = "";
    int w = im->getWidth();
    int h = im->getHeight();
    bool done = false;
    //cout << "image dimensions: " << w << "x" << h << endl;
    int bit = 6;
    char ch = 0;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int incr = ((im->getRGB(x, y) & 0x1) << bit);
            ch += char(incr);
            bit--;
            if (bit == -1) {
                bit = 6;
                if (ch == 0) {
                    done = true;
                    break;
                }
                message += ch;
                ch = 0;
            }
        } // end inner for
        if (done)
            break;
    } // end outer for
    return message;
}

// Modified version that assumes the green channel holds the encoded text
string modified_decodeTextFromImage(GBufferedImage *im) {
    string message = "";
    int w = im->getWidth();
    int h = im->getHeight();
    bool done = false;
    //cout << "image dimensions: " << w << "x" << h << endl;
    int bit = 6;
    char ch = 0;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int incr = (((im->getRGB(x, y) >> 8) & 0x1) << bit);  // shift right 8 bits for green channel
            ch += char(incr);
            bit--;
            if (bit == -1) {
                bit = 6;
                if (ch == 0) {
                    done = true;
                    break;
                }
                message += ch;
                ch = 0;
            }
        } // end inner for
        if (done)
            break;
    } // end outer for
    return message;
}
/*
 * Returns a pointer to a new image obtained by encoding the given
 * message into the from the given image. Does not modify the given
 * image. Signals an error if the message is too long to be encoded
 * in the image. Client is responsible for freeing memory allocated
 * for new image.
 */
GBufferedImage *encodeTextInImage(const string &message, const GBufferedImage *im) {
    int w = im->getWidth();
    int h = im->getHeight();
    string msg = message;
    msg += '\0';
    int len = msg.length();
    if (7 * len > h * w)
        error("encodeTextInImage: message is too long to be encoded in image");
    int bit = 6;
    char ch = msg[0];
    int numPixelsDone = 0;
    int index = 0;
    GBufferedImage *newIm = new GBufferedImage(w, h);
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            int rgb = im->getRGB(x, y);
            //cout << "orig:" << hex << rgb << endl;
            if (numPixelsDone > 7 * len) {
                newIm->setRGB(x, y, rgb);
                //cout << "new: " << rgb << endl;
                continue;
            }
            if ((ch >> bit) & 0x1)
              rgb |= 0x1;
            else
              rgb &= 0xfffffe;
            newIm->setRGB(x, y, rgb);
            //cout << "new: " << rgb << endl;
            numPixelsDone++;
            bit--;
            if (bit == -1) {
                bit = 6;
                index++;
                ch = msg[index];
            }
        }
    return newIm;
}

GBufferedImage *decodeImageFromImage(const GBufferedImage *im, int bitDepth) {
//        secret_image = image.copy()
//        sanitized_image = image.copy()

//        # Problem C1:
//        # TODO: Extract the secret image.

//        w, h = image.size
//        extracted = Image.new("RGB", (w, h))
//        mask = 255 >> (8 - bitdepth)
//        for i in range(w):
//            for j in range(h):
//                pixel = image.getpixel((i, j))
//                newPixel = tuple(map(lambda x: (x & mask) << (8-bitdepth), pixel))
//                secret_image.putpixel((i, j), newPixel)

//        # Return the sanitized image and the secret image
    int w = im->getWidth();
    int h = im->getHeight();
    GBufferedImage *extracted = new GBufferedImage(w, h);
    int mask = 0xff >> (8 - bitDepth);
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            RGB pixel = intToRGB(im->getRGB(x, y));
            int r = (pixel.r & mask) << (8 - bitDepth);
            int g = (pixel.g & mask) << (8 - bitDepth);
            int b = (pixel.b & mask) << (8 - bitDepth);
            extracted->setRGB(x, y, rgbToInt(r, g, b));
        }
    return extracted;
}

GBufferedImage *encodeImageInImage(const GBufferedImage *payload, const GBufferedImage *carrier, int bitDepth) {
    // Assumes payload width/height are <= carrier width/height
    int cw = carrier->getWidth();
    int ch = carrier->getHeight();
    int pw = payload->getWidth();
    int ph = payload->getHeight();
    GBufferedImage *package = new GBufferedImage(cw, ch);
    int mask = 255 << bitDepth;
    for (int y = 0; y < ch; y++) {
        for (int x = 0; x < cw; x++) {
            int carrierColor = carrier->getRGB(x, y);
            if (x >= pw || y >= ph) {
                package->setRGB(x, y, carrierColor);
            } else {
                RGB carrierPix = intToRGB(carrierColor);
                RGB payloadPix = intToRGB(payload->getRGB(x, y));
                int r = (payloadPix.r >> (8 - bitDepth)) + (carrierPix.r & mask);
                int g = (payloadPix.g >> (8 - bitDepth)) + (carrierPix.g & mask);
                int b = (payloadPix.b >> (8 - bitDepth)) + (carrierPix.b & mask);
                package->setRGB(x, y, rgbToInt(r, g, b));
            }
        }
    }
    return package;
}

void problemA() {
    GWindow *gw = new GWindow(256, 256);
    GBufferedImage *im = makeGradientImage();
    gw->add(im);
    saveImageIn3Formats(im, "gradient");
    getLine("Image saved in GIF, JPEG, and PNG formats. Press ENTER to load GIF file.");
    GBufferedImage *loaded = new GBufferedImage();
    loaded->load("images/gradient.gif");
    gw->remove(im);
    gw->add(loaded);
    int gifColors = countColors(loaded);
    getLine("Press ENTER to load JPEG file.");
    loaded->load("images/gradient.jpg");
    int jpgColors = countColors(loaded);
    getLine("Press ENTER to load PNG file.");
    loaded->load("images/gradient.png");
    int pngColors = countColors(loaded);
    delete loaded;

    cout << "Format     Number of colors in image" << endl;
    cout << "------     -------------------------" << endl;
    cout << "GIF        " << gifColors << endl;
    cout << "JPEG       " << jpgColors << endl;
    cout << "PNG        " << pngColors << endl;

    getLine("Press ENTER to close graphics window.");
    gw->close();

    delete im;
    delete gw;
}

void problemB() {
    ifstream ifs;

    // B1: test decodeTextFromImage
    while (true) {
        string yesNo = getLine("Decode text from a file? (y/n) ");
        if (yesNo != "y")
            break;
        GBufferedImage *im = new GBufferedImage();
        string filename = openFileDialog("Decode text from file", "images/*.png");
        if (filename.empty())
            continue; // user canceled dialog
        im->load(filename);
        cout << "Secret message:" << endl;
        cout << decodeTextFromImage(im) << endl;
        if (ifs.is_open())
            ifs.close();
        delete im;
    }

    // B2: test encodeTextInImage
    while (true) {
        string yesNo = getLine("Encode text in an image? (y/n) ");
        if (yesNo != "y")
            break;
        string message = getLine("Enter message: ");
        GBufferedImage *im = new GBufferedImage(20, 20, 0xff0000);
        GBufferedImage *newIm = encodeTextInImage(message, im);
        string decoded = decodeTextFromImage(newIm);
        if (message == decoded)
            cout << "Decoded message matches!" << endl;
        else
            cout << "Bad news: decoded message:" << decoded << endl;
        delete im;
        delete newIm;
    }

    // B3: obtain instructions to get image viewer GUI
    string yesNo = getLine("Print instructions for obtaining image viewer GUI? (y/n) ");
    if (yesNo != "y")
        return;
    GBufferedImage *im = new GBufferedImage();
    im->load("images/transformers.png");
    cout << decodeTextFromImage(im);

    // obtain image viewer
    cout << "\nImageViewer code:" << endl;
    cout << modified_decodeTextFromImage(im);

    delete im;
}



int main() {
    setConsoleSize(700, 200);
    setConsolePrintExceptions(true);

    //problemA();
    //problemB();

    //getLine("pausing (ENTER):");
    StegoViewer viewer;

    viewer.run();


    /******************************************************************************/
    getLine("\n*** Program finished. Press ENTER to close all windows and exit.");
    exitGraphics();
    return 0;
}
