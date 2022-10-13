#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <variant>
#include <unordered_map>

using namespace std;

void printAndPlot(
        vector<vector<double> > const& a,
        double xLen, double yLen,
        string const& path,
        string const& plotTitle,
        string const& plotKey
);

void parseArgsWithDefaults(
        int argc, const char *argv[],
        unordered_map<string, variant<int, double>>& defaults
);

