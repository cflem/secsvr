#include "init.h"

int main (int argc, char** argv) {
  if (argc < 2) {
    printf("Usage: %s <port> <certfile.pem> <keyfile.pem>\n", argv[0]);
  }

  SSL_CTX* ctx = init_ctx();
  load_certs(ctx, argv[2], argv[3])
  int servfd = opensvr(atoi(argv[1]));
  printf("*** SERVER STARTED ***\n");

  while (1) {
    struct sockaddr_in addr;
    int len = sizeof(addr);

    int clifd = accept(servfd, &addr, &len);
    printf("[INFO] Connection from %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    if (fork() == 0) continue; // TODO: choose either to fork or multithread
    SSL* ssl = SSL_new(ctx);
    SSL_set_fd(ssl, clifd);
    cloop(ssl);
  }
  close(servfd);
  SSL_CTX_free(ctx);
  printf("*** SERVER EXITING NORMALLY ***\n");
  exit(0);
}

void die() {
  fprintf(stdout, "*** SERVER QUITTING ***\n");
  exit(1);
}

SSL_CTX* init_ctx () {
  SSL_METHOD* method;
  SSL_CTX* ctx;

  OpenSSL_add_all_algorithms();
  SSL_load_error_strings();
  method = TLSv1_server_method();
  ctx = SSL_CTX_new(method);
  if (ctx == 0) {
    ERR_print_errors_fp(stderr);
    die();
  }
  return ctx;
}

void load_certs (SSL_CTX* ctx, char* cert, char* key) {
  if (!SSL_CTX_use_certificate_file(ctx, cert, SSL_FILETYPE_PEM)) {
    ERR_print_errors_fp(stderr);
    die();
  }
  if (!SSL_CTX_use_PrivateKey_file(ctx, key, SSL_FILETYPE_PEM)) {
    ERR_print_errors_fp(stderr);
    die();
  }
  if (!SSL_CTX_check_private_key(ctx)) {
    fprintf(stderr, "Private key doesn't match certificate.\n");
    die();
  }
}

int opensvr (int port) {
  int sockfd;
  struct sockaddr_in addr;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  memset(&addr, 0, sizeof(addr))
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;
  if (bind(sockfd, &addr, sizeof(addr)) != 0) {
    fprintf(stderr, "Failed to bind to port %d.\n", port);
    die();
  }

  // daemonize
  daemon(1, 0);
  dup2(open("error.log", O_WRONLY|O_CREAT|O_APPEND, 0644), fileno(stderr));
  dup2(open("server.log", O_WRONLY|O_CREAT|O_APPEND, 0644), fileno(stdout));

  if (listen(sockfd, 5) != 0) {
    fprintf(stderr, "Can't listen on port %d.\n", port);
    die();
  }

  return sockfd;
}

void cloop (SSL* ssl) {
  // TODO: your play
}
