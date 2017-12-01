#include <iostream>
#include "logratio.h"
using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 5){
        cout <<"Usage: LogRatio image1 image2 polarization threshold output_file \n"
               "polarization values: VV or VH\n"
               "output file should be in .tif format\n";
        return -1;
    }

    cout << "Starting Change Detection\n";
    LogRatio obj(argv[1], argv[2], argv[3], stod(argv[4]), argv[5]);
    int ret = obj.init();
    if (ret == -1)
        return ret;

    ret = obj.compute();
    if (ret == -1)
        return ret;

    cout << "Change Detection Finished\n" << endl;
    return 0;
}
