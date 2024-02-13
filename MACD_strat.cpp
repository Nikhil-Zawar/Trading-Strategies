#include"base.h"

vector<double> ewm_calc(vector<double> data, double alpha){
    vector<double> ewms;
    ewms.push_back(data[0]);
    for(int i=1 ; i<data.size() ; i++){
        double val = (alpha*data[i]) + ((1-alpha)*ewms.back());
        ewms.push_back(val);
    }
    return ewms;
}

double strategize_MACD(int x, string start_date, string end_date){
    vector<string> dates;
    vector<double> prices;
    csv_parser("MACD_stock_data.csv", dates, prices);
    dates.erase(dates.begin());
    prices.erase(prices.begin());

    double a1 = 2.0/13;
    vector<double> short_EWM = ewm_calc(prices, a1);
    double a2 = 2.0/27;
    vector<double> long_EWM = ewm_calc(prices, a2);

    vector<double> MACD;
    for(int i=0; i<short_EWM.size();i++){
        double value = short_EWM[i] - long_EWM[i];
        MACD.push_back(value);
    }

    double a3 = 2.0/10;
    vector<double> signals = ewm_calc(MACD, a3);

    ofstream stats_file("order_statistics.csv");
    ofstream cashflow_file("daily_cashflow.csv");
    if(!stats_file.is_open()){cout<<"Order stats not created"<<endl; return 0;}
    if(!cashflow_file.is_open()){cout<<"Cashflow not created"<<endl; return 0;}
    stats_file << "Date,Order_dir,Quantity,Price"<<endl;
    cashflow_file<<"Date,Cashflow"<<endl;

    float balance = 0;
    int position = 0;

    for(int i=0; i<MACD.size(); i++){
        // if(position < x && position > ((-1)*x)){
        if(MACD[i] > signals[i] && position < x){
            //BUY share
            string s = dates[i] + ",BUY,1," + to_string(prices[i]);
            stats_file << s << endl;
            balance = balance - prices[i];
            string c = dates[i] + "," + to_string(balance);
            cashflow_file << c << endl;
            position++;
        }else if (MACD[i] < signals[i] && position > ((-1)*x)){
            //SELL share
            string s = dates[i] + ",SELL,1," + to_string(prices[i]);
            stats_file<< s << endl;
            balance = balance + prices[i];
            string c = dates[i] + "," + to_string(balance);
            cashflow_file << c << endl;
            position--;
        }else{
            string c = dates[i] + "," + to_string(balance);
            cashflow_file << c << endl;    
        }
    }

    ofstream txt_file("final_pnl.txt");
    if(!txt_file.is_open()){cout<<"Text File not created"<<endl; return 0;}
    balance = balance + position * (prices.back());
    if (balance < 0){
        txt_file << "Loss : " << balance << endl;
    }else if (balance > 0){
        txt_file << "Profit: " << balance << endl;
    }else{
        txt_file << "No profit or loss" << endl;
    }
    txt_file.close();
    stats_file.close();
    cashflow_file.close();
    cout<<"Strategy implementation complete"<<endl;
    return balance;
}

double MACD_strategy(string symbol, int x, string from_date, string to_date){
    string comm = "python3 file_generator.py strategy=MACD symbol="+symbol+" n="+to_string(1)+" from_date="+from_date+" to_date="+to_date;
    const char* command = comm.c_str();
    double pnl =0 ;
    int files_generated = system(command);
    if(files_generated == -1){
        cout<<"Failed to generate files using python"<<endl;
    }else{
        cout<<"File generation using python successful"<<endl;
        pnl = strategize_MACD(x, from_date, to_date);
    }
    remove("MACD_stock_data.csv");
    return pnl;
}

// int main(){
//     MACD_strategy("SBIN", 5, "01/01/2023", "01/01/2024");
//     return 0;
// }