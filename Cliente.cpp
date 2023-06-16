#include <iostream>
#include <winsock2.h>

using namespace std;

class Client {
public:
    WSADATA WSAData;
    SOCKET server;
    SOCKADDR_IN addr;
    char buffer[1024];

    Client() {
        cout << "Conectando al servidor..." << endl << endl;
        WSAStartup(MAKEWORD(2, 0), &WSAData);
        server = socket(AF_INET, SOCK_STREAM, 0);
        addr.sin_addr.s_addr = inet_addr("192.168.0.8");
        addr.sin_family = AF_INET;
        addr.sin_port = htons(5555);
        connect(server, (SOCKADDR*)&addr, sizeof(addr));
        cout << "Conectado al Servidor!" << endl;
    }

    void Enviar() {
        cout << "Escribe el mensaje a enviar: ";
        cin.ignore(); // Ignorar el salto de línea anterior
        cin.getline(this->buffer, sizeof(buffer));
        send(server, buffer, sizeof(buffer), 0);
        memset(buffer, 0, sizeof(buffer));
        cout << "Mensaje enviado!" << endl;
    }

    void Recibir() {
        recv(server, buffer, sizeof(buffer), 0);
        cout << "El servidor dice: " << buffer << endl;
        memset(buffer, 0, sizeof(buffer));
    }

    void CerrarSocket() {
        closesocket(server);
        WSACleanup();
        cout << "Socket cerrado." << endl << endl;
    }
};

int main() {
    Client* Cliente = new Client();
    while (true) {
        int opcion;
        cout << "Seleccione una opción:" << endl;
        cout << "1. Insertar empleado" << endl;
        cout << "2. Leer todos los empleados" << endl;
        cout << "3. Actualizar empleado" << endl;
        cout << "4. Eliminar empleado" << endl;
        cout << "5. Salir" << endl;
        cout << "Opción: ";
        cin >> opcion;

        switch (opcion) {
        case 1:
            Cliente->Enviar();
            break;
        case 2:
            Cliente->Enviar();
            Cliente->Recibir();
            break;
        case 3:
            Cliente->Enviar();
            break;
        case 4:
            Cliente->Enviar();
            break;
        case 5:
            Cliente->CerrarSocket();
            exit(0);
        default:
            cout << "Opción inválida. Intente nuevamente." << endl;
            break;
        }
    }
}
