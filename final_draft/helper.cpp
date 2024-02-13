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
        prices.push_back(stod(cell)); // take second word -- close

        getline(ss, cell, ',');
        highs.push_back(stod(cell)); // take third word -- high
        
        getline(ss, cell, ',');
        lows.push_back(stod(cell)); // take fourth word -- low
        
        getline(ss, cell, ',');
        prev_close.push_back(stod(cell)); // take fifth word -- prev_close
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
        prices.push_back(stod(cell)); // take second word (close price)
    }
    file.close();
}



void csv_parser_pairs(string file_name1,string file_name2,vector<string> &dates, vector<double> &prices1, vector<double> &prices2, vector<double> &spread){

    ifstream file1(file_name1);
    ifstream file2(file_name2);
    if (!file1.is_open())
    {
        cout << "Error opening the " << file_name1 << " file" << endl;
        return;
    }
    if (!file2.is_open())
    {
        cout << "Error opening the " << file_name2 << " file" << endl;
        return;
    }
    string line1;
    string line2;
    getline(file1, line1);
    getline(file2, line2);
    while (getline(file1, line1) && getline(file2, line2))
    {
        stringstream ss1(line1);
        stringstream ss2(line2);
        string cell1;
        string cell2;
        getline(ss1, cell1, ',');
        getline(ss2, cell2, ',');

        getline(ss1, cell1, ','); // take first word (date)
        getline(ss2, cell2, ','); // take first word (date)
        dates.push_back(format_date(cell1));

        getline(ss1, cell1, ',');
        getline(ss2, cell2, ',');
        prices1.push_back(stod(cell1));
        prices2.push_back(stod(cell2));
        double diff = stod(cell1) - stod(cell2);
        spread.push_back(diff); // take second word (close price)
    }
    file1.close();
    file2.close();
}
void csv_parser_linear(string file_name, vector<string> &dates, vector<double> &target, vector<vector<double>> &prices){
    ifstream file(file_name);
    if (!file.is_open())
    {
        cout << "Error opening the " << file_name << " file" << endl;
        return;
    }
    string line;
    getline(file, line); // skip first line

    while (getline(file, line))
    {
        stringstream ss(line);
        string cell;
        getline(ss, cell, ','); // skip first words

        getline(ss, cell, ','); // take first word (date)
        dates.push_back(format_date(cell));
        vector<double> v;
        // prices[0].push_back(1);
        v.push_back(1);
        for (int i = 1; i < 7; i++)
        {

            getline(ss, cell, ',');
            v.push_back(stod(cell)); // take second word (close price)
        }
        prices.push_back(v);
    }
    for (int i = 1; i < prices.size(); i++)
    {
        prices[i - 1].push_back(prices[i][2]);
        target.push_back(prices[i][1]);
    }
    prices.erase(prices.end() - 1);
    dates.erase(dates.begin());

    file.close();
}