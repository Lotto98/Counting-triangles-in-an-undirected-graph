import itertools
from math import comb
import os
from sys import argv

import numpy as np


def main():
    
    if len(argv)!=4:
        print("parameter should be 3: graph_name, n_nodes, n_edges")
        return
    
    _,graph_name,n_nodes, n_edges=argv
    
    cb=comb(int(n_nodes),2)
    
    if int(n_edges)>cb:
        print("n_edges should be <="+str(cb))
        return
    
    print("generating mask...")
    pairs=itertools.combinations(range(int(n_nodes)),2)
    mask=np.append(np.repeat(1,int(n_edges)),np.zeros(cb-int(n_edges),np.int8))
    np.random.shuffle(mask)
    
    PATH = os.getcwd()+"/"+graph_name
    
    if not os.path.exists(PATH):
        os.mkdir(PATH)

    print("generating files...")
    with open(graph_name+"/edges.txt", "w") as edges:
        for n,pair in enumerate(pairs):
            if mask[n]: 
                edges.write(str(pair[0])+" "+str(pair[1])+"\n")
        
    with open(graph_name+"/nodes.txt", "w") as nodes:
        nodes.write(n_nodes)
    
    print("END")
if __name__ == '__main__':
    main()