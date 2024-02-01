#include "ListsUGraph.h"

// input/output
#include <iostream>
#include <fstream>
#include <sstream>

// parallel execution
#include <future>
#include <thread>

// std::set_intersection, std::upper_bound and std::back_inserter
#include <algorithm>

ListsUGraph::ListsUGraph(std::string dirname){

    std::string line; //buffer

    //read n nodes
    std::ifstream nodes_file("data/"+dirname+"/nodes.txt");
    getline(nodes_file, line);

    std::istringstream ss_nodes(line);
    ss_nodes>>n_nodes;

    nodes_file.close();
    
    //read edges
    std::ifstream edges_file("data/"+dirname + "/edges.txt");

    this->n_edges=0;

    while( getline (edges_file, line) ){
        
        std::istringstream ss_edges(line);
        
        size_t a,b;
        ss_edges>> a;
        ss_edges>> b;

        edges.push_back(std::pair<size_t,size_t>(a,b));

        this->n_edges++;

    }

    edges_file.close();

}

void ListsUGraph::print_edges(bool to_file){
    
    if (to_file){
        std::ofstream file("edges_print.txt");

        for (auto j: edges){
            file << j.first <<' '<< j.second << std::endl;
        }
    }
    else{
        for (auto j: edges){
            std::cout << j.first <<' '<< j.second << std::endl;
        }
    }
}

void ListsUGraph::print_variables(){
    
    std::cout<<"N° of edges "<<this->n_edges<<std::endl;
    std::cout<<"N° of nodes "<<this->n_nodes<<std::endl;

    std::cout<<"Density: "<<double(2*n_edges)/double(n_nodes*(n_nodes-1))<<std::endl<<std::endl;
}

void inline ListsUGraph::_populate_lists(Lists &lists,
                                        PairsList &edges,
                                        std::vector<std::mutex> &mutexes,
                                        size_t id, size_t skip){
    
    // acquire the specific list's lock to insert a new element without conflicts
    for (size_t k = id; k < edges.size(); k+=skip){
        
        size_t i=edges.at(k).first;
        size_t j=edges.at(k).second;

        
        if(!mutexes.empty()) mutexes.at(i).lock();

        lists.at(i).insert(std::upper_bound(lists.at(i).begin(),
                                            lists.at(i).end(),
                                            j),j);

        if(!mutexes.empty()) mutexes.at(i).unlock();

        
        if(!mutexes.empty()) mutexes.at(j).lock();

        lists.at(j).insert(std::upper_bound(lists.at(j).begin(),
                                            lists.at(j).end(),
                                            i),i);

        if(!mutexes.empty()) mutexes.at(j).unlock();

    }
}


unsigned long long inline ListsUGraph::_count_triangles(Lists &lists,
                                                        PairsList &edges,
                                                        size_t id, size_t skip ){

    unsigned long long n_triangles=0;

    for (size_t j = id; j < edges.size(); j+=skip){

        size_t a=edges[j].first;
        size_t b=edges[j].second;

        std::vector<size_t> c;

        set_intersection(lists.at(a).begin(),lists.at(a).end(),
                            lists.at(b).begin(),lists.at(b).end(),
                            back_inserter(c));
        

        n_triangles+=c.size();
    }
    
    return n_triangles;
}

Triple ListsUGraph::count_triangles(){
    
    // list of lists construction
    auto start=std::chrono::high_resolution_clock::now();

    Lists lists(n_nodes);
    std::vector<std::mutex> nullV; //sequential execution

    _populate_lists(lists,edges,nullV);

    auto stop=std::chrono::high_resolution_clock::now();
    auto elapsed=std::chrono::duration_cast<std::chrono::milliseconds>(stop-start);
    unsigned long long construction_time=elapsed.count();


    // counting triangles algorithm
    start=std::chrono::high_resolution_clock::now();

    unsigned long long n_triangles=_count_triangles(lists,edges)/3;

    stop=std::chrono::high_resolution_clock::now();
    elapsed=std::chrono::duration_cast<std::chrono::milliseconds>(stop-start);
    unsigned long long algo_time=elapsed.count(); 


    return Triple(n_triangles, construction_time, algo_time);
}


Triple ListsUGraph::count_triangles_multi(size_t n_threads, bool parallel_construction){

    // list of lists construction
    auto start=std::chrono::high_resolution_clock::now();

    Lists lists(n_nodes);

    if(parallel_construction){

        std::thread threads[n_threads];
        std::vector<std::mutex> mutexes(n_nodes);

        for (size_t i = 0; i < n_threads; i++){
            threads[i]=std::thread(_populate_lists,std::ref(lists),std::ref(edges),std::ref(mutexes),i,n_threads);
        }
        
        for (size_t i = 0; i < n_threads; i++){
            threads[i].join();
        }
    }
    else{
        std::vector<std::mutex> mutexes(0);
        _populate_lists(lists,edges,mutexes);
    }

    auto stop=std::chrono::high_resolution_clock::now();
    auto elapsed=std::chrono::duration_cast<std::chrono::milliseconds>(stop-start);
    unsigned long long construction_time=elapsed.count(); 


    // counting triangles algorithm
    start=std::chrono::high_resolution_clock::now();

    std::future<unsigned long long> future[n_threads]; 

    for (size_t i = 0; i < n_threads; i++){
        future[i]=std::async(std::launch::async,_count_triangles,std::ref(lists),std::ref(edges),i,n_threads);
    }

    unsigned long long n_triangles=0;
    
    for (size_t i = 0; i < n_threads; i++){
        n_triangles+=future[i].get();
    }

    n_triangles/=3;

    stop=std::chrono::high_resolution_clock::now();
    elapsed=std::chrono::duration_cast<std::chrono::milliseconds>(stop-start);
    unsigned long long algo_time=elapsed.count(); 


    return Triple(n_triangles, construction_time, algo_time);
}