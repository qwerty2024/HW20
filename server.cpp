#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <map>

#include "user.h"

using namespace std;

#define MAX_LEN_LOGIN 50
#define MESSAGE_LENGHT 1024
#define PORT 7777

struct Node
{
	Node *next;
	string name_sender;
	string name_recver;
	string message;

	string give_my_message(string login)
	{
		string tmp = "";
		Node *current = this;
		while(current != nullptr)
		{
			if (current->name_recver == login || current->name_recver == "ALL")
				tmp = "$" + current->name_sender + current->message + tmp;;
			current = current->next;
		}
		return tmp;
	}
};

char login[MAX_LEN_LOGIN];
char buffer[MESSAGE_LENGHT]; 
char message[MESSAGE_LENGHT];
int socket_file_descriptor, message_size, connection_status, connection;
socklen_t length;
struct sockaddr_in serveraddress, client;

map<string, User> users_database;
Node *message_database = nullptr;
User current_user;
bool go_exit = false;

enum string_code
{
	test, reg, auth, all, privat, show, endd, null
};

string_code command_code(string const& str)
{
	if (str == "@test") return test;
	if (str == "@reg") return reg;
	if (str == "@auth") return auth;
//	if (str == "@unlogin") return unlogin;
	if (str == "@all") return all;
	if (str == "@private") return privat;
	if (str == "@show") return show;
	if (str == "@end") return endd;

return null;
}

void parse_str(string &str, int n, string &mess) // достаем n-ое слово из строки (разделитель пробел)
{
	int count = 0;
	for (int i = 0; i < mess.size(); ++i)
	{
		if (count == n && mess[i] != ' ')
		{
			str += mess[i];
			continue;
		}

		if (mess[i] == ' ')
			count++;

		if (count > n) break;
		if (mess[i] == '\0') break;
	}
}

void req()
{
	bzero(message, MESSAGE_LENGHT);
	//memset(message, '\0', MESSAGE_LENGHT);
	//read(connection, message, sizeof(message));
	recv(connection, message, MESSAGE_LENGHT, 0);
	if (strcmp(message, "") != 0) cout << "\nData: " << message << endl;

	string recv_message = message;
	string temp = "";
	parse_str(temp, 0, recv_message);

	bzero(message, MESSAGE_LENGHT); // обнуляем сообщение для возврата клиенту
	string tmp_message = "";

	switch(command_code(temp))
	{
	case test:
		{
			tmp_message = "OK!"; // клиенту вернем ОК, так как получили от него сообщение
			break;
		}

	case reg:
		{
			string name; 
			parse_str(name, 1, recv_message);
			string login;
			parse_str(login, 2, recv_message);
			string pass;
			parse_str(pass, 3, recv_message);

			User temp_user(name, login, pass);
			// проверка на наличие такого аккаунта
			try	
			{
				temp_user = users_database.at(login);
				cout << "ERROR. Login is busy!" << endl;
				tmp_message = "@error ";
			}
			catch(...)
			{
				cout << "SAVE. Good login!" << endl;
				tmp_message = "@success ";
				users_database[login] = temp_user;
			}
			break;
		}

	case auth:
		{
			User temp_user;

			string login;
			parse_str(login, 1, recv_message);
			string pass;
			parse_str(pass, 2, recv_message);

			try
			{
				temp_user = users_database.at(login);
				cout << "Login exists!" << endl;
				if (temp_user.get_pass() == pass)
				{
					tmp_message = temp_user.get_name();
					current_user = temp_user; // Сохранить текущего пользователя
				}else
				{
					tmp_message = "@error: Incorrect password";
				}
			}
			catch(...)
			{
				cout << "ERROR. No this user!" << endl;
				tmp_message = "@error ";
				users_database[login] = temp_user;
			}


			break;
		}

	case all:
		{
			string msg;
			parse_str(msg, 1, recv_message);
			Node *new_node = new Node;
			new_node->name_recver = "ALL";
			new_node->name_sender = current_user.get_login();
			new_node->message = msg;
			new_node->next = message_database;
			message_database = new_node;
			tmp_message = "OK!";
			break;
		}

	case privat:
		{
			string login;
			parse_str(login, 1, recv_message);
			string msg;
			parse_str(msg, 2, recv_message);

			Node *new_node = new Node;
			new_node->name_recver = login;
			new_node->name_sender = current_user.get_login();;
			new_node->message = msg;
			new_node->next = message_database;
			message_database = new_node;
			tmp_message = "OK!";
			break;
		}

	case endd:
		{
			tmp_message = "OK!";
			go_exit = true;
			break;
		}

	case show:
		{
			string login = current_user.get_login();
			string msg = message_database->give_my_message(login);

			if (msg == "")
			{
				tmp_message = "@nomsg"; // временно
			}else
			{
				tmp_message = msg;
			}
			break;
		}

	case null:
		{
			break;
		}
	}


	strcpy(message, tmp_message.c_str());
//	ssize_t bytes = write(connection, message, sizeof(message));
	ssize_t bytes = send(connection, message, strlen(message), 0);
}


int main(int argc, char** argv)
{
	system("clear");
	cout << "SERVER IS LISTENING THROUGH THE PORT: " << PORT << endl;

	socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
	serveraddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddress.sin_port = htons(PORT);
	serveraddress.sin_family = AF_INET;
	bind(socket_file_descriptor, (struct sockaddr*)&serveraddress, sizeof(serveraddress));

	connection_status = listen(socket_file_descriptor, 1);
	length = sizeof(client);
	connection = accept(socket_file_descriptor, (struct sockaddr*)&client, &length);


	while(!go_exit)
		req();


	close(socket_file_descriptor);
	return 0;
}