/*Author : Peter Grofčík, xgrofc00*/
#include <mpi.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <tgmath.h>
#include <cmath>

using namespace std;

#ifndef MEASURE
#define MEASURE 0
#endif

//initial redistribution by proc 0 to all other proc for computation
double init (double *input, int cpu, std::array<double,2> &angle){
  double firstVal = input[0];
  angle[0] = 0;
  angle[1] = input[1];
  for (int i = 2; i < cpu*2; i+=2) {
    MPI_Send(&input[0], 1, MPI_DOUBLE, i/2, 0, MPI_COMM_WORLD);
    MPI_Send(&input[i], 1, MPI_DOUBLE, i/2, 0, MPI_COMM_WORLD);
    MPI_Send(&input[i+1], 1, MPI_DOUBLE, i/2, 0, MPI_COMM_WORLD);
  }
  return firstVal;
}

//initial redistribution reciving by all other proc except proc 0
double initRec(std::array<double,2> &angle){
  MPI_Status status;
  double firstVal;
  MPI_Recv(&firstVal, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
  MPI_Recv(&angle[0], 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
  MPI_Recv(&angle[1], 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
  return firstVal;
}

//computation of angle for all processes
//special variant for proc 0 first val to 0
void initAngleCompute (int id, std::array<double,2> &angle, double firstVal){
  if (id == 0) {
    angle[0] = 0;
  }else{
    angle[0] = atan((angle[0] - firstVal)/(id*2));
  }
  angle[1] = atan((angle[1] - firstVal)/(id*2+1));
}

//sweepUP alg. doing max between recived and owned value by proc. that is recieving value on exact heigh of simulated tree
//cycle over the heigh of a simulated tree with special variant for height 0 where all the proceses work with their own values
//cycle -> way of isolationg proc that should work on exact height, because each iteration of first cycle divide number of active proc by 2
void sweepUp(int logD, int id, int cpu, std::array<double,2> &maxangle){
  MPI_Status status;
  for (int k = 0; k <= logD; k++) {
    if (k == 0) {
      maxangle[1] = max(maxangle[0],maxangle[1]);
    }else{
      for (int i = 0; i < cpu; i+=pow(2,k)) {
        if( id == i+(int)pow(2,k)-1){//pravy vzdy prijma od laveho
          double reciving;
          MPI_Recv(&reciving, 1, MPI_DOUBLE, i+(int)pow(2,k-1)-1, 0, MPI_COMM_WORLD, &status);
          maxangle[1] = max(maxangle[1],reciving);
        }else if(id == i+(int)pow(2,k-1)-1){//lavy odosiela pravemu
          MPI_Send(&maxangle[1], 1, MPI_DOUBLE, i+(int)pow(2,k)-1, 0, MPI_COMM_WORLD);
        }
        else{//nothing to do here
        }
      }
    }
  }
}

//sweepDown alg. doing max between recived and owned value by proc. that is recieving value on exact heigh of simulated tree(root and also right child for next iteration)
//left child just recieve and overwrite its own value
//cycle over the heigh of a simulated tree with special variant for height 0 where all the proceses work with their own values (simulating value 0 as left child value 1 as right and also root)
//cycle -> way of isolationg proc that should work on exact height, because each iteration of first cycle multiply number of active proc by 2
//also presets first root value to 0
void sweepDown(int logD, int id, int cpu, std::array<double,2> &maxangle){
  MPI_Status status;
  if(id == (cpu-1)){
    maxangle[1]=0;
  }
  for (int k = logD; k >= 0; k--) {
    if(k == 0){
      double sequence = maxangle[0];
      maxangle[0]=maxangle[1];
      maxangle[1]=max(sequence,maxangle[1]);
    }else{
      for (int i = 0; i < cpu; i+=pow(2,k)){
        if( id == i+(int)pow(2,k)-1){
          double reciving;
          MPI_Send(&maxangle[1], 1, MPI_DOUBLE, i+(int)pow(2,(k-1))-1, 0, MPI_COMM_WORLD);
          MPI_Recv(&reciving, 1, MPI_DOUBLE, i+(int)pow(2,(k-1))-1, 0, MPI_COMM_WORLD, &status);
          maxangle[1] = max(maxangle[1],reciving);
        }else if(id == i+(int)pow(2,(k-1))-1){
          double reciving;
          MPI_Recv(&reciving, 1, MPI_DOUBLE, i+(int)pow(2,k)-1, 0, MPI_COMM_WORLD, &status);
          MPI_Send(&maxangle[1], 1, MPI_DOUBLE, i+(int)pow(2,k)-1, 0, MPI_COMM_WORLD);
          maxangle[1] = reciving;
        }
        else{//nothing to do here
        }
    }
    }
  }
}


//finally proc 0 obtains values from all the processes for outputing
//timer has beed stoped already, because as for measurement this is not a part of alg.
void finalize(int id, int cpu, std::array<double,2> &fin, int count){
  if(id == 0){
    double final[cpu*2];
    final[0] = 1;
    final[1] = fin[1];
    for (int i = 2; i < cpu*2; i+=2){
      MPI_Status status;
      MPI_Recv(&final[i], 1, MPI_DOUBLE, i/2, 0, MPI_COMM_WORLD, &status);
      MPI_Recv(&final[i+1], 1, MPI_DOUBLE, i/2, 0, MPI_COMM_WORLD, &status);
    }
    if (!MEASURE) {
      std::cout << "_";
      for (int i = 1; i < count; i++) {
        if (final[i]) {
          std::cout << ",v";
        }else{
          std::cout << ",u";
        }
      }
      std::cout << '\n';
    }
  }else{
    MPI_Send(&fin[0], 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    MPI_Send(&fin[1], 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
  }
}

int main(int argc, char *argv[]){
  int cpu;
  int id;
  double starttime, endtime;//time counts for experiments
  MPI_Init(&argc,&argv);
  std::array<double,2>  angle;
  double firstVal;

  MPI_Comm_size(MPI_COMM_WORLD, &cpu);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);

  int logD = log2(cpu*2)-1;

  //load values from input param just by proc 0
  //redistributes afterwards
  if(id == 0){
    double startarr[cpu*2];
    int i = 1;
    while (i <= cpu*2) {
      if(i<argc){
        startarr[i-1] = atof(argv[i]);
      }
      else{
        startarr[i-1] = 0.0;
      }
      i++;
    }
    firstVal=init(startarr, cpu, angle);
  }else{
    firstVal=initRec(angle);
  }
  MPI_Barrier(MPI_COMM_WORLD);//wait untill truly every process is ready to start computation
  if (MEASURE && id == 0) {
    starttime = MPI_Wtime();
  }
  initAngleCompute (id, angle, firstVal);
  std::array<double,2>  maxangle=angle;
  sweepUp(logD, id, cpu, maxangle);
  sweepDown(logD, id, cpu, maxangle);

  //each proc compares its own angle and maxangle values
  std::array<double,2>  fin;
  fin[0] = angle[0] > maxangle[0] ? 1 : 0;
  fin[1] = angle[1] > maxangle[1] ? 1 : 0;
  MPI_Barrier(MPI_COMM_WORLD);//wait untill truly every process ended with computation
  if (MEASURE && id == 0) {
    endtime = MPI_Wtime();
    std::cout << (size_t)((endtime-starttime)*1000000) << '\n';
  }
  finalize(id, cpu, fin, argc-1);//final redistribution to process 0 and output values

  MPI_Finalize();
}
