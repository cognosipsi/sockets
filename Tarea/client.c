#include <stdio.h> // perror, printf
#include <stdlib.h> // exit, atoi
#include <unistd.h> // read, write, close
#include <arpa/inet.h> // sockaddr_in, AF_INET, SOCK_STREAM,
                       // INADDR_ANY, socket etc...
#include <string.h> // memset

int main(int argc, char const *argv[]) {

  // 0 ./nombre | argv[0]
  // 1 parametro1 (direccion IP) | argv[1]
  // 2 parametro2 (puerto broker) | argv[2]
  // 3 parametro3 (mensaje) | argv[3]

  int serverFd;
  struct sockaddr_in server;
  socklen_t len;
  
  int puerto_broker = 1250;
  char const *server_ip = "127.0.0.1"; 
  char *buffer = "Mensaje predefinido";

  if (argc == 4) {
    server_ip = argv[1];
    puerto_broker = atoi(argv[2]);
    buffer = argv[3];
  }

  serverFd = socket(AF_INET, SOCK_STREAM, 0);
  
  if (serverFd < 0) {
    perror("Cannot create socket");
    exit(1);
  }
  
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr(server_ip);
  server.sin_port = htons(puerto_broker);

  len = sizeof(server);

  // Se utiliza la sentencia connect() para conectar la "cosa" con la máquina
  // Esta recibe 3 parámetros:
  // El primer parámetro es el "socket file descriptor" vecino (la máquina que queremos conectarnos)
  // El segundo (server) es un puntero a un struct "sockaddr_in" que contiene información sobre la direccion ip, puerto, etc..
  // El tercero (len) es el tamaño en bytes de la direccion
  
  if (connect(serverFd, (struct sockaddr *)&server, len) < 0) {
    perror("Cannot connect to server");
    exit(2);
  }

  if (write(serverFd, buffer, strlen(buffer)) < 0) {
    perror("Cannot write");
    exit(3);
  }

  char recv[1024];
  memset(recv, 0, sizeof(recv));

  if (read(serverFd, recv, sizeof(recv)) < 0) {
    perror("Cannot read");
    exit(4);
  }

  printf("Mensaje recibido: \"%s\" por el broker\n", recv);

  close(serverFd);
  return 0;
}
