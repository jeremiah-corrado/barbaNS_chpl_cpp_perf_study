#include <iostream>
#include <vector>
#include <cmath>
#include <variant>
#include <unordered_map>

#include <omp.h>
#include "utils.h"

using namespace std;

void solvePoisson(
    vector<vector<double> >& p,
    vector<vector<double> > const& b,
    double dx, double dy,
    double tolerance,
    int max_iters
);

void poissonKernel(
    vector<vector<double> >& p,
    vector<vector<double> >const& pn,
    vector<vector<double> >const& b,
    double dx, double dy
);

void pBoundary(vector<vector<double> >& p);
double l1Delta(const vector<vector<double> >& p, const vector<vector<double> >& pn);
