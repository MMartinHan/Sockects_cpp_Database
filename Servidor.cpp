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
    }

    string Recibir() {
        recv(client, buffer, sizeof(buffer), 0);
        cout << "El cliente dice: " << buffer << endl;
        string mensaje(buffer);
        memset(buffer, 0, sizeof(buffer));
        return mensaje;
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
    }
};

// Funciones del CRUD
void InsertarEmpleado(MYSQL* conn, const string& nombre, const string& apellido, const string& email) {
    string query = "INSERT INTO empleado (nombre, apellido, email) VALUES ('" + nombre + "', '" + apellido + "', '" + email + "')";
    mysql_query(conn, query.c_str());
    cout << "Empleado insertado correctamente." << endl;
}

void LeerEmpleados(MYSQL* conn) {
    string query = "SELECT * FROM empleado";
    mysql_query(conn, query.c_str());
    MYSQL_RES* res = mysql_store_result(conn);

    cout << "Datos de todos los empleados:" << endl;
    while (MYSQL_ROW row = mysql_fetch_row(res)) {
        cout << "ID: " << row[0] << ", Nombre: " << row[1] << ", Apellido: " << row[2] << ", Email: " << row[3] << endl;
    }

    mysql_free_result(res);
}

void ActualizarEmpleado(MYSQL* conn, int id, const string& nombre, const string& apellido, const string& email) {
    string query = "UPDATE empleado SET nombre = '" + nombre + "', apellido = '" + apellido + "', email = '" + email + "' WHERE id = " + to_string(id);
    mysql_query(conn, query.c_str());
    cout << "Empleado actualizado correctamente." << endl;
}

void EliminarEmpleado(MYSQL* conn, int id) {
    string query = "DELETE FROM empleado WHERE id = " + to_string(id);
    mysql_query(conn, query.c_str());
    cout << "Empleado eliminado correctamente." << endl;
}

int main() {
    MYSQL* conn;
    MYSQL_ROW row;
    MYSQL_RES* res;

    conn = mysql_init(0);
    conn = mysql_real_connect(conn, "localhost", "root", "", "prueba", 0, NULL, 0);
    if (conn) {
        system("color a");
        cout << "Database connected with success." << endl;
    }
    else {
        system("color c");
        cout << "Failed to connect database." << endl;
        return 0;
    }

    Server* Servidor = new Server();
    while (true) {
        string mensaje = Servidor->Recibir();
        string respuesta;

        // Parsear mensaje y realizar operaciones CRUD según corresponda
        if (mensaje == "insertar") {
            string nombre, apellido, email;
            cout << "Ingrese el nombre: ";
            getline(cin, nombre);
            cout << "Ingrese el apellido: ";
            getline(cin, apellido);
            cout << "Ingrese el email: ";
            getline(cin, email);

            InsertarEmpleado(conn, nombre, apellido, email);
            respuesta = "Empleado insertado correctamente.";
        }
        else if (mensaje == "leer") {
            LeerEmpleados(conn);
            respuesta = "Datos de todos los empleados mostrados en el servidor.";
        }
        else if (mensaje == "actualizar") {
            int id;
            string nombre, apellido, email;
            cout << "Ingrese el ID del empleado a actualizar: ";
            cin >> id;
            cin.ignore(); // Ignorar el salto de línea anterior
            cout << "Ingrese el nuevo nombre: ";
            getline(cin, nombre);
            cout << "Ingrese el nuevo apellido: ";
            getline(cin, apellido);
            cout << "Ingrese el nuevo email: ";
            getline(cin, email);

            ActualizarEmpleado(conn, id, nombre, apellido, email);
            respuesta = "Empleado actualizado correctamente.";
        }
        else if (mensaje == "eliminar") {
            int id;
            cout << "Ingrese el ID del empleado a eliminar: ";
            cin >> id;

            EliminarEmpleado(conn, id);
            respuesta = "Empleado eliminado correctamente.";
        }
        else {
            respuesta = "Comando inválido.";
        }

        Servidor->Enviar(respuesta.c_str());
    }

    mysql_close(conn);
    return 0;
}
