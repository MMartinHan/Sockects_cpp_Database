#include <iostream>
#include <winsock2.h>
#include <mysql.h>
#include <vector>

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
        string mensaje;
        char c;
        while (true) {
            recv(client, &c, sizeof(c), 0);
            if (c == '\0') {
                break;
            }
            mensaje += c;
        }
        cout << "El cliente dice: " << mensaje << endl;
        ProcesarMensaje(mensaje);
    }

    void Enviar(const string& mensaje) {
        for (char c : mensaje) {
            send(client, &c, sizeof(c), 0);
        }
        char terminador = '\0';
        send(client, &terminador, sizeof(terminador), 0);
    }

    void ProcesarMensaje(const string& mensaje) {
        vector<string> palabras;
        string palabra;
        for (char c : mensaje) {
            if (c == '|') {
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
            if (palabras.size() != 5) {
                Enviar("Formato de mensaje incorrecto. Uso correcto: insertar|<id>|<nombre>|<apellido>|<email>");
                return;
            }

            int id = stoi(palabras[1]);
            string nombre = palabras[2];
            string apellido = palabras[3];
            string email = palabras[4];

            string query = "INSERT INTO empleado (id, nombre, apellido, email) VALUES (" + to_string(id) + ", '" + nombre + "', '" + apellido + "', '" + email + "')";
            if (mysql_query(conn, query.c_str()) == 0) {
                Enviar("Empleado insertado correctamente.");
            }
            else {
                Enviar("Error al insertar empleado.");
            }
        }
        else if (comando == "leer") {
            string query = "SELECT * FROM empleado";
            if (mysql_query(conn, query.c_str()) == 0) {
                MYSQL_RES* res = mysql_store_result(conn);

                string respuesta = "Datos de todos los empleados:\n";
                MYSQL_ROW row;
                while ((row = mysql_fetch_row(res))) {
                    respuesta += "ID: " + string(row[0]) + ", Nombre: " + string(row[1]) + ", Apellido: " + string(row[2]) + ", Email: " + string(row[3]) + "\n";
                }

                mysql_free_result(res);
                Enviar(respuesta);
            }
            else {
                Enviar("Error al leer los datos de los empleados.");
            }
        }
        else if (comando == "actualizar") {
            if (palabras.size() != 5) {
                Enviar("Formato de mensaje incorrecto. Uso correcto: actualizar|<id>|<nombre>|<apellido>|<email>");
                return;
            }

            int id = stoi(palabras[1]);
            string nombre = palabras[2];
            string apellido = palabras[3];
            string email = palabras[4];

            string query = "UPDATE empleado SET nombre = '" + nombre + "', apellido = '" + apellido + "', email = '" + email + "' WHERE id = " + to_string(id);
            if (mysql_query(conn, query.c_str()) == 0) {
                Enviar("Empleado actualizado correctamente.");
            }
            else {
                Enviar("Error al actualizar empleado.");
            }
        }
        else if (comando == "eliminar") {
            if (palabras.size() != 2) {
                Enviar("Formato de mensaje incorrecto. Uso correcto: eliminar|<id>");
                return;
            }

            int id = stoi(palabras[1]);

            string query = "DELETE FROM empleado WHERE id = " + to_string(id);
            if (mysql_query(conn, query.c_str()) == 0) {
                Enviar("Empleado eliminado correctamente.");
            }
            else {
                Enviar("Error al eliminar empleado.");
            }
        }
        else {
            Enviar("Comando inválido.");
        }
    }

    void CerrarSocket() {
        closesocket(client);
        cout << "Socket cerrado, cliente desconectado." << endl;
        mysql_close(conn);
    }
};

int main() {
    Server* Servidor = new Server();
    while (true) {
        Servidor->Recibir();
    }
    Servidor->CerrarSocket();
    return 0;
}
