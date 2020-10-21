import os

def path_leaf(path):
        _, path_tail = os.path.split(path)
        return os.path.splitext(path_tail)[0]