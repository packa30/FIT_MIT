/*Author : Peter Grofčík, xgrofc00*/
#include <mpi.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <tgmath.h>
#include <cmath>

using namespace std;

int main(int argc, char *argv[]){
  int cpu=8;
  int id;
  double starttime, endtime;//time counts for experiments
  MPI_Init(&argc,&argv);
  bool measure;
  if(argv[1] == "measure"){
    measure = true;
  }else{
    measure = false;
  }

  // MPI_Comm_size(MPI_COMM_WORLD, &cpu);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);

  //Nacitanie suboru numbers
  if(id == 0){
      char c;
      int input, cpuIn = 0;
      ifstream fin;
      double startarr[argc-1];



      double angle[argc-1];
      double maxangle[argc-1];

      for (size_t i = 1; i < argc; i++) {
        startarr[i-1] = atof(argv[i]);
      }

      angle[0]=0;
      maxangle[0]=0;
      for (size_t i = 1; i < cpu; i++) {
        angle[i] = atan((startarr[i] - startarr[0])/i);
        maxangle[i] = atan((startarr[i] - startarr[0])/i);
      }

      // for (size_t i = 0; i < cpu; i++) {
      //   std::cout << startarr[i] << '\t';
      //   std::cout << maxangle[i] << '\n';
      // }

      int loga = log2(cpu)-1;
      for (int k = 0; k <= loga; k++) {
        for (int i = 0; i < cpu; i+=pow(2,(k+1))) {
          maxangle[i+(int)pow(2,(k+1))-1]=max(maxangle[i+(int)pow(2,k)-1],maxangle[i+(int)pow(2,(k+1))-1]);
        }
      }

      // for (size_t i = 0; i < cpu; i++) {
      //   std::cout << maxangle[i] << "\t";
      // }
      // std::cout << '\n';

      maxangle[cpu-1]=0;
      for (int k = loga; k >= 0; k--) {
        for (int i = 0; i < cpu; i+=pow(2,(k+1))) {
          double t = maxangle[i+(int)pow(2,k)-1];
          maxangle[i+(int)pow(2,k)-1]=maxangle[i+(int)pow(2,(k+1))-1];
          maxangle[i+(int)pow(2,(k+1))-1]=max(t,maxangle[i+(int)pow(2,(k+1))-1]);
        }
      }

      for (size_t i = 0; i < cpu; i++) {
        std::cout << maxangle[i] << "\t";
      }
      std::cout << '\n';



      printf("_,");
      for (size_t i = 1; i < cpu; i++) {
        if(angle[i] > maxangle[i]){
          printf("v");
        }
        else{
          printf("u");
        }
        if (i<cpu-1) {
          printf(",");
        }
        else
        printf("\n");
      }



      //
      // if (!measure) {
      //   for (int i = 0; i < argc-1; i++) {
      //     cout<<startarr[i]<<" ";
      //   }
      //   std::cout << '\n';
      //
      // }else{
      //   starttime = MPI_Wtime();
      // }
      for (int i = 0; i < cpu ; i++) {
        // MPI_Send(&startarr[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD);
      }
  }


  // if(id == 0){
  //   double final[cpu*2];
  //   final[0] = maxangle[0];
  //   final[1] = maxangle[1];
  //   for (int i = 2; i < cpu*2; i+=2){
  //     MPI_Status status;
  //     MPI_Recv(&final[i], 1, MPI_DOUBLE, i/2, 0, MPI_COMM_WORLD, &status);
  //     MPI_Recv(&final[i+1], 1, MPI_DOUBLE, i/2, 0, MPI_COMM_WORLD, &status);
  //   }
  //   for (int i = 0; i < cpu*2; i++){
  //     std::cout << final[i] << ",";
  //   }
  //   std::cout << '\n';
  //
  // }else{
  //   double fin[2];
  //   fin[0] = angle[0] > maxangle[0] ? 1 : 0;
  //   fin[1] = angle[1] > maxangle[1] ? 1 : 0;
  //
  //   MPI_Send(&maxangle[0], 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
  //   MPI_Send(&maxangle[1], 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
  // }
  MPI_Finalize();
}
