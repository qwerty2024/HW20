#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "user.h"

using namespace std;


#define MESSAGE_BUFFER 4096 // 4 MByte
#define PORT 7777 // client's port


char buffer[MESSAGE_BUFFER];
char message[MESSAGE_BUFFER];
int socket_descriptor;
struct sockaddr_in serveraddress;


void set_ip_server(int argc, char** argv, bool flag)
{
	if ((argc < 2) || flag)
	{
		string ip_server;
		cout << "Please enter IP-server: ";
		cin >> ip_server;
		serveraddress.sin_addr.s_addr = inet_addr(ip_server.c_str());
	}
	else
	{
		serveraddress.sin_addr.s_addr = inet_addr(argv[1]);
	}
}

void req(string msg)
{
	bzero(message, sizeof(message));

	strcpy(message, msg.c_str());
	//ssize_t bytes = write(socket_descriptor, message, sizeof(message));
	ssize_t bytes = send(socket_descriptor, message, strlen(message), 0);

	bzero(message, sizeof(message));
	//read(socket_descriptor, message, sizeof(message));
	recv(socket_descriptor, message, MESSAGE_BUFFER, 0);
}

bool test_connection()
{
	if(connect(socket_descriptor, (struct sockaddr*) &serveraddress, sizeof(serveraddress)) < 0) 
	{
		cout << "Something went wrong Connection Failed" << endl;
		return false;
	}

	req("@test ");

	if ((strcmp(message, "OK!")) == 0) 
	{
		cout << "Connected!!!" << endl;
	}
	else
	{
		return false;
	}

	return true;
}

void for_send(string &str)
{
	for (int i = 0; i < str.size(); ++i)
	{
		if (str[i] == ' ') str[i] = '#';
	}
}

void print_msg()
{
	cout << "\n===========================================\n";

	int i = 0;
	while(message[i] != '\0')
	{
		if (message[i] != '$')
		{
			while(message[i] != '#')
				cout << message[i++];
			cout << ": ";
			i++;
			while(message[i] != '$' && message[i] != '\0')
			{
				if (message[i] == '#')
				{
					cout << " ";
					i++;
				}
				else
					cout << message[i++];
			}
		}else 
		{
			i++;
			continue;
		}
	cout << endl;
	}


	cout << "===========================================\n" << endl;
}

void action_menu()
{
	while(1)
	{
		cout << "Please choose: Send message to all (1)\n" << 
		        "	       Send message to person (2)\n" <<
			"	       Show messages (3)\n" <<
			"	       Quit (q)?" << endl;
		char choose;
		string tmp_message = "";
		//string name, login, pass;
		cin >> choose;
		if (choose == 'q') 
			break;
		switch(choose)
		{
			case '1':
			{
				string str;
				cout << "Write message for all: " << endl;
				cin.get();
				getline(cin, str);
//cout << str << endl;
				for_send(str);
				str = "@all #" + str;
				req(str);

				// обработать ответ
				if (strcmp(message, "OK!") == 0)
				{
					cout << "Messege delivered!\n" << endl;
				}
				break;
			}

			case '2':
			{
				string str, log;
				cout << "Write login receiver: " << endl;
				cin >> log;
				cout << "Write message for receiver: " << endl;
				cin.get();
				getline(cin, str);
				for_send(str);
				str = "@private " + log + " #" + str;
				req(str);

				if (strcmp(message, "OK!") == 0)
				{
					cout << "Messege delivered!\n" << endl;
				}
				break;
			}

			case '3':
			{
				//cout << "Show" << endl;
				req("@show ");
				// РАСПАРСИТЬ ОТВЕТ!!!
				if (strcmp(message, "@nomsg") == 0)
				{
					cout << "No message!\n" << endl;
				}else
				{
					print_msg();
				}

				break;
			}
		}

	}
}

int main(int argc, char** argv) 
{
	system("clear");

	User my_person;

	serveraddress.sin_port = htons(PORT);
	serveraddress.sin_family = AF_INET;
	socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);

	// set ip_server or read from the args
	set_ip_server(argc, argv, false);
	// try connect with the server
	while(!test_connection())
	{
		// set ip_server
		set_ip_server(argc, argv, true);
	}


	cout << "===========================" << endl;
	cout << "*                         *" << endl;
	cout << "* <<< HELLO! GO CHAT! >>> *" << endl;
	cout << "*                         *" << endl;
	cout << "===========================" << endl;


	// reg or auth?
	while(1)
	{
		cout << "Please choose: Registration (1) or Authentication (2) or Quit (q)?" << endl;
		char choose;
		string tmp_message = "";
		cin >> choose;
		if (choose == 'q') 
		{
			req("@end ");
			
			break;
		}
		if (choose == '1')
		{
			// request reg
			while(1)
			{
				my_person.enter_user();

				tmp_message = "@reg " + my_person.userdata_to_message(); // @reg - key for server
				req(tmp_message);

				if (strcmp(message, "@success ") != 0)
				{
					cout << "\nIncorrect login!\n" << endl;
				}
				else
				{
					cout << "Your reg success! Login: " << my_person.get_login() << " Pass: " << my_person.get_pass() << endl << endl;
					break;
				}
			}
		}
		if (choose == '2')
		{
			string login;
			string pass;

			// request auth
			while(1)
			{
				cout << "Enter your login: ";
				cin >> login;
				cout << "Enter your pass: ";
				cin >> pass;
				//cout << login << " " << pass << endl;
				
				tmp_message = "@auth " + login + " " + pass; // @reg - key for server
				req(tmp_message);

				// обработка ответа
				if (strcmp(message, "@error: Incorrect password") == 0)
				{
					cout << "\nIncorrect password!\n" << endl;
				}
				else if (strcmp(message, "@error ") == 0)
				{
					cout << "\nUnknown user!\n" << endl;
				}else
				{
					cout << "\nYour authentificartion success! Hello, " << message << "!\n" << endl << endl;

					// меню действий в аккаунте
					action_menu();

					break;
				}
			}
		}
	}



	close(socket_descriptor);

	return 0;
}




























