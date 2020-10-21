import sys
import os
import glob
import numpy as np

def usage():
    print("python combine_data.py <data-dir-path>")

if __name__ == "__main__":

    # get data dir path from arvv
    if(len(sys.argv) != 2):
        usage()
        sys.exit(1)

    data_dir = sys.argv[1]
    if(not os.path.exists(data_dir) or not os.path.isdir(data_dir)):
        print("path '" + data_dir + "' is not a valid directory!")
        sys.exit(1)

    # get data file list
    data_fnames = glob.glob(os.path.join(data_dir, "*B.csv"))
    if(len(data_fnames) == 0):
        print("no csv files found in", data_dir + ". Exiting now...")
        sys.exit(0)

    # combine
    header_items = []
    data_combined = None
    for i,data_fname in enumerate(data_fnames):

        print("processing", data_fname)
        _, data_fname_tail = os.path.split(data_fname)
        header_items.append(os.path.splitext(data_fname_tail)[0])

        data = np.loadtxt(data_fname, delimiter=',', dtype=np.uint64)
        if(data_combined is None): # lazy-init when we know a reasonable size
            data_combined = np.empty([data.shape[0], len(data_fnames)], dtype=np.uint64)
        data_combined[:data.shape[0],i] = data

    # gen output file
    print("saving output to combined.csv")
    np.savetxt(os.path.join(data_dir, "combined.csv"), data_combined, delimiter=',', header=','.join(header_items))