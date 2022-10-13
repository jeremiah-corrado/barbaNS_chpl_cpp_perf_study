use IO;
use IO.FormattedIO;
import surfPlot.printAndPlot;

// domain-related constants
config const xLen : real = 2,
             yLen : real = 1,
             nx = 31,
             ny = 31;

const dx : real = xLen / (nx - 1),
      dy : real = yLen / (ny - 1);

// simulation settings
config const l1_tol = 1e-4,
             max_iters = 10000;

// compile-time configurations
config param createPlots = true,
             termOnTol = true;

proc main() {
    // define a domain shared by both arrays: p and b
    const dom = {0..<nx, 0..<ny};
    var p : [dom] real = 0.0,   // fluid pressure
        b : [dom] real = 0.0;   // sinks/sources

    // apply initial boundary conditions and define sinks/sources
    p[nx-1, ..] = [j in 0..<ny] j * dy; pBoundary(p);
    b[3*nx/4, ny/4] =  500;
    b[nx/4, 3*ny/4] = -500;

    if createPlots then printAndPlot(p, (xLen, yLen), "results/nsStep9_ics", "ICs", "pressure");
    solvePoisson(p, b);
    if createPlots then printAndPlot(p, (xLen, yLen), "results/nsStep9_sol", "Sol", "pressure");
}

// tolerance terminated solver
proc solvePoisson(ref p :[?d] real, ref b: [] real) where termOnTol == true {
    // temp copy of 'p'
    var pn = p;

    // subdomain of p, b (the inner n-2 indices)
    const domInner : subdomain(d) = d.expand(-1);

    // run until the tolerance condition is met
    var l1_delta = 1.0, i = 0;
    while l1_delta > l1_tol && i < max_iters {
        p <=> pn;
        poissonKernel(p, pn, b, domInner);
        pBoundary(p);

        // compute the tolerance condition and increment 'i'
        l1_delta = (+ reduce (abs(p) - abs(pn))) / (+ reduce abs(pn));
        i += 1;
    }

    writeln("Ran for ", i, " iterations");
    writef("Final L1-Delta: %.8dr\n", l1_delta);
}

// iteration terminated solver
proc solvePoisson(ref p :[?d] real, const ref b: [] real) where termOnTol == false {
    // temp copy of 'p'
    var pn = p;

    // subdomain of p, b (the inner {nx-2, ny-2} indices)
    const domInner : subdomain(d) = d.expand(-1);

    // run for max_iters iterations
    for i in 0..#max_iters {
        p <=> pn;
        poissonKernel(p, pn, b, domInner);
        pBoundary(p);
    }

    writeln("Ran for ", max_iters, " iterations");
}

proc poissonKernel(ref p, const ref pn, const ref b, domInner) {
        forall (i, j) in domInner {
            p[i, j] = (
                dx**2 * (pn[i, j+1] + pn[i, j-1]) +
                dy**2 * (pn[i+1, j] + pn[i-1, j]) -
                b[i, j] * dx**2 * dy**2
            ) / (2.0 * (dx**2 + dy**2));
        }
}

proc pBoundary(p) {
    // p[0,    ..] = 0.0;                   // p(x=0,    y) = 0
    // p[nx-1, ..] = [j in 0..<ny] j * dy;  // p(x=xLen, y) = y
    p[..,    0] = p[..,    1];          // dp/dy(x,    y=0) = 0
    p[.., nx-1] = p[.., nx-2];          // dp/dy(x, y=yLen) = 0
}
