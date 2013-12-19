//flags
#define SF_OK  (1 << 1)
#define SF_ERR (1 << 2)
#define SF_VER (1 << 3)
//errcodes
#define SF_MIS (1 << 1)

struct _server_response
{
        int flags;
        int err;
};

struct _epi_header
{
        char name[30];
        char password[30]; //will be implemented later
        int flags; //will be implemented later
        unsigned long pilen;
        unsigned long elen;
        unsigned long epilen;
};
