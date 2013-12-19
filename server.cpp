#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include "epihead.h"

struct _db_entry
{
	char name[30];
	unsigned long epilen;
	int num;
	char confirmed;
};

struct _db_entry_list
{
	struct _db_entry * data;
	unsigned int count;
};

void update(char *, struct _db_entry, struct _db_entry);
struct _db_entry_list *select(char *, int, void *, struct _db_entry_list *);
void db_entry_list_add(struct _db_entry_list *, struct _db_entry);
void update_db_list(struct _db_entry_list * list, int flags, void * data);
void db_entry_list_del(struct _db_entry_list * list, int key);

#define SEL_NAME (1 << 1) /*select by name*/
#define SEL_EPI  (1 << 2) /*select by epilen*/
#define SEL_NUM  (1 << 3) /*select by num*/
#define SEL_CONF (1 << 4) /*select by confirmed*/
#define SEL_ALL  (1 << 5) /*select all*/

void db_entry_list_del(struct _db_entry_list * list, int key)
{
	struct _db_entry_list *list2 = (struct _db_entry_list *)malloc(sizeof(struct _db_entry_list));
	int i, x;
	list2->count = list->count - 1;
	list2->data = (struct _db_entry *)malloc(sizeof(struct _db_entry) * list2->count);
	for (i = 0, x = 0; i < list->count; i++)
	{
		if (i == key)
			continue;
		strcpy(list2->data[x].name, list->data[i].name);
		list2->data[x].epilen = list->data[i].epilen;
		list2->data[x].num = list->data[i].num;
		list2->data[x].confirmed = list->data[i].confirmed;
		x++;
	}
}

void update_db_list(struct _db_entry_list * list, int flags, void * data)
{
	int i;
	char del = 0x0;
	for (i = 0; i < list->count; i++)
	{
		del = 0x0;
		if (flags & SEL_NAME)
		{
			if (strcmp(list->data[i].name, (char *)data) != 0)
				del = 0x1;
		}
		else if (flags & SEL_EPI)
		{
			if (list->data[i].epilen != (unsigned long)data)
				del = 0x1;
		}
		else if (flags & SEL_NUM)
		{
			if (list->data[i].num != *(int *)data)
				del = 0x1;
		}
		else if (flags & SEL_CONF)
		{
			if (list->data[i].confirmed != *(char *)data)
				del = 0x1;
		}
		else {}
		if (del)
			db_entry_list_del(list, i);
	}
}

void db_entry_list_add(struct _db_entry_list *list, struct _db_entry data)
{
	struct _db_entry_list *list2 = (struct _db_entry_list *)malloc(sizeof(struct _db_entry_list));
	int i;
	list2->count = list->count + 1;
	list2->data = (struct _db_entry *)malloc(sizeof(struct _db_entry) * list2->count);
	for (i = 0; i < list->count; i++)
	{
		strcpy(list2->data[i].name, list->data[i].name);
		list2->data[i].epilen = list->data[i].epilen;
		list2->data[i].num = list->data[i].num;
		list2->data[i].confirmed = list->data[i].confirmed;
	}
	list2->data[i].epilen = data.epilen;
	list2->data[i].num = data.num;
	list2->data[i].confirmed = data.confirmed;
	free(list);
	list = list2;
}

struct _db_entry_list * select(char * filename, int flags, void * data, struct _db_entry_list *list)
{
	if (filename == NULL || flags == 0 || data == NULL) return NULL;
	FILE *f = fopen(filename, "r");
	char add = 0x0;
	if (f == NULL) return NULL;
	struct _db_entry_list * ret = (struct _db_entry_list *)malloc(sizeof(struct _db_entry_list));
	struct _db_entry fdata;
	ret->count = 0;
	while (fread(&fdata, sizeof(struct _db_entry), 1, f) != EOF)
	{
		add = 0x0;
		if (flags & SEL_NAME)
		{
			if (strcmp(fdata.name, (char *)data) == 0)
				add = 0x1;
		}
		else if (flags & SEL_EPI)
		{
			if (fdata.epilen == (unsigned long)data)
				add = 0x1;
		}
		else if (flags & SEL_NUM)
		{
			if (fdata.num == *(int *)data)
				add = 0x1;
		}
		else if (flags & SEL_CONF)
		{
			if (fdata.confirmed == *(char *)data)
				add = 0x1;
		}
		else {}
		if (add)
		{
			db_entry_list_add(ret, fdata);
		}
	}
	update_db_list(ret, flags, data);
	return ret;
}

