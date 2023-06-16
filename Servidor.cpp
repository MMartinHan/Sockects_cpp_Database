#include <iostream>
#include <winsock2.h>

using namespace std;

class Server{
public:
    WSADATA WSAData;
    SOCKET server, client;
    SOCKADDR_IN serverAddr, clientAddr;
    char buffer[1024];
    Server()
    {
        WSAStartup(MAKEWORD(2,0), &WSAData);
        server = socket(AF_INET, SOCK_STREAM, 0);

        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(5555);

        bind(server, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
        listen(server, 0);

        cout << "Escuchando para conexiones entrantes." << endl;
        int clientAddrSize = sizeof(clientAddr);
        if((client = accept(server, (SOCKADDR *)&clientAddr, &clientAddrSize)) != INVALID_SOCKET)
        {
            cout << "Cliente conectado!" << endl;
        }
    }

    string Recibir()
    {
      recv(client, buffer, sizeof(buffer), 0);
      cout << "El cliente dice: " << buffer << endl;
      memset(buffer, 0, sizeof(buffer));
    }
    void Enviar()
    {
        string empleado[5];
        string buffer;
        empleado[0] = "insertar";
        cout << "Escribe el ID: ";
        cin >> empleado[1];
        cout << "Escribe el Nombre: ";
        cin >> empleado[2];
        cout << "Escribe el Apellido: ";
        cin >> empleado[3];
        cout << "Escribe el Email: ";
        cin >> empleado[4];

        for (const string& dato : empleado) {
            buffer += dato + " ";
        }

        send(client, buffer.c_str(), buffer.length(), 0);
        cout << "Mensaje enviado!" << endl;
    }
    void CerrarSocket()
    {
        closesocket(client);
        cout << "Socket cerrado, cliente desconectado." << endl;
    }
};


int main()
{
  Server *Servidor = new Server();
  while(true)
  {
     Servidor->Recibir();
     Servidor->Enviar();
  }
}
