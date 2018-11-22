#ifndef HEADER_FILE
#define HEADER_FILE

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <time.h>
#include <limits>
#include <mysql/mysql.h>
#include <my_global.h>
using namespace std;

void login();
void transcript(string id);
void enroll(string id);
void withdraw(string id);
void personal_details(string id);
void student_menu(string id);
void course_details(string id);
string getQuarter(int month);
vector<string> getFollowingSemesterYear(string semester, int year);
void cureCinState();

#endif