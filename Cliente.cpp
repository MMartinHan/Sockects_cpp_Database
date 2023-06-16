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

    void Enviar(const char* mensaje) {
        send(server, mensaje, strlen(mensaje), 0);
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
        case 1: {
            cin.ignore();
            string nombre, apellido, email;
            cout << "Ingrese el nombre: ";
            getline(cin, nombre);
            cout << "Ingrese el apellido: ";
            getline(cin, apellido);
            cout << "Ingrese el email: ";
            getline(cin, email);

            string mensaje = "insertar " + nombre + " " + apellido + " " + email;
            Cliente->Enviar(mensaje.c_str());
            break;
        }
        case 2:
            Cliente->Enviar("leer");
            Cliente->Recibir();
            break;
        case 3: {
            int id;
            cout << "Ingrese el ID del empleado a actualizar: ";
            cin >> id;
            cin.ignore();
            string nombre, apellido, email;
            cout << "Ingrese el nuevo nombre: ";
            getline(cin, nombre);
            cout << "Ingrese el nuevo apellido: ";
            getline(cin, apellido);
            cout << "Ingrese el nuevo email: ";
            getline(cin, email);

            string mensaje = "actualizar " + to_string(id) + " " + nombre + " " + apellido + " " + email;
            Cliente->Enviar(mensaje.c_str());
            break;
        }
        case 4: {
            int id;
            cout << "Ingrese el ID del empleado a eliminar: ";
            cin >> id;

            string mensaje = "eliminar " + to_string(id);
            Cliente->Enviar(mensaje.c_str());
            break;
        }
        case 5:
            Cliente->CerrarSocket();
            exit(0);
        default:
            cout << "Opción inválida. Intente nuevamente." << endl;
            break;
        }
    }
}
