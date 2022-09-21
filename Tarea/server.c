#include <stdio.h> // perror, printf
#include <stdlib.h> // exit, atoi
#include <unistd.h> // read, write, close
#include <arpa/inet.h> // sockaddr_in, AF_INET, SOCK_STREAM,
                       // INADDR_ANY, socket etc...
#include <string.h> // memset

/*
  El struct sockaddr_in está compuesto de lo siguiente 

  struct sockaddr_in {
    short int           sin_family; // direccion de la familia
    unsigned short int  sin_port;   // numero del puerto
    struct in_addr      sin_addr;   // direccion de internet
    unsigned char       sin_zero[8];// mismo tamaño como el struct sockaddr

  };

*/
int main(int argc, char const *argv[]) {

  // server (Socket) File Descriptor 
  int serverFd, clientFd;

  struct sockaddr_in server, client;
  socklen_t len;
  int port = 1234;
  char buffer[1024];
  if (argc == 2) {
    port = atoi(argv[1]);
  }

  // Se crea el socket con la sentencia socket()
  // AF_INET para IP's de la versión IPv4
  // AF_INET6 para IP's de la versión IPv6
  // SOCK_STREAM es para sockets que utilizan TCP
  // SOCK_DGRAM es para sockets que utilizan UDP
  serverFd = socket(AF_INET, SOCK_STREAM, 0);

  if (serverFd < 0) {
    perror("Cannot create socket");
    exit(1);
  }

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(port);
  len = sizeof(server);

  // Se utiliza la sentencia bind() para indicar en el puerto que se encuentra el servidor
  // Este recibe 3 parámetros, estos son:
  // El primer parametro (serverFd) es el "Socket file descriptor" retornado por socket()
  // El segundo (server) es un puntero a una estructura "sockaddr_in" que contiene información sobre la direccion ip, puerto, etc..
  // El tercero (len) es el tamaño en bytes de la direccion

  if (bind(serverFd, (struct sockaddr *)&server, len) < 0) {
    perror("Cannot bind sokcet");
    exit(2);
  }

  // Se utiliza la sentencia listen() para que el socket se mantenga "atento"
  // Este recibe 2 parametros, estos son:
  // El primer parámetro (serverFd) es el "Socket file descriptor"  
  // El segundo (10) es el numero de conecciones entrantes permitidas en la cola
  if (listen(serverFd, 10) < 0) {
    perror("Listen error");
    exit(3);
  }

  while (1) {

    len = sizeof(client);
    printf("waiting for clients\n");

    // En el caso de que una "cosa" quiera conectarse ( conect() ) con mi máquina,
    // esta máquina se encontrará escuchando ( listen() ) y la solicitud o el mensaje entrante se alojará en la cola de espera esperando a ser aceptado ( accept() )
    // Una vez sea aceptada la conexión, la máquina le dice al cliente que tenga una conexión pendiente
    // Esta operación retornará un nuevo "socket file descriptor" para ser utilizado para esta simple conexión
    // Este "socket file descriptor" se usará para enviar ( send() ) y recibir( recv() )

    // Accept() recibe 3 parametros, estos son:
    // El primer parámetro (serverFd) es el "Socket file descriptor" del servidor
    // El segundo parámetro (client) es usualmente un puntero a una struct local llamado "sockaddr_storage"
    // El tercer parámetro (len) es una variable local de tipo entero que debería ser asignada al tamaño de "sockaddr_storage" ( sizeof(struct sockaddr_storage)) antes de que su dirección sea aceptada ( accept() )

    if ((clientFd = accept(serverFd, (struct sockaddr *)&client, &len)) < 0) {
      perror("accept error");
      exit(4);
    }

    // inet_ntoa(IP) transforma los datos al formato de IPv4. 
    char *client_ip = inet_ntoa(client.sin_addr);
    printf("Accepted new connection from a client %s:%d\n", client_ip, ntohs(client.sin_port));

    // memset() asigna valores '0' a buffer es decir, si buffer es un arreglo de longitud igual a 5 quedaría como: [0,0,0,0,0]
    memset(buffer, 0, sizeof(buffer));

    int size = read(clientFd, buffer, sizeof(buffer));

    if ( size < 0 ) {
      perror("read error");
      exit(5);
    }

    printf("El mensaje recibido es: %s\n", buffer);

    if (write(clientFd, buffer, size) < 0) {
      perror("write error");
      exit(6);
    }

    close(clientFd);

  }
  close(serverFd);
  return 0;
}