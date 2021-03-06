import numpy as np
import sys
import os
import matplotlib.pyplot as plt
import glob

from utils import path_leaf

IMAGES_DIR = './images'
IMAGES_ADJUSTED_DIR = './images_adjusted'

def plot_and_save(data, title, output_dir, percentile):

    # create dir if needed
    if not os.path.exists(output_dir) or not os.path.isdir(output_dir):
        os.mkdir(output_dir, mode=755)

    # plot and format
    plt.hist(data, range = (0,np.percentile(data, percentile)))

    plt.title(title)
    plt.ylabel('Count')
    plt.xlabel('Ticks')
    plt.xticks(rotation=45)

    # save out
    plt.savefig(os.path.join(output_dir, title), dpi = 300, bbox_inches='tight')
    plt.close()

if __name__ == "__main__":

    # get data dir path from argv
    if(len(sys.argv) != 2):
        sys.exit(1)

    data_dir = sys.argv[1]
    if(not os.path.exists(data_dir) or not os.path.isdir(data_dir)):
        print("path '" + data_dir + "' is not a valid directory!")
        sys.exit(1)

    path = os.path.join(data_dir, "*B.csv")

    #Spread out the values across the histogram by taking out the top 99.5 percentile (removing high outliers)
    for fname in glob.glob(path):
        data = np.loadtxt(fname, delimiter=",", dtype=np.uint64)
        buffer_name = path_leaf(fname)
        plot_and_save(data, buffer_name, IMAGES_DIR, 100)
        plot_and_save(data, buffer_name, IMAGES_ADJUSTED_DIR, 99.5)