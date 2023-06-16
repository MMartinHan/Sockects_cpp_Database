#include <iostream>
#include <winsock2.h>
#include <mysql.h>
#include <string>

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
            cout << "Error al conectar con la base de datos." << endl;
        }
    }

    void Enviar(const string& mensaje) {
        strcpy(buffer, mensaje.c_str());
        send(client, buffer, sizeof(buffer), 0);
        memset(buffer, 0, sizeof(buffer));
        cout << "Mensaje enviado!" << endl;
    }

    void Recibir() {
        recv(client, buffer, sizeof(buffer), 0);
        string mensaje = buffer;
        cout << "El cliente dice: " << mensaje << endl;
        ProcesarMensaje(mensaje);
        memset(buffer, 0, sizeof(buffer));
    }

    void ProcesarMensaje(const string& mensaje) {
        size_t pos = mensaje.find("|");
        if (pos == string::npos) {
            Enviar("Formato de mensaje incorrecto.");
            return;
        }

        string comando = mensaje.substr(0, pos);
        string parametros = mensaje.substr(pos + 1);

        if (comando == "insertar") {
            size_t pos1 = parametros.find("|");
            size_t pos2 = parametros.find("|", pos1 + 1);
            size_t pos3 = parametros.find("|", pos2 + 1);

            if (pos1 == string::npos || pos2 == string::npos || pos3 == string::npos) {
                Enviar("Formato de mensaje incorrecto. Uso correcto: insertar|<id>|<nombre>|<apellido>|<email>");
                return;
            }

            int id = stoi(parametros.substr(0, pos1));
            string nombre = parametros.substr(pos1 + 1, pos2 - pos1 - 1);
            string apellido = parametros.substr(pos2 + 1, pos3 - pos2 - 1);
            string email = parametros.substr(pos3 + 1);

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
                if (res) {
                    string empleados;  // Variable para acumular los registros

                    MYSQL_ROW row;
                    while ((row = mysql_fetch_row(res))) {
                        string id = row[0];
                        string nombre = row[1];
                        string apellido = row[2];
                        string email = row[3];

                        string empleado = "ID: " + id + ", Nombre: " + nombre + ", Apellido: " + apellido + ", Email: " + email;
                        empleados += empleado + "\n";  // Agregar el registro a la variable acumulada
                    }

                    if (!empleados.empty()) {
                        Enviar(empleados);  // Enviar todos los registros acumulados
                    } else {
                        Enviar("No se encontraron empleados.");
                    }

                    mysql_free_result(res);
                } else {
                    Enviar("No se encontraron empleados.");
                }
            } else {
                Enviar("Error al leer empleados.");
            }
        }
        else if (comando == "actualizar") {
            size_t pos1 = parametros.find("|");
            size_t pos2 = parametros.find("|", pos1 + 1);
            size_t pos3 = parametros.find("|", pos2 + 1);

            if (pos1 == string::npos || pos2 == string::npos || pos3 == string::npos) {
                Enviar("Formato de mensaje incorrecto. Uso correcto: actualizar|<id>|<nombre>|<apellido>|<email>");
                return;
            }

            int id = stoi(parametros.substr(0, pos1));
            string nombre = parametros.substr(pos1 + 1, pos2 - pos1 - 1);
            string apellido = parametros.substr(pos2 + 1, pos3 - pos2 - 1);
            string email = parametros.substr(pos3 + 1);

            string query = "UPDATE empleado SET nombre = '" + nombre + "', apellido = '" + apellido + "', email = '" + email + "' WHERE id = " + to_string(id);
            if (mysql_query(conn, query.c_str()) == 0) {
                Enviar("Empleado actualizado correctamente.");
            }
            else {
                Enviar("Error al actualizar empleado.");
            }
        }
        else if (comando == "eliminar") {
            int id = stoi(parametros);

            string query = "DELETE FROM empleado WHERE id = " + to_string(id);
            if (mysql_query(conn, query.c_str()) == 0) {
                Enviar("Empleado eliminado correctamente.");
            }
            else {
                Enviar("Error al eliminar empleado.");
            }
        }
        else {
            Enviar("Comando desconocido.");
        }
    }

    void CerrarSocket() {
        closesocket(client);
        cout << "Socket cerrado, cliente desconectado." << endl;
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
