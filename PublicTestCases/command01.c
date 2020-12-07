#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "dht.h"

void commandNode() {
  int dummy;

  fprintf(stderr, "END\n");
  MPI_Send(&dummy, 1, MPI_INT, 0, END, MPI_COMM_WORLD);
  printf("command finalizing\n");
  MPI_Finalize();
  exit(0);
}

