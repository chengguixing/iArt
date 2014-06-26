#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <cassert>
#include "json/json.h"
using namespace std;


int main(void)
{
	/*
	ifstream ifs;
	ifs.open("testjson1.json");
	assert(ifs.io_open());

	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(ifs, root, false))
	{
		return -1;
	}

	string name = root["name"].asString();
	int age = root["age"].asInt();

	cout << name << endl;
	cout << age << endl;

	ifs.close();
	*/
	/*
	ifstream ifs;
	ifs.open("testjson2.json");
	assert(ifs.io_open());

	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(ifs, root, false))
	{
		return -1;
	}

	string name;
	int age;
	int size = root.size();
	for (int i = 0; i < size; ++i)
	{
		name = root[i]["name"].asString();
		age = root[i]["age"].asInt();
		cout << name << endl;
		cout << age << endl;
	}

	ifs.close();
	*/

	Json::Value root;
	Json::FastWriter writer;
	Json::Value person;


	person["name"] = "izhuxin";
	person["age"] = 23;
	root.append(person);

	string json_file = writer.write(root);

	ofstream ofs;
	ofs.open("testjson3.json");
	assert(ofs.is_open());
	ofs << json_file;

	ofs.close();
	system("pause");
	return 0;
}
