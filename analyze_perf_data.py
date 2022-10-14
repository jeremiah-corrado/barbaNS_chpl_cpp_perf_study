import os
import re
import matplotlib.pyplot as plt

test_files = ["nsStep7", "nsStep89"]
weak_scaling_sizes = [[256, 256], [512, 512], [1024, 1024]]

def get_exp_key(ps):
    return "{}x{}".format(ps[0], ps[1])

for test_file in test_files:
    strong_runtimes = {}
    strong_variances = {}
    strong_counts = {}

    weak_runtimes = {}
    weak_variances = {}

    def init_strong_exp(key):
        if key not in strong_runtimes:
            strong_runtimes[key] = {"cpp": {}, "chpl": {}}
            strong_variances[key] = {"cpp": {}, "chpl": {}}
            strong_counts[key] = []

    def try_init_weak_exp(ps, np):
        for wss in weak_scaling_sizes:
            if ps[0] / np == wss[0] and ps[1] / np == wss[1]:
                weak_key = get_exp_key(wss)
                if weak_key not in weak_runtimes:
                    weak_runtimes[weak_key] = {"cpp": {}, "chpl": {}}
                    weak_variances[weak_key] = {"cpp": {}, "chpl": {}}
                return (True, weak_key)
        return (False, "")

    print('Digesting Files...')
    for fname in os.listdir('./dat/'):
        if 'strong' in fname and test_file in fname:
            print("\t", fname)

            exp = fname.split('_')
            ps = [int(exp[1]), int(exp[2])]
            np = int(exp[3])
            lang = exp[5].split('.')[0]
            exp_key = get_exp_key(ps)

            init_strong_exp(exp_key)
            (is_weak_exp, weak_key) = try_init_weak_exp(ps, np)

            cnt = 0
            sums = [0.0, 0.0, 0.0] # [real, user, sys]
            means = []
            stds = []

            with open('./dat/' + fname, 'r') as file:
                for line in file.readlines():
                    for (i, match) in enumerate(re.finditer("(\d\.\d+)", line)):
                        sums[i] += float(match.group(1))
                    cnt += 1

                means = [s / cnt for s in sums]
            sums = [0.0, 0.0, 0.0]

            with open('./dat/' + fname, 'r') as file:
                for line in file.readlines():
                    for (i, match) in enumerate(re.finditer("(\d\.\d+)", line)):
                        sums[i] += (means[i] - float(match.group(1)))**2

                stds = [s**(0.5) / cnt for s in sums]

            print('\t\treal: {0:0.5f} \t {0:0.5f}'.format(means[0], stds[0]))
            print('\t\tuser: {0:0.5f} \t {0:0.5f}'.format(means[1], stds[1]))
            print('\t\tsys:  {0:0.5f} \t {0:0.5f}'.format(means[2], stds[2]))

            # only storing walltimes for now
            strong_runtimes[exp_key][lang][str(np)] = means[0]
            strong_variances[exp_key][lang][str(np)] = stds[0]
            strong_counts[exp_key].append(cnt)

            if is_weak_exp:
                print("\t\t\t(including in weak trend)")
                weak_runtimes[weak_key][lang][str(np)] = means[0]
                weak_variances[weak_key][lang][str(np)] = stds[0]

    print("Generating Strong-Scaling Plots...")
    for prob_size_key, lang_data in strong_runtimes.items():
        print("\t", prob_size_key)

        fig = plt.figure()
        ax = plt.axes()

        for lang_key, means in lang_data.items():
            nps = [int(k) for k in list(means.keys())]
            mns = list(means.values())
            vrs = list(strong_variances[prob_size_key][lang_key].values())

            nps, mns, vrs = list(zip(*sorted(zip(nps, mns, vrs))))

            plt.errorbar(nps, mns, yerr=vrs, fmt='-o', markersize=8, label=lang_key)
            ax.set_xticks(list(nps))

        cnt = max(set(strong_counts[prob_size_key]), key=strong_counts[prob_size_key].count)
        ax.set_ylabel('Mean Walltime - sec (n={})'.format(cnt))
        ax.set_xlabel('Num Tasks')
        ax.set_title('Strong Scaling ' + prob_size_key + ' ' + test_file)
        ax.legend()

        plt.savefig('./figures/strong_' + prob_size_key + '_' + test_file + '.png')
        plt.close()

    print("Generating Weak-Scaling Plots...")
    for prob_size_key, lang_data in weak_runtimes.items():
        print("\t", prob_size_key)

        fig = plt.figure()
        ax = plt.axes()

        for lang_key, means in lang_data.items():
            nps = [int(k) for k in list(means.keys())]
            mns = list(means.values())
            vrs = list(weak_variances[prob_size_key][lang_key].values())

            nps, mns, vrs = list(zip(*sorted(zip(nps, mns, vrs))))

            plt.errorbar(nps, mns, yerr=vrs, fmt='-o', markersize=8, label=lang_key)
            ax.set_xticks(list(nps))

        ax.set_ylabel('Mean Walltime - sec')
        ax.set_xlabel('Num Tasks')
        ax.set_title('Weak Scaling (' + prob_size_key + ' per task) ' + test_file)
        ax.legend()

        plt.savefig('./figures/weak_' + prob_size_key + '_' + test_file + '.png')
        plt.close()
