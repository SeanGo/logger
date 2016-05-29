#include "utils.h"
#include "types.h"

typedef struct structClient
{
    Block *pBlock;
	int nPort;
	char szIP[32];
    int nfd;
    bool free;
}Client; 

void init_client(Client *pClient);
void init_clients(Client client[], int count);
void close_clients(Client client[], int count);

Client *get_one_free_client(Client clients[], int nCount);
