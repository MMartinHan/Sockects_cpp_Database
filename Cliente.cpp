#include <iostream>
#include <winsock2.h>
#include <string>

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
        addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Cambiar la dirección IP por la del servidor
        addr.sin_family = AF_INET;
        addr.sin_port = htons(5555);
        connect(server, (SOCKADDR*)&addr, sizeof(addr));
        cout << "Conectado al Servidor!" << endl;
    }

    void Enviar(const string& mensaje) {
        strcpy_s(buffer, mensaje.c_str());
        send(server, buffer, sizeof(buffer), 0);
        memset(buffer, 0, sizeof(buffer));
        cout << "Mensaje enviado!" << endl;
    }

    void Recibir()
    {
        vector<string> registros;

        while (true)
        {
            memset(buffer, 0, sizeof(buffer));
            int bytesRecibidos = recv(server, buffer, sizeof(buffer) - 1, 0);

            if (bytesRecibidos <= 0)
            {
                break;
            }

            registros.push_back(buffer);
        }

        for (const string& registro : registros)
        {
            cout << "El servidor dice: " << registro << endl;
        }
    }
    void CerrarSocket() {
        closesocket(server);
        WSACleanup();
        cout << "Socket cerrado." << endl << endl;
    }
};

int main() {
    Client* Cliente = new Client();
    string opcion;

    while (true) {
        cout << "Seleccione una opción:" << endl;
        cout << "1. Insertar empleado" << endl;
        cout << "2. Leer empleados" << endl;
        cout << "3. Actualizar empleado" << endl;
        cout << "4. Eliminar empleado" << endl;
        cout << "0. Salir" << endl;
        cout << "Opción: ";
        cin >> opcion;

        if (opcion == "1") {
            int id;
            string nombre, apellido, email;

            cout << "Ingrese el ID: ";
            cin >> id;
            cout << "Ingrese el nombre: ";
            cin >> nombre;
            cout << "Ingrese el apellido: ";
            cin >> apellido;
            cout << "Ingrese el email: ";
            cin >> email;

            string mensaje = "insertar|" + to_string(id) + "|" + nombre + "|" + apellido + "|" + email;
            Cliente->Enviar(mensaje);
        }
        else if (opcion == "2") {
            Cliente->Enviar("leer");
        }
        else if (opcion == "3") {
            int id;
            string nombre, apellido, email;

            cout << "Ingrese el ID: ";
            cin >> id;
            cout << "Ingrese el nuevo nombre: ";
            cin >> nombre;
            cout << "Ingrese el nuevo apellido: ";
            cin >> apellido;
            cout << "Ingrese el nuevo email: ";
            cin >> email;

            string mensaje = "actualizar|" + to_string(id) + "|" + nombre + "|" + apellido + "|" + email;
            Cliente->Enviar(mensaje);
        }
        else if (opcion == "4") {
            int id;

            cout << "Ingrese el ID del empleado a eliminar: ";
            cin >> id;

            string mensaje = "eliminar|" + to_string(id);
            Cliente->Enviar(mensaje);
        }
        else if (opcion == "0") {
            break;
        }
        else {
            cout << "Opción inválida. Intente nuevamente." << endl;
        }

        Cliente->Recibir();
    }

    Cliente->CerrarSocket();
    return 0;
}
