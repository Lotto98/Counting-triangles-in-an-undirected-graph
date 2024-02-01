#ifndef ListsUGraph_H
#define ListsUGraph_H

#include <vector>
#include <mutex>

using PairsList = std::vector<std::pair<size_t,size_t>>;
using Lists = std::vector<std::vector<size_t>>;
using Triple = std::tuple<unsigned long long,unsigned long long,unsigned long long>;

class ListsUGraph
{
private:
    size_t n_nodes=0;
    size_t n_edges=0;

    //read edges from file
    PairsList edges;

    //creates lists data structure, populates it and sorts every list
    static void _populate_lists(Lists &lists,
                                PairsList &edges,
                                std::vector<std::mutex> &mutexes,
                                size_t id=0, size_t skip=1);

    //count the number of triangles
    static unsigned long long _count_triangles( Lists &lists,
                                                PairsList &edges,
                                                size_t id=0, size_t skip=1);

public:
    //create edges data structure from file
    ListsUGraph(std::string dirname);

    //print edges
    void print_edges(bool to_file);

    //print graph variables and stats
    void print_variables();

    //count triangle of this object (single thread)
    Triple count_triangles();

    //count triangle of this object (multi thread)
    Triple count_triangles_multi(size_t n_threads,bool parallel_construction);
};

#endif