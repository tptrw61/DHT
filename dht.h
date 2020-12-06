#define MAX 1000
#define MIN 1

#define PUT 0
#define GET 1
#define ADD 2
#define REMOVE 3
#define END 4

#define RETVAL 5
#define ACK 6

//tells the process added with ADD what its child is
#define HELPADD 7
//propagates forward until at process before instertion point
#define ADDBEFORE 9
//helps redstributes data by moving data to the head node
#define REPUT 8

#define REDIST_ADD 10
#define REDIST_ADD_PUT_ROT 11
#define REDIST_ADD_PUT 12
#define REDIST_ADD_ACK 13

#define REDIST_RM 14
#define REDIST_RM_PUT 15
#define REDIST_RM_ACK 16

void commandNode();

