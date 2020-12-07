#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "dht.h"

void commandNode() {
  int dummy;
  int keyval[2];
  int addArgs[2], removeArg;
  int answer[2];
  int key;

  fprintf(stderr, "PUT 9 100\n");
  keyval[0] = 9;
  keyval[1] = 100;
  MPI_Send(keyval, 2, MPI_INT, 0, PUT, MPI_COMM_WORLD);
  MPI_Recv(&dummy, 1, MPI_INT, 0, ACK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  fprintf(stderr, "ADD 3 25\n");
  addArgs[0] = 3;
  addArgs[1] = 25;
  MPI_Send(addArgs, 2, MPI_INT, 0, ADD, MPI_COMM_WORLD);
  MPI_Recv(&dummy, 1, MPI_INT, 0, ACK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  fprintf(stderr, "REMOVE 25\n");
  removeArg = 25;
  MPI_Send(&removeArg, 1, MPI_INT, 0, REMOVE, MPI_COMM_WORLD);
  MPI_Recv(&dummy, 1, MPI_INT, 0, ACK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  fprintf(stderr, "GET 9\n");
  key = 9;
  MPI_Send(&key, 1, MPI_INT, 0, GET, MPI_COMM_WORLD);
  MPI_Recv(answer, 2, MPI_INT, 0, RETVAL, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  printf("val is %d, storage id is %d\n", answer[0], answer[1]);

  fprintf(stderr, "END\n");
  MPI_Send(&dummy, 1, MPI_INT, 0, END, MPI_COMM_WORLD);
  printf("command finalizing\n");
  MPI_Finalize();
  exit(0);
}

