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

/*
RSI - Relative strength index
average gain = (sum of Gain() for the last n days) / n
Gain(ith day) = max(Price on ith day - Price on(i-1)th day,  0)
Loss(ith day) = min(Price on ith day - Price on(i-1)th day,  0)

RS = Avg Gain / Avg Loss
RSI = 100 - (100/(1+RS))

Decision - if(RSI < oversold_threshold) ---> then BUY
           if(RSI > overbought_threshold) ---> then SELL

make strategy=RSI symbol=SBIN x=3 n=14 oversold_threshold=30 overbought_threshold=70 start_date="a" end_date="b"
*/

double gain(double p, double q){
    if(p>q){
        return p-q;
    }else{
        return 0;
    }
}

double loss(double p, double q){
    if(p<q){
        return p-q;
    }else{
        return 0;
    }
}

void strategize(int n, int x, string start_date, string end_date, double overbought_threshold, double oversold_threshold){
    vector<string> dates;
    vector<double> prices;
    csv_parser("stock_data.csv", dates, prices);

    //calculate where in the long_date range , does the first element of short_date comes
    double gains_sum =0;
    double avg_gain = 0;
    double loss_sum =0;
    double avg_loss =0;
    double rs = 0;
    double rsi = 0;
    vector<double> rsi_store;
    // rsi.push_back(0);
    // in dates -- 0 to (n-1) index will be data not within our start_date range
    // start_date or the trading day just after start_date will be at index n.
    for(int i=1; i<n; i++){
        gains_sum = gains_sum + gain(prices[i], prices[i-1]);
        loss_sum = loss_sum + loss(prices[i], prices[i-1]);
    }
    for(int i=n;i<dates.size();i++){
        gains_sum = gains_sum + gain(prices[i], prices[i-1]);
        gains_sum = gains_sum - gain(prices[i-n+1], prices[i-n]);
        loss_sum = loss_sum + loss(prices[i], prices[i-1]);
        loss_sum = loss_sum - loss(prices[i-n+1], prices[i-n]);
        avg_gain = gains_sum / n;
        avg_loss = loss_sum / n;
        rs = avg_gain/avg_loss;
        rsi = 100 - (100/(1+rs));
        rsi_store.push_back(rsi);
    }
    cout<<rsi_store.size()<<endl;


    ofstream stats_file("order_statistics.csv");
    ofstream cashflow_file("daily_cashflow.csv");
    if(!stats_file.is_open()){cout<<"Order stats not created"<<endl; return;}
    if(!cashflow_file.is_open()){cout<<"Cashflow not created"<<endl; return;}
    stats_file << "Date,Order_dir,Quantity,Price"<<endl;
    cashflow_file<<"Date,Cashflow"<<endl;

    float balance = 0;
    int position = 0;
    for(int i=n; i<dates.size(); i++){
        if(rsi_store[i-n] > overbought_threshold && position > ((-1)*x)){
            //SELL
            string s = dates[i] + ",SELL,1," + to_string(prices[i]);
            stats_file<< s << endl;
            balance = balance + prices[i];
            string c = dates[i] + "," + to_string(balance);
            cashflow_file << c << endl;
            position--; 
        }else if(rsi_store[i-n] < oversold_threshold && position < x){
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

int main(){
    const char* command = "python3 RSI_strat_files.py symbol=SBIN n=14 from_date=01/01/2023 to_date=01/01/2024";
    int files_generated = system(command);
    //constrant - overbought_threshold >= oversold_threshold
    strategize(14,3,"01/01/2023","01/01/2024", 1000, 600);
    if(files_generated == 0){
        cout<<"Failed to generate files using python"<<endl;
    }else{
        cout<<"Python files generated successfully"<<endl;
    }
    return 0;
}
