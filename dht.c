#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "dht.h"
#include "dht-helper.h"
#include <assert.h>


#define REDIST_RM_PUT 10
#define RMBEFORE 11

#define ADD_BEFORE 12
#define ADDED 13
#define ADD_MOVE 14
#define ADD_ACK 15


static int myStorageId, childRank, myRank, numProcesses;
static List list;

// on an END message, the head node is to contact all storage nodes and tell them
void headEnd() {
	int i, dummy;

	// the head node knows there is an END message waiting to be received because
	// we just called MPI_Probe to peek at the message.  Now we just receive it.
	MPI_Recv(&dummy, 1, MPI_INT, numProcesses-1, END, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	// tell all the storage nodes to END
	// the data sent is unimportant here, so just send a dummy value
	for (i = 1; i < numProcesses-1; i++) {
		MPI_Send(&dummy, 1, MPI_INT, i, END, MPI_COMM_WORLD);
	}
	MPI_Finalize();
	exit(0);
}

// on an END message, a storage node just calls MPI_Finalize and exits
void storageEnd() {
	int dummy;  // the data is unimportant for an END

	MPI_Recv(&dummy, 1, MPI_INT, 0, END, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	MPI_Finalize();
	exit(0);
}

void getKeyVal(int source) {
	int *argsAdd;
	int key, value;

	// receive the GET message
	// note that at this point, we've only called MPI_Probe, which only peeks at the message
	// we are receiving the key from whoever sent us the message 
	MPI_Recv(&key, 1, MPI_INT, source, GET, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	if (key <= myStorageId) {  // I own this key/value pair
		// allocate two integers: the first will be the value, the second will be this storage id
		argsAdd = (int *) malloc (2 * sizeof(int));

		// find the associated value (called "value") using whatever data structure you use
		// you must add this code to find it (omitted here)
		Node *node = getNode(&list, key);
		assert(node != NULL);
		value = node->value;

		argsAdd[0] = value;
		argsAdd[1] = myStorageId;
		// send this value around the ring
		MPI_Send(argsAdd, 2, MPI_INT, childRank, RETVAL, MPI_COMM_WORLD);
	}
	else {  // I do NOT own this key/value pair; just forward request to next hop
		MPI_Send(&key, 1, MPI_INT, childRank, GET, MPI_COMM_WORLD);
	}
}
void put(int source) {
	int args[2];
	int k, v;

	MPI_Recv(args, 2, MPI_INT, source, PUT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	k = args[0];
	v = args[1];
	if (k <= myStorageId) {
		putNode(&list, k, v);

		MPI_Send(&k, 1, MPI_INT, childRank, ACK, MPI_COMM_WORLD);
	} else {
		MPI_Send(args, 2, MPI_INT, childRank, PUT, MPI_COMM_WORLD);
	}
}
void add(int source) {
	int args[2];
	int rank, id;
	MPI_Recv(args, 2, MPI_INT, source, ADD, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	rank = args[0];
	id = args[1];
	if (id < myStorageId) {
		int sendArgs[3] = {rank, id, myRank};
		MPI_Send(sendArgs, 3, MPI_INT, childRank, ADD_BEFORE, MPI_COMM_WORLD);
	} else {
		MPI_Send(args, 2, MPI_INT, childRank, ADD, MPI_COMM_WORLD);
	}
}
void addBefore(int source) {
	int args[3];
	int rank, id, afterRank;
	MPI_Recv(args, 3, MPI_INT, source, ADD_BEFORE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	rank = args[0];
	id = args[1];
	afterRank = args[2];
	if (childRank == afterRank) {
		int sendArgs[2] = {id, afterRank};
		childRank = rank;
		MPI_Send(sendArgs, 2, MPI_INT, childRank, ADDED, MPI_COMM_WORLD);
	} else {
		MPI_Send(args, 3, MPI_INT, childRank, ADD_BEFORE, MPI_COMM_WORLD);
	}
}
void added(int source) {
	int args[2];
	//myStorageId, childRank
	MPI_Recv(args, 2, MPI_INT, source, ADDED, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	myStorageId = args[0];
	childRank = args[1];
	int sendArgs[2] = {myRank, myStorageId};
	MPI_Send(sendArgs, 2, MPI_INT, childRank, ADD_ACK, MPI_COMM_WORLD);
}
void addAck(int source) {
	int args[2];
	int rank, id;
	MPI_Recv(args, 2, MPI_INT, source, ADD_ACK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	rank = args[0];
	id = args[1];
	if (list.head != NULL) {
		if (list.head->key <= id) {
			int sendArgs[3] = {rank, list.head->key, list.head->value};
			removeNode(&list, list.head->key);
			MPI_Send(sendArgs, 3, MPI_INT, childRank, ADD_MOVE, MPI_COMM_WORLD);
			return;
		}
	}
	MPI_Send(&rank, 1, MPI_INT, childRank, ACK, MPI_COMM_WORLD);
}
void addMove(int source) {
	int args[3];
	int rank, key, value;
	MPI_Recv(args, 3, MPI_INT, source, ADD_MOVE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	rank = args[0];
	key = args[1];
	value = args[2];
	if (rank == myRank) {
		putNode(&list, key, value);
		int sendArgs[2] = {myRank, myStorageId};
		MPI_Send(sendArgs, 2, MPI_INT, childRank, ADD_ACK, MPI_COMM_WORLD);
	} else {
		MPI_Send(args, 3, MPI_INT, childRank, ADD_MOVE, MPI_COMM_WORLD);
	}
}
void removeF(int source) {
	int id;
	MPI_Recv(&id, 1, MPI_INT, source, REMOVE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	if (myStorageId == id) {
		if (list.head != NULL) {
			int args[4] = {list.head->key, list.head->value, id, myRank};
			removeNode(&list, list.head->key);
			MPI_Send(args, 4, MPI_INT, childRank, REDIST_RM_PUT, MPI_COMM_WORLD);
		} else {
			int args[2] = {myRank, childRank};
			MPI_Send(args, 2, MPI_INT, childRank, RMBEFORE, MPI_COMM_WORLD);
		}
	} else {
		MPI_Send(&id, 1, MPI_INT, childRank, REMOVE, MPI_COMM_WORLD);
	}
}
void redistRmPut(int source) {
	int args[4];
	int key, value, id, rmRank;
	MPI_Recv(args, 4, MPI_INT, source, REDIST_RM_PUT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	key = args[0];
	value = args[1];
	id = args[2];
	rmRank = args[3];
	if (key < myStorageId) {
		putNode(&list, key, value);
		MPI_Send(&id, 1, MPI_INT, childRank, REMOVE, MPI_COMM_WORLD);
	} else {
		MPI_Send(args, 4, MPI_INT, childRank, REDIST_RM_PUT, MPI_COMM_WORLD);
	}
}
void rmBefore(int source) {
	int args[2];
	int rmRank, futureChild;
	MPI_Recv(args, 2, MPI_INT, source, RMBEFORE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	rmRank = args[0];
	futureChild = args[1];
	if (childRank == rmRank) {
		childRank = futureChild;
		MPI_Send(&rmRank, 1, MPI_INT, childRank, ACK, MPI_COMM_WORLD);
	} else {
		MPI_Send(args, 2, MPI_INT, childRank, RMBEFORE, MPI_COMM_WORLD);
	}
}
void forwardRetval(int source) {
	int argsAdd[2];
	//receive message
	MPI_Recv(argsAdd, 2, MPI_INT, source, RETVAL, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	if (myRank == 0) {
		//forward it to command node
		MPI_Send(argsAdd, 2, MPI_INT, numProcesses-1, RETVAL, MPI_COMM_WORLD);
	} else {
		//forward it to next node
		MPI_Send(argsAdd, 2, MPI_INT, childRank, RETVAL, MPI_COMM_WORLD);
	}
}
void forwardAck(int source) {
	int ignore;
	MPI_Recv(&ignore, 1, MPI_INT, source, ACK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	if (myRank == 0) {
		MPI_Send(&ignore, 1, MPI_INT, numProcesses-1, ACK, MPI_COMM_WORLD);
	} else {
		MPI_Send(&ignore, 1, MPI_INT, childRank, ACK, MPI_COMM_WORLD);
	}
}

// handleMessages repeatedly gets messages and performs the appropriate action
void handleMessages() {
	MPI_Status status;
	int count, source, tag;

	while (1) {
		// peek at the message
		MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		// skeleton doesn't use MPI_Get_count, but you will need to
		// this call just gets how many integers are in the message
		MPI_Get_count(&status, MPI_INT, &count);
		// get the source and the tag---which MPI rank sent the message, and
		// what the tag of that message was (the tag is the command)
		source = status.MPI_SOURCE;
		tag = status.MPI_TAG;

		// now take the appropriate action
		// code for END and most of GET is given; others require your code
		switch(tag) {
			case END:
				if (myRank == 0) {
					headEnd();
				}
				else {
					storageEnd();
				}
				break;
			case ADD:
				add(source);
				break;
			case REMOVE:
				removeF(source);
				break;
			case PUT:
				put(source);
				break;
			case GET:
				getKeyVal(source);
				break;
			case ACK:
				forwardAck(source);
				break;
			case RETVAL:
				forwardRetval(source);
				break;
			case REDIST_RM_PUT:
				redistRmPut(source);
				break;
			case RMBEFORE:
				rmBefore(source);
				break;
			case ADD_BEFORE:
				addBefore(source);
				break;
			case ADDED:
				added(source);
				break;
			case ADD_ACK:
				addAck(source);
				break;
			case ADD_MOVE:
				addMove(source);
				break;
				// NOTE: you probably will want to add more cases here, e.g., to handle data redistribution
			default:
				// should never be reached---if it is, something is wrong with your code; just bail out
				printf("ERROR, my id is %d, source is %d, tag is %d, count is %d\n", myRank, source, tag, count);
				exit(1);
		} 
	}
}

int main(int argc, char *argv[]) {
	MPI_Init(&argc, &argv);

	// get my rank and the total number of processes
	// note that myRank is not used in the skeleton, but you will
	// obviously use it in your added code
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);

	// set up the head node and the last storage node
	if (myRank == 0) {
		myStorageId = 0;
		childRank = numProcesses-2;
	}
	else if (myRank == numProcesses-2) {
		myStorageId = MAX;
		childRank = 0;
	}

	// the command node is handled separately
	if (myRank < numProcesses-1) {
		handleMessages();
	}
	else {
		commandNode(); 
	}
	return 0;
}
