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

    // cout<<"entered here"<<endl;
    for(int i=n;i<dates.size();i++){
        // cout<<"entered loop"<<endl;
        // cout<<i-n<<"  and  "<<i-n-1<<endl;
        gains_sum = gains_sum + gain(prices[i], prices[i-1]);
        loss_sum = loss_sum + loss(prices[i], prices[i-1]);
        if(i==n){}
        else{
        gains_sum = gains_sum - gain(prices[i-n], prices[i-n-1]);
        loss_sum = loss_sum - loss(prices[i-n], prices[i-n-1]);
        }
        avg_gain = gains_sum / n;
        avg_loss = abs(loss_sum / n);
        rs = avg_gain/avg_loss;
        rsi = stod("100") -(100/(1+rs));
        rsi_store.push_back(rsi);
        // cout<<"date: "<<dates[i]<<" rsi: "<<rsi<<"rs: "<<rs<<"  avg_gain: "<<avg_gain<<" avg_loss: "<<avg_loss<<endl;
        // rs_store.push_back(rs);
        // avg_gain_store.push_back(avg_gain);
        // avg_loss_store.push_back(avg_loss);
    }


    for(int i =0 ; i<dates.size(); i++){
        if(i<n){

        }else{

        }
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
            stats_file<< dates[i] << ",SELL,1," << prices[i] << endl;
            balance = balance + prices[i];
            position--; 
        }else if(rsi_store[i-n] < oversold_threshold && position < x){
            stats_file << dates[i] << ",BUY,1," << prices[i] << endl;
            balance = balance - prices[i];
            position++;
        }
        cashflow_file << dates[i] << "," << balance<< endl;

    }

    ofstream txt_file("final_pnl.txt");
    if(!txt_file.is_open()){cout<<"Text File not created"<<endl; return 0;}
    balance = balance + position * (prices.back());
    txt_file << balance << endl;
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
