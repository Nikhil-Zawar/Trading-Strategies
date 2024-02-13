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
void csv_parser_pairs(string file_name1,string file_name2,vector<string> &dates, vector<double> &prices1, vector<double> &prices2, vector<double> &spread);
void csv_parser_linear(string gile_name,vector<string> &dates, vector<double> &target, vector<vector<double>> &prices );