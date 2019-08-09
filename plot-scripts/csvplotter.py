# NIST-developed software is provided by NIST as a public service. You may use, copy and distribute copies of the
# software in any medium, provided that you keep intact this entire notice. You may improve, modify and create
# derivative works of the software or any portion of the software, and you may copy and distribute such modifications
# or works. Modified works should carry a notice stating that you changed the software and should note the date and
# nature of any such change. Please explicitly acknowledge the National Institute of Standards and Technology as the
# source of the software. NIST-developed software is expressly provided "AS IS." NIST MAKES NO WARRANTY OF ANY KIND,
# EXPRESS, IMPLIED, IN FACT OR ARISING BY OPERATION OF LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR
# WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE
# CORRECTED. NIST DOES NOT WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE SOFTWARE OR THE RESULTS
# THEREOF, INCLUDING BUT NOT LIMITED TO THE CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE. You
# are solely responsible for determining the appropriateness of using and distributing the software and you assume
# all risks associated with its use, including but not limited to the risks and costs of program errors, compliance
# with applicable laws, damage to or loss of data, programs or equipment, and the unavailability or interruption of 
# operation. This software is not intended to be used in any situation where a failure could cause risk of injury or
# damage to property. The software developed by NIST employees is not subject to copyright protection within the
# United States.
import matplotlib.pyplot as plt
import matplotlib.ticker as mtick
import collections
import numpy as np
import csv


def extract_from_csv(path):
    dict_res = collections.OrderedDict()
    block_sizes = []
    mean_runtime = []
    std_runtime = []

    with open(path, 'r') as csv_file:
        plots = csv.reader(csv_file, delimiter=',')
        next(plots)
        for row in plots:
            block_size = row[0]
            block_sizes.append(block_size)
            dict_res[block_size] = []
            for runTime in row[1:]:
                if runTime is not '':
                    dict_res[block_size].append(int(runTime))

    for runtime in dict_res.values():
        mean_runtime.append(np.mean(runtime)/1000000)
        std_runtime.append(np.std(runtime)/1000000)

    return block_sizes, mean_runtime, std_runtime


def main():
    pathHTGS = "/home/anb22/Documents/hedgehog-matrixlib/plot-scripts/result_hadamard_htgs.csv"
    pathHH = "/home/anb22/Documents/hedgehog-matrixlib/plot-scripts/result_hadamard_hedgehog.csv"

    block_sizes_hh, mean_runtime_hh, stdv_runtime_hh = extract_from_csv(pathHH)
    block_sizes_htgs, mean_runtime_htgs, stdv_runtime_htgs = extract_from_csv(pathHTGS)

    ####################################################################################################################

    fig, ax = plt.subplots()
    asymmetric_error_hh = [stdv_runtime_hh, stdv_runtime_hh]
    asymmetric_error_htgs = [stdv_runtime_htgs, stdv_runtime_htgs]

    plt.yscale('log')
    plt.title('Hadamard Product (4096x4096 matrix size) Block Size vs Runtime')
    plt.xlabel('Block size')
    plt.ylabel('Time in seconds')

    ax.errorbar(block_sizes_hh, mean_runtime_hh, yerr=asymmetric_error_hh, label='Hedgehog')
    ax.errorbar(block_sizes_htgs, mean_runtime_htgs, yerr=asymmetric_error_htgs, label='HTGS')
    ax.legend()

    plt.savefig('Hadamard_HHvxHTGS_Runtime.pdf')
    plt.savefig('Hadamard_HHvxHTGS_Runtime.png')
    ####################################################################################################################

    fig2, ax2 = plt.subplots()

    difference_runtime = [hh - htgs for hh, htgs in zip(mean_runtime_hh, mean_runtime_htgs)]
    pos_diff = [diff if diff > 0 else np.nan for diff in difference_runtime]
    neg_diff = [diff if diff <= 0 else np.nan for diff in difference_runtime]

    plt.title("Hadamard Product (4096x4096 matrix size) \nRuntime Difference: (Hedgehog - HTGS)")
    plt.xlabel('Block size')
    plt.ylabel('Runtime difference in seconds')

    ax2.scatter(block_sizes_hh, pos_diff, color='r', label='HTGS runtime > Hedgehog runtime')
    ax2.scatter(block_sizes_hh, neg_diff, color='b', label='HTGS runtime <= Hedgehog runtime')
    ax2.legend()

    for i, j in zip(block_sizes_hh, difference_runtime):
        ax2.annotate(str(round(j, 2)), xy=(i, j + 0.2))

    plt.savefig('Hadamard_HHvxHTGS_Runtime_Difference.pdf')
    plt.savefig('Hadamard_HHvxHTGS_Runtime_Difference.png')
    ####################################################################################################################

    fig3, ax3 = plt.subplots()
    relative_change = [((hh - htgs) / htgs)*100 for hh, htgs in zip(mean_runtime_hh, mean_runtime_htgs)]

    plt.title("Hadamard Product (4096x4096 matrix size) \nRelative change: (Hedgehog - HTGS) / HTGS")
    plt.xlabel('Block size')
    plt.ylabel('Relative change')

    fmt = '%.0f%%'  # Format you want the ticks, e.g. '40%'
    xticks = mtick.FormatStrFormatter(fmt)
    ax3.yaxis.set_major_formatter(xticks)
    ax3.plot(block_sizes_hh, relative_change, label="Relative change")
    ax3.plot(block_sizes_hh, [0] * len(block_sizes_hh), linestyle=":")
    ax3.legend()

    for i, j in zip(block_sizes_hh, relative_change):
        ax3.annotate(str(round(j, 2)), xy=(i, j))

    plt.savefig('Hadamard_HHvxHTGS_RelativeChange.pdf')
    plt.savefig('Hadamard_HHvxHTGS_RelativeChange.png')
    ####################################################################################################################
    plt.show()


if __name__ == "__main__":
    main()
