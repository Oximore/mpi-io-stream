/***
 * To test MpiOstream
 ***/

#include <sstream>
#include <cstdlib>
#include <mpi.h>

#include "MpiOstream.hpp"

int main(int argc, char* argv[]) {
  int mpi_rank, mpi_size;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank( MPI_COMM_WORLD, &mpi_rank );
  MPI_Comm_size( MPI_COMM_WORLD, &mpi_size );
  
  std::stringstream coutName;
  coutName << "hello." << mpi_rank << ".out.txt";
  io::setCoutFile( coutName.str() );
 
  std::stringstream cerrName;
  cerrName << "hello." << mpi_rank << ".err.txt";
  io::setCerrFile( cerrName.str() );
 
  io::cout << mpi_rank << " : Hello world :-) !\n";
  io::cerr << mpi_rank << " : Hello world :-( !\n";
  
  MPI_Finalize();
  return EXIT_SUCCESS;
}

