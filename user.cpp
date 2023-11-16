#include "user.h"

User::User() : _name(""), _login(""), _pass("")
{
}

User::User(string name, string login, string pass) : _name(name), _login(login), _pass(pass)
{
}

User::User(const User &usr)
{
	_name = usr._name;
	_login = usr._login;
	_pass = usr._pass;
}

User User::operator=(const User &usr)
{
	_name = usr._name;
	_login = usr._login;
	_pass = usr._pass;
	return *this;
}

void User::set_name(string name)
{
	_name = name;
}


void User::set_login(string login)
{
	_login = login;
}


void User::set_pass(string pass)
{
	_pass = pass;
}

string User::get_name()
{
	return _name;
}


string User::get_login()
{
	return _login;
}


string User::get_pass()
{
	return _pass;
}

void User::enter_user()
{
	string tmp;
	cout << "Enter your name: ";
	cin >> tmp;
	set_name(tmp);

	cout << "Enter your login: ";
	cin >> tmp;
	set_login(tmp);

	cout << "Enter your pass: ";
	cin >> tmp;
	set_pass(tmp);
}

string User::userdata_to_message()
{
	return (_name + " " + _login + " " + _pass);
}