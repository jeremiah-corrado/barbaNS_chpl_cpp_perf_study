#include "nsStep7.h"

int main(int argc, const char *argv[]) {
    // define default values of constants
    unordered_map<string, variant<int, double>> default_args = {
            {"xLen", 2.0},
            {"yLen", 2.0},
            {"nx", 31},
            {"ny", 31},
            {"nt", 50},
            {"nu", 0.05},
            {"sigma", 0.25}
    };
    // overwrite defaults with command line arguments if present
    parseArgsWithDefaults(argc, argv,default_args);

    // set final values of simulation constants
    const double xLen  = get<double>(default_args.at("xLen")),
                 yLen  = get<double>(default_args.at("yLen")),
                 nu    = get<double>(default_args.at("nu")),
                 sigma = get<double>(default_args.at("sigma"));
    const int nx = get<int>(default_args.at("nx")),
              ny = get<int>(default_args.at("ny")),
              nt = get<int>(default_args.at("nt"));

    const double dx = xLen / (nx - 1),
                 dy = yLen / (ny - 1),
                 dt = sigma * dx * dy / nu;

    // define the 2D density array with initial conditions
    vector<vector<double> > u(nx, vector<double>(ny, 1.0));
    for (int i = floor(0.5 / dx); i < floor(1.0 / dx) + 1; i++) {
        for (int j = floor(0.5 / dy); j < floor(1.0 / dy) + 1; j++) {
            u[i][j] = 2.0;
        }
    }

#ifdef CREATEPLOTS
    printAndPlot(u, xLen, yLen, "./results/nsStep7cpp_ics", "ICs", "density");
#endif

    // run FD computation
    auto un = u;
    for (int n = 0; n < nt; n++) {
        u.swap(un);

        #pragma omp parallel for default(none) shared(u, un) firstprivate(nx, ny, dx, dy, nu, dt)
        for (int i = 1; i < nx - 1; i++) {
            for (int j = 1; j < ny - 1; j++) {
                u[i][j] = un[i][j] +
                        nu * dt / pow(dy, 2) *
                            (un[i-1][j] - 2 * un[i][j] + un[i+1][j]) +
                        nu * dt / pow(dx, 2) *
                            (un[i][j-1] - 2 * un[i][j] + un[i][j+1]);
            }
        }
    }

#ifdef CREATEPLOTS
    printAndPlot(u, xLen, yLen, "./results/nsStep7cpp_sol", "Sol", "density");
#endif

    // compute the standard deviation of 'u'
    double m_sum = 0.0;
    #pragma omp parallel for default(none) shared(u) reduction(+:m_sum)
    for (auto const& row : u) {
        for (auto val: row) {
            m_sum += val;
        }
    }
    double mean = m_sum / (nx * ny);
    double std_sum = 0.0;
    #pragma omp parallel for default(none) shared(u) firstprivate(mean) reduction(+:std_sum)
    for (auto const& row : u) {
        for (auto val: row) {
            std_sum += pow(val - mean, 2);
        }
    }
    double std_dev = pow(std_sum, 0.5) / (nx * ny);
    printf("Final std(u): %.6lf\n", std_dev);

    return 0;
}
