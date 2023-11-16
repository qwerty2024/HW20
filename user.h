#pragma once

#include <string>
#include <iostream>

using std::string;
using std::cout;
using std::endl;
using std::cin;

class User
{
	string _name;
	string _login;
	string _pass;



public:
	User();
	User(string name, string login, string pass);
	User(const User &usr);
	User operator=(const User &usr);

	void set_name(string name);
	void set_login(string login);
	void set_pass(string pass);

	string get_name();
	string get_login();
	string get_pass();


	void enter_user();
	string userdata_to_message();
};