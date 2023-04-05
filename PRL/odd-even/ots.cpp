/*Author : Peter Grofčík, xgrofc00*/
#include <mpi.h>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

int main(int argc, char *argv[]){
  int cpu;
  int id;
  double starttime, endtime;//time counts for experiments
  MPI_Init(&argc,&argv);
  bool measure;
  if(argc > 1){
    measure = true;
  }else{
    measure = false;
  }

  MPI_Comm_size(MPI_COMM_WORLD, &cpu);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);

  //Nacitanie suboru numbers
  if(id == 0){
      char c;
      int input, cpuIn = 0;
      ifstream fin;
      fin.open("numbers");
      int startarr[cpu];
      while(input = fin.get()){
          if (fin.eof()) break;
          startarr[cpuIn] = input;
          cpuIn++;
      }
      fin.close();
      if (!measure) {
        for (int i = 0; i < cpu; i++) {
          cout<<startarr[i]<<" ";
        }
        std::cout << '\n';

      }else{
        starttime = MPI_Wtime();
      }
      for (int i = 0; i < cpu ; i++) {
        MPI_Send(&startarr[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD);
      }
  }

  int inputId, recivedNum;
  int even = 2*(cpu/2)-1;
  int odd = 2*((cpu-1)/2);
  int cycles = cpu/2;
  if (cpu % 2) { // pri lychom pocte je potrebne navysit pocet cyklov o 1 -- najmensie cislo na konci by sa nedostalo na prve miesto
    cycles++;
  }
  MPI_Status status;
  MPI_Recv(&inputId, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

  for(int i = 1; i <= cycles; i++){
    if( ((id%2) == 0) && (id < even)){//liche procesory == sudy index => index 0 == 1. procesor
      MPI_Send(&inputId, 1, MPI_INT, id+1, 0, MPI_COMM_WORLD);
      MPI_Recv(&inputId, 1, MPI_INT, id+1, 0, MPI_COMM_WORLD, &status);
    }else if(id <= even){
      MPI_Recv(&recivedNum, 1, MPI_INT, id-1, 0, MPI_COMM_WORLD, &status);
      if (recivedNum > inputId) std::swap(inputId,recivedNum);
      MPI_Send(&recivedNum, 1, MPI_INT, id-1, 0, MPI_COMM_WORLD);
    }else{
    }

    if( ((id%2) != 0) && (id < odd)){//sude procesory == lichy index => index 1 == 2. procesor
      MPI_Send(&inputId, 1, MPI_INT, id+1, 0, MPI_COMM_WORLD);
      MPI_Recv(&inputId, 1, MPI_INT, id+1, 0, MPI_COMM_WORLD, &status);
    }
    else if ( (id <= odd) && (id != 0) ) {
      MPI_Recv(&recivedNum, 1, MPI_INT, id-1, 0, MPI_COMM_WORLD, &status);
      if (recivedNum > inputId) std::swap(inputId,recivedNum);
      MPI_Send(&recivedNum, 1, MPI_INT, id-1, 0, MPI_COMM_WORLD);
    }
    else{
    }
  }

  if(id != 0){
    MPI_Send(&inputId, 1, MPI_INT, 0, 0,  MPI_COMM_WORLD);//vsetky procesory poslu svoje cislo masterovi
  }
  else{
    int endarr[cpu];
    endarr[0]=inputId;
    for (int i = 1; i < cpu; i++) {
      MPI_Recv(&endarr[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);//master ich obdrzi
    }
    if (!measure) {
      for (int i = 0; i < cpu; i++) {
        cout<<endarr[i]<<endl;//v poradi okamzite vypisuje
      }
    }else{
      endtime = MPI_Wtime();
      std::ofstream out;
      std::string str1 = "measure"+std::to_string(cpu)+".txt";
      out.open(str1, std::ios::app);
      std::string str = std::to_string((size_t)((endtime-starttime)*1000000)) + '\n';
      out << str;
    }
  }
  MPI_Finalize();
}
