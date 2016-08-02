#include "common.h"

int main (int, char**);
void die ();
SSL_CTX* init_ctx ();
void load_certs (SSL_CTX*, char*, char*);
int opensvr (int);
void cloop (SSL*);
