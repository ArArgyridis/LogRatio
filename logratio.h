/*This is part of the program LogRatio
   Copyright (C) 2017  Argyros Argyridis arargyridis@gmail.com

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
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
