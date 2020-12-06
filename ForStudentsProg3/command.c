#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "dht.h"

void commandNode() {
  int i, dummy;
  int keyval[2];
  int addArgs[2];
  int answer[2];
  int key;

#if 0
  keyval[0] = 10;
  keyval[1] = 20;
  MPI_Send(keyval, 2, MPI_INT, 0, PUT, MPI_COMM_WORLD);
  MPI_Recv(&dummy, 1, MPI_INT, 0, ACK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  key = 10;
  MPI_Send(&key, 1, MPI_INT, 0, GET, MPI_COMM_WORLD);
  MPI_Recv(answer, 2, MPI_INT, 0, RETVAL, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  printf("val is %d, storage id is %d\n", answer[0], answer[1]);

  keyval[0] = 40;
  keyval[1] = 80;
  MPI_Send(keyval, 2, MPI_INT, 0, PUT, MPI_COMM_WORLD);
  MPI_Recv(&dummy, 1, MPI_INT, 0, ACK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  key = 40;
  MPI_Send(&key, 1, MPI_INT, 0, GET, MPI_COMM_WORLD);
  MPI_Recv(answer, 2, MPI_INT, 0, RETVAL, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  printf("val is %d, storage id is %d\n", answer[0], answer[1]);

  addArgs[0] = 1;
  addArgs[1] = 70;
  MPI_Send(addArgs, 2, MPI_INT, 0, ADD, MPI_COMM_WORLD);
  MPI_Recv(&dummy, 1, MPI_INT, 0, ACK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  
  key = 10;
  MPI_Send(&key, 1, MPI_INT, 0, GET, MPI_COMM_WORLD);
  MPI_Recv(answer, 2, MPI_INT, 0, RETVAL, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  printf("val is %d, storage id is %d\n", answer[0], answer[1]);

  key = 40;
  MPI_Send(&key, 1, MPI_INT, 0, GET, MPI_COMM_WORLD);
  MPI_Recv(answer, 2, MPI_INT, 0, RETVAL, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  printf("val is %d, storage id is %d\n", answer[0], answer[1]);

  addArgs[0] = 70;
  MPI_Send(addArgs, 1, MPI_INT, 0, REMOVE, MPI_COMM_WORLD);
  MPI_Recv(&dummy, 1, MPI_INT, 0, ACK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  key = 10;
  MPI_Send(&key, 1, MPI_INT, 0, GET, MPI_COMM_WORLD);
  MPI_Recv(answer, 2, MPI_INT, 0, RETVAL, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  printf("val is %d, storage id is %d\n", answer[0], answer[1]);

  key = 40;
  MPI_Send(&key, 1, MPI_INT, 0, GET, MPI_COMM_WORLD);
  MPI_Recv(answer, 2, MPI_INT, 0, RETVAL, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  printf("val is %d, storage id is %d\n", answer[0], answer[1]);
#endif

  MPI_Send(&dummy, 1, MPI_INT, 0, END, MPI_COMM_WORLD);
  printf("command finalizing\n");
  MPI_Finalize();
  exit(0);
}

