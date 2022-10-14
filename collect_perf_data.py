import os

size_scale_factor = 16
samples_per_experiment = 1
test_files = ["nsStep7", "nsStep89"]

problem_sizes = [
    [512, 512], [1024, 1024], [2048, 2048], [4096, 4096],
    [64, 512], [128, 2048], [256, 4096],
    [512, 64], [2048, 128], [4096, 256]
]
num_procs = [2, 4, 8, 16]

os.environ["CHPL_RT_NUM_THREADS_PER_LOCALE_QUIET"]="yes"
for test_file in test_files:

    # collect execution time data
    for ps in problem_sizes:
        args = "--nx={} --xLen={} --ny={} --yLen={}".format(
            ps[0], ps[0] / size_scale_factor,
            ps[1], ps[1] / size_scale_factor
        )
        if test_file == "nsStep89":
            args = args + " --max_iters=1000"

        for np in num_procs:
            os.environ["OMP_NUM_THREADS"] = "{}".format(np)
            os.environ["CHPL_RT_NUM_THREADS_PER_LOCALE"] = "{}".format(np)

            exp_name = "strong_{}_{}_{}_{}".format(ps[0], ps[1], np, test_file)

            print("Running '", exp_name, "'...")

            for s in range(0, samples_per_experiment):
                os.system("/usr/bin/time -f 'real %e user %U sys %S' -a -o ./dat/{}_cpp.txt ./cpp_impls/release/{} {} > /dev/null".format(exp_name, test_file, args))
                os.system("/usr/bin/time -f 'real %e user %U sys %S' -a -o ./dat/{}_chpl.txt ./chpl_impls/release/{} {} > /dev/null".format(exp_name, test_file, args))