void update(char * filename, struct _db_entry old, struct _db_entry newent)
{
	FILE *f = fopen(filename, "r");
	FILE *g;
	char tmpfile[50];
	struct _db_entry tmp;
	char new_wrote = 0x0;
	char wrote_something = 0x0;

	sprintf(tmpfile, "/tmp/%s", filename);
	g = fopen(tmpfile, "w");
	fseek(f, 0L, SEEK_SET);
	if (old.name[0] == 0)
	{
		fwrite(&newent, sizeof(struct _db_entry), 1, g);
		printf("-->Created new node\n");
	}
	else while (fread(&tmp, sizeof(struct _db_entry), 1, f) != EOF)
	{
		if (strcmp(tmp.name, old.name) == 0)
		{
			if (tmp.epilen == old.epilen)
			{
				if (tmp.num == old.num)
				{
					if (tmp.confirmed == old.confirmed)
					{
						fwrite(&newent, sizeof(struct _db_entry), 1, g);
						new_wrote = 0x1;
						wrote_something = 0x1;
						printf("-->Updated node\n");
					}
				}
			}
		}
		if (!new_wrote)
		{
			fwrite(&tmp, sizeof(struct _db_entry), 1, g);
		}
	}
	fclose(f);
	fclose(g);
	rename(tmpfile, filename);
}

int main()
{
	int listenfd, connfd;
	struct sockaddr_in serv_addr;
	char sendBuff[1025];
	char *recvBuff;
	time_t ticks;
	int i;
	struct _epi_header connHeader;
	struct _server_response servResponse;
	int epiread;
	FILE *dbf;// = fopen("db", "a+");
	FILE *outf;

	printf("_epi_header size: %lu\n", sizeof(_epi_header));

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, '0', sizeof(serv_addr));
	memset(sendBuff, '0', sizeof(sendBuff));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(5000);
	bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	listen(listenfd, 10);

	while (1)
	{
		connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
		ticks = time(NULL);
		read(connfd, &connHeader, sizeof(_epi_header));
		printf( "Header recieved:\n"
			"Name: %-30s Pass: %-30s\n"
			"Flags: %d\n"
			"Pi size: %lu\n"
			"E size: %lu\n"
			"e^Pi size: %lu\n",
			connHeader.name, connHeader.password,
			connHeader.flags,
			connHeader.pilen,
			connHeader.elen,
			connHeader.epilen
		     );
		if (connHeader.pilen == connHeader.epilen)
			servResponse.flags |= SF_OK;
		else
		{
			servResponse.flags |= SF_ERR;
			servResponse.err |= SF_MIS;
		}
		recvBuff = (char *)malloc(connHeader.epilen + 1024); //add 1,024 bytes of padding, just in case
								     //some sneaky bastard tries an overflow
		write(connfd, &servResponse, sizeof(_server_response));
		epiread = 0;
		while (epiread < connHeader.epilen)
			epiread += read(connfd, recvBuff + epiread, 1);
		strcat(recvBuff, "\0");
		printf("%d bytes recieved\n", (int)strlen(recvBuff));
		struct _db_entry_list *newentlist = select((char *)"db", SEL_EPI, &connHeader.epilen, NULL);
		struct _db_entry newent;
		struct _db_entry zeroent = {0};
		if (newentlist->count > 0)
		{
			newent = newentlist->data[0];
			zeroent = newent;
			zeroent.confirmed = 0x1;
		}
		else
		{
			strcpy(newent.name, connHeader.name);
			newent.epilen = connHeader.epilen;
			newent.num = 0;
			newent.confirmed = 0;
		}
		update((char *)"db", zeroent, newent);
		close(connfd);
	}
}
