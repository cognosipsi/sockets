#include <stdio.h> // perror, printf
#include <stdlib.h> // exit, atoi
#include <unistd.h> // read, write, close
#include <arpa/inet.h> // sockaddr_in, AF_INET, SOCK_STREAM,
                       // INADDR_ANY, socket etc...
#include <string.h> // memset

/* Broker es una "mezcla" entre el servidor y el cliente. El método "mensajeCliente" representa 
 la parte del cliente que se encargará de comunicarse con "server"
 y la parte del main corresponde al comportamiento del servidor que se comunicará con "client"
*/

void mensajeCliente(char* mensaje, int puerto_server, char const* server_ip){
  int serverFd;
  struct sockaddr_in server;
  socklen_t len;

  serverFd = socket(AF_INET, SOCK_STREAM, 0);
  
  if (serverFd < 0) {
    perror("Cannot create socket");
    exit(1);
  }
  
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr(server_ip);
  server.sin_port = htons(puerto_server);

  len = sizeof(server);
  
  if (connect(serverFd, (struct sockaddr *)&server, len) < 0) {
    perror("Cannot connect to server");
    exit(2);
  }

  if (write(serverFd, mensaje, strlen(mensaje)) < 0) {
    perror("Cannot write");
    exit(3);
  }

  char recv[1024];
  memset(recv, 0, sizeof(recv));

  if (read(serverFd, recv, sizeof(recv)) < 0) {
    perror("Cannot read");
    exit(4);
  }

  close(serverFd);

}

int main(int argc, char const *argv[]) {

  // 0 ./nombre | argv[0]
  // 1 parametro1 (puerto broker) | argv[1]
  // 2 parametro2 (direccion servidor[IP]) | argv[2]
  // 3 parametro3 (puerto servidor) | argv[3]

  int serverFd, clientFd;
  struct sockaddr_in server, client;
  socklen_t len;

  int puerto_broker = 1250;
  int puerto_server = 1234;
  const char* ip_servidor = "127.0.0.1";

  char mensaje [1024];
  char buffer[1024];

  if (argc == 4) {
    
    puerto_broker = atoi(argv[1]);
    ip_servidor = argv[2];
    puerto_server = atoi(argv[3]);

  }

  // Se crea el socket con la sentencia socket()
  serverFd = socket(AF_INET, SOCK_STREAM, 0);

  if (serverFd < 0) {
    perror("Cannot create socket");
    exit(1);
  }

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(puerto_broker);

  len = sizeof(server);

  // Se utiliza la sentencia bind() para 
  if (bind(serverFd, (struct sockaddr *)&server, len) < 0) {
    perror("Cannot bind sokcet");
    exit(2);
  }

  // Se utiliza la sentencia listen() para 
  if (listen(serverFd, 10) < 0) {
    perror("Listen error");
    exit(3);
  }

  while (1) {

    len = sizeof(client);
    printf("waiting for clients\n");
    
    if ((clientFd = accept(serverFd, (struct sockaddr *)&client, &len)) < 0) {
      perror("accept error");
      exit(4);
    }

    char *client_ip = inet_ntoa(client.sin_addr);
    printf("Accepted new connection from a client %s:%d\n", client_ip, ntohs(client.sin_port));
    memset(buffer, 0, sizeof(buffer));
    memset(mensaje, 0, sizeof(mensaje));
    int size = read(clientFd, buffer, sizeof(buffer));

    if ( size < 0 ) {
      perror("read error");
      exit(5);
    }

    //printf("El mensaje recibido es: %s del cliente, su ip es: %s y su puerto es: %d \n", buffer,client_ip, ntohs(client.sin_port) );


    if (write(clientFd, buffer, size) < 0) {
      perror("write error");
      exit(6);
    }

    close(clientFd);
    char a[10];
    
    unsigned int uwu = ntohs(client.sin_port);
    sprintf(a, "%d", uwu);
    strcat(mensaje,"\""); 
    strcat(mensaje, buffer ); 
    strcat(mensaje,"\" enviado por el cliente, su IP es: "); 
    strcat(mensaje, client_ip); 
    strcat(mensaje," y su puerto: ");
    strcat(mensaje, a );

    printf("Mensaje Recibido de Cliente\n");
    
    mensajeCliente(mensaje, puerto_server, ip_servidor);
    
  }
  close(serverFd);
  return 0;
}

