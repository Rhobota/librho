#include <rho/img/tImage.h>
#include <rho/tCrashReporter.h>
#include <rho/eRho.h>

#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace rho;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
using std::string;
using std::vector;


int main(int argc, char* argv[])
{
    tCrashReporter::init();

    if (argc < 2)
    {
        cerr << argv[0] << " <path_to_image> [<path_to_image> ...]" << endl;
        exit(1);
    }

    for (int i = 1; i < argc; i++)
    {
        string path = argv[i];
        img::tImage orig(path, img::kRGB24);

        img::tImage edges;
        orig.sobel(&edges);

        edges.saveToFile(path + ".png");
    }

    cout << "Finished!" << endl;

    return 0;
}
