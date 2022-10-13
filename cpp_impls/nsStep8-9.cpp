#include "nsStep8-9.h"

int main(int argc, const char *argv[]) {
    // define default values of constants
    unordered_map<string, variant<int, double>> default_args = {
            {"xLen", 2.0},
            {"yLen", 1.0},
            {"nx", 31},
            {"ny", 31},
            {"l1_tol", 1e-4},
            {"max_iters", 10000}
    };
    // overwrite defaults with command line arguments if present
    parseArgsWithDefaults(argc, argv,default_args);

    // set final values of simulation constants
    const double xLen   = get<double>(default_args.at("xLen")),
                 yLen   = get<double>(default_args.at("yLen")),
                 l1_tol = get<double>(default_args.at("l1_tol"));
    const int nx        = get<int>(default_args.at("nx")),
              ny        = get<int>(default_args.at("ny")),
              max_iters = get<int>(default_args.at("max_iters"));

    const double dx = xLen / (nx - 1),
                 dy = yLen / (ny - 1);

    // define the 2D array of pressure values
    vector<vector<double> > p(nx, vector<double>(ny, 0.0));
    vector<vector<double> > b(nx, vector<double>(ny, 0.0));

    // apply initial boundary conditions and define sinks/sources
    for (int j = 0; j < ny; j++) {
        p[nx-1][j] = j * dy;
    }
    pBoundary(p);
    b[3*nx/4][ny/4] =  500;
    b[nx/4][3*ny/4] = -500;

#ifdef CREATEPLOTS
    printAndPlot(p, xLen, yLen, "./results/nsStep9cpp_ics", "ICs", "pressure");
#endif

    solvePoisson(p, b, dx, dy, l1_tol, max_iters);

#ifdef CREATEPLOTS
    printAndPlot(p, xLen, yLen, "./results/nsStep9cpp_sol", "Sol", "pressure");
#endif

    return 0;
}

// Iteratively solve Laplace's equation on 'p'
void solvePoisson(
    vector<vector<double> >& p,
    vector<vector<double> >const& b,
    const double dx, const double dy,
    const double tolerance,
    const int max_iters
) {
    // setup temporary simulation variables
    int i = 0;
    auto pn = p;

#ifdef TERMONTOL
    double l1_delta = 1;
    // run until the tolerance condition is met
    while (l1_delta > tolerance && i < max_iters) {
        p.swap(pn);
        poissonKernel(p, pn, b, dx, dy);
        pBoundary(p);

        // compute the tolerance condition and increment 'i'
        l1_delta = l1Delta(p, pn);
        i++;
    }
#else
    for (i = 0; i < max_iters; i++) {
        p.swap(pn);
        poissonKernel(p, pn, b, dx, dy);
        pBoundary(p);
    }
#endif

    cout << "Ran for " << i << " iterations\n";

#ifdef TERMONTOL
    printf("Final L1-Delta: %.8f\n", l1_delta);
#endif
}

void poissonKernel(
    vector<vector<double> >& p,
    vector<vector<double> >const& pn,
    vector<vector<double> >const& b,
    const double dx, const double dy
) {
    #pragma omp parallel for default(none) shared(p, pn, b) firstprivate(dx, dy) collapse(2)
    for (int i = 1; i < p.size() - 1; i++) {
        for (int j = 1; j < p[0].size() - 1; j++) {
            p[i][j] = (
                pow(dx, 2) * (pn[i][j+1] + pn[i][j-1]) +
                pow(dy, 2) * (pn[i+1][j] + pn[i-1][j]) -
                b[i][j] * pow(dx, 2) * pow(dy, 2)
            ) / (2.0 * (pow(dx, 2) + pow(dy, 2)));
        }
    }
}

// Apply boundary conditions to 'p'
void pBoundary(vector<vector<double> >& p) {
    const auto nx = p.size();
    const auto ny = p[0].size();

    #pragma omp parallel for default(none) shared(p) firstprivate(ny, nx)
    for (int i = 0; i < nx; i++) {
        p[i][0]    = p[i][1];      // p(x, y=0.0)  = 0
        p[i][ny-1] = p[i][ny-2];   // p(x, y=yLen) = 0
    }
}

// Compute the relative change in the solutions L1-Norm from the last iteration to this one
double l1Delta(vector<vector<double> > const& p, vector<vector<double> > const& pn) {
    double diffSum = 0;
    double nSum = 0;

    #pragma omp parallel for default(none) shared(p, pn) reduction(+:diffSum, nSum) collapse(2)
    for (int i = 0; i < p.size(); i++) {
        for (int j = 0; j < p[0].size(); j++) {
            diffSum += abs(p[i][j]) - abs(pn[i][j]);
            nSum += abs(pn[i][j]);
        }
    }

    return diffSum / nSum;
}
