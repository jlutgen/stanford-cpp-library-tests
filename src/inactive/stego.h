#ifndef STEGO_H
#define STEGO_H

string decodeTextFromImage(GBufferedImage *im);
GBufferedImage *encodeTextInImage(const string &message, const GBufferedImage *im);
GBufferedImage *decodeImageFromImage(const GBufferedImage *im, int bitDepth = 3);
GBufferedImage *encodeImageInImage(const GBufferedImage *payload, const GBufferedImage *carrier, int bitDepth = 3);

#endif // STEGO_H
