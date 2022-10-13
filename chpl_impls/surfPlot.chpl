import Subprocess.spawn;

// Print 'a' to '{path}.dat' and the domain information to '{path}.meta'
// Call plotting script on resultant files
// Requires python3 installation with matplotlib
proc printAndPlot(
    const ref a : [?d] real,
    lens: (real, real),
    path: string,
    plotTitle: string,
    plotKey: string = ""
) throws where d.rank == 2 {
    const nx = d.dim(0).size;
    const ny = d.dim(1).size;

    var metaFile = openwriter(path + ".meta");
    metaFile.writef("%i, %i, %.10dr, %.10dr\n", nx, ny, lens(0), lens(1));
    metaFile.close();

    var dataFile = openwriter(path + ".dat");
    for i in 0..<nx {
        for j in 0..<(ny-1) do dataFile.writef("%.10dr ", a[i, j]);
        dataFile.writef("%.10dr\n", a[i, ny-1]);
    }
    dataFile.close();

    spawn(["Python3", "surfPlot.py", path, plotTitle, plotKey]);
}
