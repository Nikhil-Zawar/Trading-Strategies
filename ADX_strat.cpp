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

void csv_parser(string file_name, vector<string> &dates, vector<float> &prices, vector<float> &highs, vector<float> &lows, vector<float> &prev_close){
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

float calc_TR(float high, float low, float prev_close){
    float a = high - low;
    float b = high - prev_close;
    float c = low - prev_close;
    if(a>b){
        if(a>c){
            return a; //a>b, a>c
        }else{
            return c; //c>a>b
        }
    }else{
        // b>a
        if(b>c){
            return b;
        }else{
            return c;
        }
    }
}

float max_calc(float a, float b){
    if (a>b){return a;}
    else{return 0;}
}

float dx_calc(float di_plus, float di_minus, vector<bool> &can_trade){
    if(di_plus == 0 && di_minus == 0){
        can_trade.push_back(false);
        return 0;
    }
    can_trade.push_back(true);
    float dx = 100*(abs(di_plus - di_minus))/(di_plus + di_minus);
    return dx;
}

void strategize_ADX(int n, int x, float adx_threshold, string start_date, string end_date){
    vector<string> dates;
    vector<float> prices;
    vector<float> highs;
    vector<float> lows;
    vector<float> prev_close;
    csv_parser("ADX_stock_data.csv", dates, prices, highs, lows, prev_close);

    float tr=0;
    float dm_plus = max_calc(highs[1], highs[0]);
    float dm_minus = max_calc(lows[1], lows[0]);

    dates.erase(dates.begin(), dates.begin()+1);
    prices.erase(prices.begin(), prices.begin()+1);
    highs.erase(highs.begin(), highs.begin()+1);
    lows.erase(lows.begin(), lows.begin()+1);
    prev_close.erase(prev_close.begin(), prev_close.begin()+1);
    
    float a1 = 2.0/(n+1);
    vector<float> atr_values;
    float di_plus=0;
    float di_minus=0;
    float dx = 0;
    float adx =0 ;
    float atr =0 ;
    vector<float> adx_values;
    vector<bool> can_trade;

    for(int i=0;i<dates.size();i++){
        tr = calc_TR(highs[i], lows[i], prev_close[i]);
        if(i==0){
            atr = tr;
            di_plus = dm_plus/atr;
            di_minus = dm_minus/atr;
            dx = dx_calc(di_plus, di_minus, can_trade);
            adx = dx;
        }else{
            atr = (a1*tr) + ((1-a1)*atr);
            dm_plus = max_calc(highs[i], highs[i-1]);
            dm_minus = max_calc(lows[i], lows[i-1]);
            di_plus = (a1*(dm_plus/atr)) + ((1-a1)*(di_plus));
            di_minus = (a1*(dm_minus/atr)) + ((1-a1)*(di_minus));
            dx = dx_calc(di_plus, di_minus, can_trade);
            adx = (a1*dx) + ((1-a1)*(adx_values.back()));
        }
        adx_values.push_back(adx);
        // cout<<"The size of adx_values: "<<adx_values.size()<<" and adx: "<<adx <<" and dx: "<<dx<<" and di_plus: "<<di_plus<<" and di_minus: "<<di_minus<<" and can_trade: "<<can_trade[i]<<" and dm_plus: "<<dm_plus<<" and dm_minus: "<<dm_minus<<endl;
        // cout<<" the element diplus: "<<di_plus<<endl;
        // cout<<" the element diminus: "<<di_minus<<endl;
    }
    adx_values.erase(adx_values.begin(), adx_values.begin() + n);
    dates.erase(dates.begin(), dates.begin() + n);
    prices.erase(prices.begin(), prices.begin()+n);
    can_trade.erase(can_trade.begin(), can_trade.begin() +n);

    ofstream stats_file("order_statistics.csv");
    ofstream cashflow_file("daily_cashflow.csv");
    if(!stats_file.is_open()){cout<<"Order stats not created"<<endl; return;}
    if(!cashflow_file.is_open()){cout<<"Cashflow not created"<<endl; return;}
    stats_file << "Date,Order_dir,Quantity,Price"<<endl;
    cashflow_file<<"Date,Cashflow"<<endl;

    float balance = 0;
    int position = 0;

    for(int i=0; i<dates.size(); i++){
        if(adx_values[i] < adx_threshold && position > ((-1)*x) && can_trade[i] == true){
            //SELL
            string s = dates[i] + ",SELL,1," + to_string(prices[i]);
            stats_file<< s << endl;
            balance = balance + prices[i];
            string c = dates[i] + "," + to_string(balance);
            cashflow_file << c << endl;
            position--; 
        }else if(adx_values[i] > adx_threshold && position < x && can_trade[i] == true){
            //BUY
            string s = dates[i] + ",BUY,1," + to_string(prices[i]);
            stats_file << s << endl;
            balance = balance - prices[i];
            string c = dates[i] + "," + to_string(balance);
            cashflow_file << c << endl;
            position++;
        }else{
            //Do nothing
            string c = dates[i] + "," + to_string(balance);
            cashflow_file << c << endl;
        }
    }
    stats_file.close();
    cashflow_file.close();
    cout<<"Strategy implementation complete"<<endl;
}