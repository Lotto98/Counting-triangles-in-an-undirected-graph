//g++ -std=c++11 -O3 *.cpp -o test && ./test > test_output.txt

// ListsUGraph
#include "ListsUGraph.h"

// input/output
#include <iostream>
#include <fstream>

#define N_THREADS 20 //max thread to use    
#define N_ITERATION 50 //test iterations

// write arrays to file.
void to_txt(unsigned long long construction_times[N_THREADS],
            unsigned long long algorithm_times[N_THREADS],
            unsigned long long seq_construction_total_times[N_THREADS],
            std::string graph_name){

    std::ofstream file_const("results/"+graph_name+"_construction"+".txt");
    std::ofstream file_algo("results/"+graph_name+"_algo"+".txt");
    std::ofstream file_total_seq_const("results/"+graph_name+"_total_seq_const"+".txt");

    for (size_t i = 0; i < N_THREADS; i++){
        
        file_const << construction_times[i] << ' ';
        file_algo << algorithm_times[i] << ' ';
        file_total_seq_const << seq_construction_total_times[i] << ' ';
    }
}
 
// test the given graph with N_ITERATIONS from 1 to N_THREADS threads.
void test(std::string graph_name){

    std::cout<<graph_name<<std::endl;

    ListsUGraph ug(graph_name);
    ug.print_variables();

    unsigned long long triangles,construction_time,algorithm_time;
    unsigned long long construction_times[N_THREADS]={0};
    unsigned long long algorithm_times[N_THREADS]={0};
    unsigned long long seq_construction_total_times[N_THREADS]={0};
    
    for (size_t j = 0; j < N_ITERATION; j++){

        std::cout <<"Iteration "<<j<<std::endl;

        std::tie(triangles,construction_time,algorithm_time)=ug.count_triangles();

        construction_times[0]+=construction_time;
        algorithm_times[0]+=algorithm_time;

        seq_construction_total_times[0]+=construction_time+algorithm_time;
        
        std::cout <<"N° triangles single core: "<<triangles <<std::endl;

        for (unsigned int i = 2; i <= N_THREADS; i++){

            // full parallel
            std::tie(triangles,construction_time,algorithm_time)=ug.count_triangles_multi(i,true);

            construction_times[i-1]+=construction_time;
            algorithm_times[i-1]+=algorithm_time;

            std::cout <<"N° triangles "<<i<<" threads: "<<triangles <<std::endl;

            // sequential construction
            std::tie(triangles,construction_time,algorithm_time)=ug.count_triangles_multi(i,false);

            seq_construction_total_times[i-1]+=construction_time+algorithm_time;

            std::cout <<"N° triangles (seq construction) "<<i<<" threads: "<<triangles <<std::endl;
        }
        std::cout<<std::endl;
    }
    
    std::cout<<std::endl;
    
    for (size_t i = 0; i < N_THREADS; i++){
        construction_times[i]=construction_times[i]/N_ITERATION;
        algorithm_times[i]=algorithm_times[i]/N_ITERATION;
        seq_construction_total_times[i]=seq_construction_total_times[i]/N_ITERATION;

        if (i==0) std::cout<<"*** Single core ***"<<std::endl<<std::endl;
        if(i==1) std::cout<<"*** Multi core ***"<<std::endl<<std::endl;

        std::cout<<"N° threads: "<<i+1<<std::endl;
        std::cout<<"Construction elapsed time: "<< construction_times[i] <<"ms or "<<double(construction_times[i])/1000.0<<"s"<<std::endl;
        std::cout <<"Triangle counting elapsed time: "<< algorithm_times[i] <<"ms or "<<double(algorithm_times[i])/1000.0<<"s"<<std::endl;
        std::cout <<"Total elapsed time: "<< algorithm_times[i]+construction_times[i] <<"ms or "<<double(algorithm_times[i]+construction_times[i])/1000.0<<"s"<<std::endl;

        std::cout <<"Total elapsed time (seq construction): "<< seq_construction_total_times[i] <<"ms or "<<double(seq_construction_total_times[i])/1000.0<<"s"<<std::endl;
        std::cout<<std::endl;
    }
    std::cout<<std::endl<<std::endl;

    to_txt(construction_times,algorithm_times,seq_construction_total_times,graph_name);
}

void test_all(){

    std::vector<std::string> graph_names={"facebook","oregon1","oregon2","45100_sparse_graph","4039_dense_graph"};

    for (std::string graph_name:graph_names){
        test(graph_name);
    }
}

int main(int argc, char const *argv[]){
    
    test_all();

    // example of usage
    /*
    ListsUGraph ug("facebook");

    unsigned long long triangles,construction_time,algorithm_time;

    std::tie(triangles,construction_time,algorithm_time)=ug.count_triangles_multi(800,false);
    std::cout<<triangles<<" "<<construction_time<<" "<<algorithm_time<<std::endl;

    std::tie(triangles,construction_time,algorithm_time)=ug.count_triangles_multi(643,true);
    std::cout<<triangles<<" "<<construction_time<<" "<<algorithm_time<<std::endl;

    std::tie(triangles,construction_time,algorithm_time)=ug.count_triangles();
    std::cout<<triangles<<" "<<construction_time<<" "<<algorithm_time<<std::endl;
    */
    return 0;
}
