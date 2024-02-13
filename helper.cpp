#include<vector>
#include<string>
#include<fstream>
#include<sstream>
#include<cmath>
using namespace std;

string format_date(string date){
    // 2022-11-30
    string new_date="";
    string temp = "";
    //new format = dd/mm/yyyy
    for(int i=date.size()-1 ; i>=0; i--){
        if(date[i] == '-'){
            int num = stoi(temp);
            new_date = new_date + temp + "/";
            temp = "";
        }else{
            temp = date[i] + temp;
        }
    }
    new_date = new_date + temp;
    return new_date;
}

void csv_parser_ADX(string file_name, vector<string> &dates, vector<double> &prices, vector<double> &highs, vector<double> &lows, vector<double> &prev_close){
    ifstream file(file_name);
    if(!file.is_open()){
        cout<<"Error opening the "<<file_name<<" file"<<endl;
        return;
    }
    string line;
    getline(file, line);
    while(getline(file, line)){
        stringstream ss(line);
        string cell;
        getline(ss, cell, ',');

        getline(ss, cell, ','); // take first word (date)
        dates.push_back(format_date(cell));

        getline(ss, cell, ',');
        prices.push_back(stof(cell)); // take second word -- close

        getline(ss, cell, ',');
        highs.push_back(stof(cell)); // take third word -- high
        
        getline(ss, cell, ',');
        lows.push_back(stof(cell)); // take fourth word -- low
        
        getline(ss, cell, ',');
        prev_close.push_back(stof(cell)); // take fifth word -- prev_close
    }
    file.close();
}

void csv_parser(string file_name, vector<string> &dates, vector<double> &prices){
    ifstream file(file_name);
    if(!file.is_open()){
        cout<<"Error opening the "<<file_name<<" file"<<endl;
        return;
    }
    string line;
    getline(file, line);
    while(getline(file, line)){
        stringstream ss(line);
        string cell;
        getline(ss, cell, ',');

        getline(ss, cell, ','); // take first word (date)
        dates.push_back(format_date(cell));

        getline(ss, cell, ',');
        prices.push_back(stof(cell)); // take second word (close price)
    }
    file.close();
}
