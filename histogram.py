import numpy as np
import sys
import os
import matplotlib.pyplot as plt
import glob

if __name__ == "__main__":
# get data dir path from arvv
    if(len(sys.argv) != 2):
        sys.exit(1)

    data_dir = sys.argv[1]
    if(not os.path.exists(data_dir) or not os.path.isdir(data_dir)):
        print("path '" + data_dir + "' is not a valid directory!")
        sys.exit(1)

    path = data_dir + "/*.csv"
    #Spread out the values across the histogram by taking out the top 99.5 percentile (removing high outliers)
    for fname in glob.glob(path):
        data = np.loadtxt(open(fname, "rb"), delimiter=",")
        buffer_name = fname.split('/',1)[1][:-4]
        plt.hist(data, range = (0, np.percentile(data, 99.5)))
        plt.title(buffer_name + " Adjusted")
        plt.ylabel('Count')
        plt.xlabel('Ticks')
        plt.xticks(rotation=45)
        plt.savefig(buffer_name + "_adjusted.png", dpi = 300, bbox_inches='tight')
        plt.close()

    #No adjustment to graph
    for fname in glob.glob(path):
        data = np.loadtxt(open(fname, "rb"), delimiter=",")
        buffer_name = fname.split('/',1)[1][:-4]
        plt.hist(data)
        plt.title(buffer_name)
        plt.ylabel('Count')
        plt.xlabel('Ticks')
        plt.xticks(rotation=45)
        plt.savefig(buffer_name, dpi = 300, bbox_inches='tight')
        plt.close()