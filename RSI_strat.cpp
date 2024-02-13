#include"base.h"

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

double strategize_RSI(int n, int x, double overbought_threshold, double oversold_threshold, string start_date, string end_date){
    vector<string> dates;
    vector<double> prices;
    csv_parser("RSI_stock_data.csv", dates, prices);

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


    ofstream stats_file("order_statistics.csv");
    ofstream cashflow_file("daily_cashflow.csv");
    if(!stats_file.is_open()){cout<<"Order stats not created"<<endl; return 0;}
    if(!cashflow_file.is_open()){cout<<"Cashflow not created"<<endl; return 0;}
    stats_file << "Date,Order_dir,Quantity,Price"<<endl;
    cashflow_file<<"Date,Cashflow"<<endl;

    double balance = 0;
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

double RSI_strategy(string symbol, int n, int x, double overbought_threshold, double oversold_threshold, string from_date, string to_date){
    string comm = "python3 file_generator.py strategy=RSI symbol="+symbol+" n="+to_string(n)+" from_date="+from_date+" to_date="+to_date;
    const char* command = comm.c_str();
    double pnl =0;
    int files_generated = system(command);
    if(files_generated == -1){
        cout<<"Failed to generate files using python"<<endl;
    }else{
        cout<<"File generation using python successful"<<endl;
        pnl = strategize_RSI(n, x, overbought_threshold, oversold_threshold, from_date, to_date);
    }
    remove("RSI_stock_data.csv");
    return pnl;
}

// int main(){
//     RSI_strategy("SBIN", 10, 5, 25, 10, "01/01/2023", "01/01/2024");
//     return 0;
// }
