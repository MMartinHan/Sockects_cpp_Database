#include <iostream>
#include <winsock2.h>
#include <mysql.h>
#include <windows.h>

using namespace std;

class Server {
public:
    WSADATA WSAData;
    SOCKET server, client;
    SOCKADDR_IN serverAddr, clientAddr;
    char buffer[1024];
    MYSQL* conn;

    Server() {
        WSAStartup(MAKEWORD(2, 0), &WSAData);
        server = socket(AF_INET, SOCK_STREAM, 0);

        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(5555);

        bind(server, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
        listen(server, 0);

        cout << "Escuchando para conexiones entrantes." << endl;
        int clientAddrSize = sizeof(clientAddr);
        if ((client = accept(server, (SOCKADDR*)&clientAddr, &clientAddrSize)) != INVALID_SOCKET) {
            cout << "Cliente conectado!" << endl;
        }

        conn = mysql_init(0);
        conn = mysql_real_connect(conn, "localhost", "root", "", "prueba", 0, NULL, 0);
        if (conn) {
            system("color a");
            cout << "Base de datos conectada con éxito." << endl;
        }
        else {
            system("color c");
            cout << "Error al conectar a la base de datos." << endl;
            exit(0);
        }
    }

    void Recibir() {
        recv(client, buffer, sizeof(buffer), 0);
        cout << "El cliente dice: " << buffer << endl;
        string mensaje(buffer);
        ProcesarMensaje(mensaje);
        memset(buffer, 0, sizeof(buffer));
    }

    void Enviar(const char* mensaje) {
        send(client, mensaje, strlen(mensaje), 0);
        memset(buffer, 0, sizeof(buffer));
        cout << "Mensaje enviado!" << endl;
    }

    void CerrarSocket() {
        closesocket(client);
        closesocket(server);
        cout << "Socket cerrado, cliente desconectado." << endl;
        mysql_close(conn);
    }

    void ProcesarMensaje(const string& mensaje) {
        // Dividir el mensaje en palabras separadas
        vector<string> palabras;
        string palabra;
        for (char c : mensaje) {
            if (c == ' ') {
                palabras.push_back(palabra);
                palabra.clear();
            }
            else {
                palabra += c;
            }
        }
        palabras.push_back(palabra);

        if (palabras.empty()) {
            Enviar("Mensaje inválido.");
            return;
        }

        string comando = palabras[0];
        if (comando == "insertar") {
            if (palabras.size() != 4) {
                Enviar("Formato de mensaje incorrecto. Uso correcto: insertar <nombre> <apellido> <email>");
                return;
            }

            string nombre = palabras[1];
            string apellido = palabras[2];
            string email = palabras[3];

            string query = "INSERT INTO empleado (nombre, apellido, email) VALUES ('" + nombre + "', '" + apellido + "', '" + email + "')";
            mysql_query(conn, query.c_str());

            Enviar("Empleado insertado correctamente.");
        }
        else if (comando == "leer") {
            string query = "SELECT * FROM empleado";
            mysql_query(conn, query.c_str());
            MYSQL_RES* res = mysql_store_result(conn);

            string respuesta = "Datos de todos los empleados:\n";
            while (MYSQL_ROW row = mysql_fetch_row(res)) {
                respuesta += "ID: " + string(row[0]) + ", Nombre: " + string(row[1]) + ", Apellido: " + string(row[2]) + ", Email: " + string(row[3]) + "\n";
            }

            mysql_free_result(res);

            Enviar(respuesta.c_str());
        }
        else if (comando == "actualizar") {
            if (palabras.size() != 5) {
                Enviar("Formato de mensaje incorrecto. Uso correcto: actualizar <id> <nombre> <apellido> <email>");
                return;
            }

            int id = stoi(palabras[1]);
            string nombre = palabras[2];
            string apellido = palabras[3];
            string email = palabras[4];

            string query = "UPDATE empleado SET nombre = '" + nombre + "', apellido = '" + apellido + "', email = '" + email + "' WHERE id = " + to_string(id);
            mysql_query(conn, query.c_str());

            Enviar("Empleado actualizado correctamente.");
        }
        else if (comando == "eliminar") {
            if (palabras.size() != 2) {
                Enviar("Formato de mensaje incorrecto. Uso correcto: eliminar <id>");
                return;
            }

            int id = stoi(palabras[1]);

            string query = "DELETE FROM empleado WHERE id = " + to_string(id);
            mysql_query(conn, query.c_str());

            Enviar("Empleado eliminado correctamente.");
        }
        else {
            Enviar("Comando inválido.");
        }
    }
};

int main() {
    Server* Servidor = new Server();
    while (true) {
        Servidor->Recibir();
    }
}
