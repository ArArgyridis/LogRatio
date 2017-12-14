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
#include "logratio.h"
#include <gdal_priv.h>
using namespace std;

int LogRatio::createOutputDataset() {
    //creating output directory
    try {
    string driver = "GTiff";

    GDALDriver *poDriver;
    poDriver = GetGDALDriverManager()->GetDriverByName(driver.c_str());

    //checking if path is correct
    char splitChar = '/';
    vector<string> splitOutFile;
    splitString(outputFile, splitChar, splitOutFile);

    string outputPath = "";
    for(size_t i = 0; i < splitOutFile.size()  - 1; i++)
        outputPath += splitOutFile[i] + "/";

    bool checkDir = false;
    if (outputPath !="") {
        checkDir = createDirectory(outputPath);
        if (!checkDir)
            exit (1);
    }

    cout <<"Creating output dataset: " << outputFile << endl;
    GDALDataset *tmpDataset;
    tmpDataset = poDriver->Create(outputFile.c_str(), imageWidth, imageHeight, 1, GDT_Float32, NULL);
    if (tmpDataset == nullptr){
        throw 3;
    }
    double geoTranform[6];
    dateImage1->GetGeoTransform(geoTranform);
    tmpDataset->SetGeoTransform(geoTranform);
    tmpDataset->SetProjection(dateImage1->GetProjectionRef());

    //determining block size
     tmpDataset->GetRasterBand(1)->GetBlockSize(&xBlockSize, &yBlockSize);
     xBlockCount  = (imageWidth + (xBlockSize - 1))/xBlockSize;
     yBlockCount  = (imageHeight + (yBlockSize - 1 ))/yBlockSize;

    GDALClose(tmpDataset);
    }
    catch(int e) {
        return -1;
    }
}

LogRatio::LogRatio(char *img1, char *img2, char *polar, char *out): inImage1(img1), inImage2(img2), polarization(polar),threshold(null), outputFile(out),
dateImage1(nullptr), dateImage2(nullptr), xBlockCount(0), yBlockCount(0), xBlockSize(0), yBlockSize(0) {
    //checking if polarization value is proper
    boost::algorithm::to_lower(polarization);

    //reading images
    GDALAllRegister();
    dateImage1 = (GDALDataset*)GDALOpen( inImage1.c_str(), GA_ReadOnly );
    dateImage2 = (GDALDataset*)GDALOpen( inImage2.c_str(), GA_ReadOnly );
}


LogRatio::LogRatio(char *img1, char *img2, char *polar, char *out, float threshold): inImage1(img1), inImage2(img2), polarization(polar),threshold(threshold), outputFile(out),
dateImage1(nullptr), dateImage2(nullptr), xBlockCount(0), yBlockCount(0), xBlockSize(0), yBlockSize(0) {
    //checking if polarization value is proper
    boost::algorithm::to_lower(polarization);

    //reading images
    GDALAllRegister();
    dateImage1 = (GDALDataset*)GDALOpen( inImage1.c_str(), GA_ReadOnly );
    dateImage2 = (GDALDataset*)GDALOpen( inImage2.c_str(), GA_ReadOnly );
}

