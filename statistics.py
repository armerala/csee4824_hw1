import numpy as np
import sys
import os
import matplotlib.pyplot as plt
import glob
import pandas as pd
from collections import OrderedDict

from utils import path_leaf

if __name__ == "__main__":
    # get data dir path from argv
    if(len(sys.argv) != 2):
        sys.exit(1)

    data_dir = sys.argv[1]
    if(not os.path.exists(data_dir) or not os.path.isdir(data_dir)):
        print("path '" + data_dir + "' is not a valid directory!")
        sys.exit(1)

    data = {}
    path = os.path.join(data_dir, "*B.csv")
    for fname in glob.glob(path):

        buffer_data = np.loadtxt(fname, delimiter=",")
        buffer_data = np.sort(buffer_data).astype(np.uint64)
        buffer_name = path_leaf(fname) 

        #get statistics after removing outliers
        percentile_value = np.percentile(buffer_data, 99.5, interpolation = 'nearest')
        length = len(np.where(buffer_data == percentile_value)[0])
        index = np.where(buffer_data == percentile_value)[0][length - 1]
        adjusted_buffer_data = buffer_data[:index]

        average = np.average(buffer_data)
        std = np.std(buffer_data)
        average_adjusted = np.average(adjusted_buffer_data)
        std_adjusted = np.std(adjusted_buffer_data)

        data[buffer_name] = {"average": average, "average_adjusted": average_adjusted, "std": std, "std_adjusted": std_adjusted}
    
    sorted_data = sorted(data.items(), key=lambda item: int(item[0][:-1]))
    data_1 = OrderedDict(sorted_data[len(sorted_data)//2:])
    data_2 = OrderedDict(sorted_data[:len(sorted_data)//2])
    df = pd.DataFrame(data_1)
    print(df)
    df.plot(kind='bar', logy = True)
    plt.tight_layout()
    plt.ylabel('Ticks')
    plt.xlabel('Statistics')
    plt.legend(bbox_to_anchor=(0,1.02,1,0.2), loc="lower left", mode="expand", borderaxespad=0, ncol=4)
    plt.savefig("statistics_1.png", dpi = 300, bbox_inches='tight')
    plt.close()
    df = pd.DataFrame(data_2)
    print(df)
    df.plot(kind='bar', logy = True)
    plt.tight_layout()
    plt.ylabel('Ticks')
    plt.xlabel('Statistics')
    plt.legend(bbox_to_anchor=(0,1.02,1,0.2), loc="lower left", mode="expand", borderaxespad=0, ncol=4)
    plt.savefig("statistics_2.png", dpi = 300, bbox_inches='tight')
    plt.close()






