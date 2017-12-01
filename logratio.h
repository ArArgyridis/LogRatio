#ifndef LOGRATIO_H
#define LOGRATIO_H
#include "functions.h"
#include <gdal_priv.h>
#include <memory>
class LogRatio
{
private:
    std::string inImage1, inImage2, polarization, outputFile;
    double threshold;
    int xBlockCount, yBlockCount, xBlockSize, yBlockSize, imageWidth, imageHeight, d1BandNumber, d2BandNumber;
    GDALDataset *dateImage1, *dateImage2;
    int createOutputDataset();

public:
    LogRatio(char* img1, char* img2, char* polar, float threshold, char* out );
    int init();
    int compute();
    ~LogRatio();
};
#endif // LOGRATIO_H