int LogRatio::compute() {
#pragma omp parallel
    {
        int xBlock, yBlock;
        GDALDataset *outDataset, *d1Dataset, *d2Dataset;
        outDataset = (GDALDataset*)(GDALOpen(outputFile.c_str(), GA_Update));
        GDALRasterBand *outBand;
        outBand = outDataset->GetRasterBand(1);

        d1Dataset = (GDALDataset*)(GDALOpen(inImage1.c_str(), GA_Update));
        GDALRasterBand *d1Band;
        d1Band = d1Dataset->GetRasterBand(d1BandNumber);

        d2Dataset = (GDALDataset*)(GDALOpen(inImage2.c_str(), GA_Update));
        GDALRasterBand *d2Band;
        d2Band = d2Dataset->GetRasterBand(d2BandNumber);

        float *outBuffer;
        float *d1Buffer, *d2Buffer;
        outBuffer = new float[xBlockSize*yBlockSize];
        d1Buffer =  new float[xBlockSize*yBlockSize];
        d2Buffer = new float[xBlockSize*yBlockSize];

        fill(d1Buffer, d1Buffer + xBlockSize*yBlockSize, 0.0);
        fill(d2Buffer, d2Buffer + xBlockSize*yBlockSize, 0.0);

#pragma omp for schedule(dynamic, 1) collapse(2)
        //cout << xBlockCount <<"\t" <<yBlockCount <<"\t" <<xBlockSize<<"\t" <<yBlockSize << endl;
        for(xBlock = 0; xBlock < xBlockCount; xBlock++) {
            for(yBlock = 0; yBlock < yBlockCount; yBlock++) {
                fill(outBuffer, outBuffer + xBlockSize*yBlockSize, 0.0);
                //reading block data
                //cout << xBlock <<"\t" <<yBlock << endl;
                d1Band->AdviseRead(xBlock*xBlockSize, yBlock*yBlockSize, xBlockSize, yBlockSize, xBlockSize, yBlockSize, GDT_Float32, NULL);
                d1Band->RasterIO(GF_Read, xBlock*xBlockSize, yBlock*yBlockSize, xBlockSize, yBlockSize, d1Buffer, xBlockSize, yBlockSize, GDT_Float32, 0, 0);

                d2Band->AdviseRead(xBlock*xBlockSize, yBlock*yBlockSize, xBlockSize, yBlockSize, xBlockSize, yBlockSize, GDT_Float32, NULL);
                d2Band->RasterIO(GF_Read, xBlock*xBlockSize, yBlock*yBlockSize, xBlockSize, yBlockSize, d2Buffer, xBlockSize, yBlockSize, GDT_Float32, 0, 0);

                /*
                d1Band->ReadBlock(xBlock, yBlock, d1Buffer);
                d2Band->ReadBlock(xBlock, yBlock, d2Buffer);
                */
                for (register int y = 0; y <yBlockSize; y++) {
                    for (register int x = 0; x < xBlockSize; x++) {
                   
                        int pos = xBlockSize*y + x;
                        //cout << pos << "\t" << x<< "\t" << y << "\t" << xBlockSize << endl;
                        if (d1Buffer[pos] > 0 && d2Buffer[pos] > 0 ) {
                            double ratio = log(d2Buffer[pos]/d1Buffer[pos]);
                            if (threshold != null)
                                outBuffer[pos] = ( (ratio > threshold) || (ratio < -threshold ) );
                            else
                                outBuffer[pos] = ratio;
                        }
                    }
                }
                outBand->WriteBlock(xBlock, yBlock, outBuffer);
            }
        }
        GDALClose(outDataset);
        delete[] outBuffer;
        outBuffer = nullptr;

        delete d1Buffer;
        d1Buffer = nullptr;

        delete d2Buffer;
        d2Buffer = nullptr;
    }
    return 0;
}


int LogRatio::init() {

    try {
        if ((polarization != "vh") &&  (polarization != "vv"))
            throw(1);
    }
    catch (int e) {
        cerr <<"wrong polarization value, program will exit\n";
        return e;
    }
    string d1Projection, d2Projection;

    d1Projection = dateImage1->GetProjectionRef();
    d2Projection = dateImage2->GetProjectionRef();

    if (d1Projection != d2Projection) {
        cerr <<"Different projection system between the two images. Exiting\n";
        exit(1);
    }

    double d1GeoTransform[6];
    double d2GeoTransform[6];
    dateImage1->GetGeoTransform(d1GeoTransform);
    dateImage2->GetGeoTransform(d2GeoTransform);

    try {
        for (register int i = 0; i < 6; i++) {
            if (d1GeoTransform[i] != d2GeoTransform[i])
               throw(2);
        }
    }
    catch (int e) {
        cerr <<"Image extents are not the same! exiting\n";
        return e;
    }
    imageWidth = dateImage1->GetRasterXSize();
    imageHeight = dateImage1->GetRasterYSize();
    int ret = createOutputDataset();

    for (register int i = 1; i <= dateImage1->GetRasterCount(); i++) {
        string d1Description = dateImage1->GetRasterBand(i)->GetDescription();
        boost::algorithm::to_lower(d1Description);
        if (d1Description.find(polarization) != std::string::npos)
            d1BandNumber = i;
    }

    for (register int i = 1; i <= dateImage2->GetRasterCount(); i++) {
        string d2Description = dateImage2->GetRasterBand(i)->GetDescription();
        boost::algorithm::to_lower(d2Description);
        if (d2Description.find(polarization) != std::string::npos)
            d2BandNumber = i;
    }
    //cout << d1BandNumber <<"\t" << d2BandNumber << endl;

    if (ret != 0)
        return ret;
    return 0;
}


LogRatio::~LogRatio() {
    if(dateImage1 != nullptr)
        GDALClose(dateImage1);

    if (dateImage2 != nullptr)
        GDALClose(dateImage2);
}
