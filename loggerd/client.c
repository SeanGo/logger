#include <string.h>
#include "client.h"

void init_client(Client *pClient)
{
    pClient->pBlock = NULL;
	pClient->nPort = -1;
	memset(pClient->szIP, 0, sizeof(pClient->szIP));
	pClient->nfd = 0;
    pClient->free = true;
}

Client *get_one_free_client(Client clients[], int nCount)
{
    int i=0;
    for (i=0; i<nCount;i++)
    {
        Client *pClient = &clients[i];
        if(pClient->free == true)
        {
            return pClient;
        }
    }
    return NULL;
}


void init_clients(Client client[], int count)
{
    int i=0;
    for(i=0;i<count;i++)
    {
        init_client(&client[i]);
    }
}

void close_clients(Client client[], int count)
{
    int i=0;
    for (i=0;i<count;i++)
    {
        if (client[i].nfd>0 && client[i].free == false)
        {
            close(client[i].nfd);
        }
    }
}
