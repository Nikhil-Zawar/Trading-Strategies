#include<iostream>
#include<cstdlib>
#include<fstream>
#include<sstream>
#include<vector>
#include<cmath>
using namespace std;


#include<vector>
#include<string>
#include<fstream>
#include<sstream>
#include<cmath>
using namespace std;

string format_date(string date);

void csv_parser_ADX(string file_name, vector<string> &dates, vector<double> &prices, vector<double> &highs, vector<double> &lows, vector<double> &prev_close);

void csv_parser(string file_name, vector<string> &dates, vector<double> &prices);
