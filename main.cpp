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
#include <iostream>
#include "logratio.h"
using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 5){
        cout <<"Usage: LogRatioChangeDetection image1 image2 polarization threshold output_file \n"
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
