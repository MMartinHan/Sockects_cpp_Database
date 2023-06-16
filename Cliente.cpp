#include <iostream>
#include <winsock2.h>

using namespace std;

class Client{
public:
    WSADATA WSAData;
    SOCKET server;
    SOCKADDR_IN addr;
    char buffer[1024];
    Client()
    {
        cout<<"Conectando al servidor..."<<endl<<endl;
        WSAStartup(MAKEWORD(2,0), &WSAData);
        server = socket(AF_INET, SOCK_STREAM, 0);
        addr.sin_addr.s_addr = inet_addr("192.168.0.8");
        addr.sin_family = AF_INET;
        addr.sin_port = htons(5555);
        connect(server, (SOCKADDR *)&addr, sizeof(addr));
        cout << "Conectado al Servidor!" << endl;
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

        for (size_t i = 0; i < 5; ++i) {
            buffer += empleado[i];
            if (i < 4) {
                buffer += ",";
            }
        }
        send(server, buffer.c_str(), buffer.length(), 0);
        cout << "Mensaje enviado!" << endl;
    }
    void Recibir()
    {
        recv(server, buffer, sizeof(buffer), 0);
        cout << "El servidor dice: " << buffer << endl;
        memset(buffer, 0, sizeof(buffer));
    }
    void CerrarSocket()
    {
       closesocket(server);
       WSACleanup();
       cout << "Socket cerrado." << endl << endl;
    }
};

int main()
{
    Client *Cliente = new Client();
    while(true)
    {
        int opcion;
        do {
            cout << "MENU:" << endl;
            cout << "1. Ver registros" << endl;
            cout << "2. Insertar registro" << endl;
            cout << "3. Actualizar registro" << endl;
            cout << "4. Eliminar registro" << endl;
            cout << "0. Salir" << endl;
            cout << "Ingrese una opcion: ";
            cin >> opcion;

            switch (opcion) {
                case 1:
                    Cliente->Enviar();
                    Cliente->Recibir();
                    break;
                case 2:
                    //insertarRegistro(conn);
                    break;
                case 3:
                    //actualizarRegistro(conn);
                    break;
                case 4:
                    //eliminarRegistro(conn);
                    break;
                case 0:
                    cout << "Saliendo del programa." << endl;
                    break;
                default:
                    cout << "Opcion no valida." << endl;
            }

            cout << endl;
        } while (opcion != 0);

        return 0;
    }
}
