from sys import argv
import numpy as np

def main():
    
    if len(argv)!=2:
        print("parameter should be 1: graph_name")
        return
    
    _,graph_name=argv
    node1=np.array([],dtype=int)
    node2=np.array([],dtype=int)
    
    with open(graph_name+"/edges.txt") as edges_file:
        
        for line in edges_file:
        
            if len(line)>0 and line[0]!='#':
                
                words=line.split()
                
                save_line=True
                
                if len(words)!=2:
                    save_line=False
                
                for word in words:
                    if not word.isnumeric():
                        save_line=False
                
                if save_line:
                    a,b=words
                    node1=np.append(node1,int(a))
                    node2=np.append(node2,int(b))
    
    min_index=min(min(node1),min(node2))
    max_index=max(max(node1),max(node2))
    
    new_index=0
    for index in range(min_index,max_index+1):
        if index in node1 or index in node2:
            
            node1[node1==index]=new_index
            node2[node2==index]=new_index
            
            new_index+=1
            
    with open(graph_name+"/edges.txt","w") as edges_file:
        for a,b in zip(node1,node2):
            edges_file.write(str(a)+" "+str(b)+"\n")
            
    with open(graph_name+"/nodes.txt","w") as nodes_file:
        nodes_file.write(str(max(max(node1),max(node2))+1))

if __name__ == '__main__':
    main()