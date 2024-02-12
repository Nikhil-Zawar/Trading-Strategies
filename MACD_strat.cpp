#include<iostream>
#include<cstdlib>
#include<fstream>
#include<sstream>
#include<vector>
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

vector<double> ewm_calc(vector<double> data, double alpha){
    vector<double> ewms;
    ewms.push_back(data[0]);
    for(int i=1 ; i<data.size() ; i++){
        double val = (alpha*data[i]) + ((1-alpha)*ewms.back());
        ewms.push_back(val);
    }
    return ewms;
}

void strategize_MACD(int x, string start_date, string end_date){
    vector<string> short_dates, long_dates;
    vector<double> short_prices, long_prices;
    csv_parser("MACD_short_stock_data.csv", short_dates, short_prices);
    csv_parser("MACD_long_stock_data.csv", long_dates, long_prices);

//Here i need to know from where does the data start with the start_date
    double a1 = 2.0/13;
    vector<double> short_EWM = ewm_calc(short_prices, a1);
    double a2 = 2.0/27;
    vector<double> long_EWM = ewm_calc(long_prices, a2);

    //calculate where in the long_date range , does the first element of short_date comes
    int short_long =0;
    while(short_dates[0] != long_dates[short_long]){
        short_long++;
    }
    long_EWM.erase(long_EWM.begin(), long_EWM.begin() + short_long);

    vector<double> MACD;
    for(int i=0; i<short_EWM.size();i++){
        double value = short_EWM[i] - long_EWM[i];
        MACD.push_back(value);
    }
    MACD.erase(MACD.begin(), MACD.begin()+3);
    double a3 = 2.0/10;
    vector<double> signals = ewm_calc(MACD, a3);

    MACD.erase(MACD.begin(), MACD.begin()+9);
    signals.erase(signals.begin(), signals.begin()+9);

    ofstream stats_file("order_statistics.csv");
    ofstream cashflow_file("daily_cashflow.csv");
    if(!stats_file.is_open()){cout<<"Order stats not created"<<endl; return;}
    if(!cashflow_file.is_open()){cout<<"Cashflow not created"<<endl; return;}
    stats_file << "Date,Order_dir,Quantity,Price"<<endl;
    cashflow_file<<"Date,Cashflow"<<endl;

    float balance = 0;
    int position = 0;
    short_dates.erase(short_dates.begin(), short_dates.begin()+12);
    short_prices.erase(short_prices.begin(), short_prices.begin()+12);

    for(int i=0; i<MACD.size(); i++){
        // if(position < x && position > ((-1)*x)){
        if(MACD[i] > signals[i] && position < x){
            //BUY share
            string s = short_dates[i] + ",BUY,1," + to_string(short_prices[i]);
            stats_file << s << endl;
            balance = balance - short_prices[i];
            string c = short_dates[i] + "," + to_string(balance);
            cashflow_file << c << endl;
            position++;
        }else if (MACD[i] < signals[i] && position > ((-1)*x)){
            //SELL share
            string s = short_dates[i] + ",SELL,1," + to_string(short_prices[i]);
            stats_file<< s << endl;
            balance = balance + long_prices[i];
            string c = short_dates[i] + "," + to_string(balance);
            cashflow_file << c << endl;
            position--;
        }else{
            string c = short_dates[i] + "," + to_string(balance);
            cashflow_file << c << endl;    
        }
    }
    stats_file.close();
    cashflow_file.close();
    cout<<"Strategy implementation complete"<<endl;
}



