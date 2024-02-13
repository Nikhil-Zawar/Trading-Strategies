#include"base.h"

double calc_TR(double high, double low, double prev_close){
    double a = high - low;
    double b = high - prev_close;
    double c = low - prev_close;
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

double max_calc(double a, double b){
    if (a>b){return a;}
    else{return 0;}
}

double dx_calc(double di_plus, double di_minus, vector<bool> &can_trade){
    if(di_plus == 0 && di_minus == 0){
        can_trade.push_back(false);
        return 0;
    }
    can_trade.push_back(true);
    double dx = 100*(abs(di_plus - di_minus))/(di_plus + di_minus);
    return dx;
}

double strategize_ADX(int n, int x, double adx_threshold, string start_date, string end_date){
    vector<string> dates;
    vector<double> prices;
    vector<double> highs;
    vector<double> lows;
    vector<double> prev_close;
    csv_parser_ADX("ADX_stock_data.csv", dates, prices, highs, lows, prev_close);

    double tr=0;
    double dm_plus = max_calc(highs[1], highs[0]);
    double dm_minus = max_calc(lows[1], lows[0]);

    dates.erase(dates.begin(), dates.begin()+1);
    prices.erase(prices.begin(), prices.begin()+1);
    highs.erase(highs.begin(), highs.begin()+1);
    lows.erase(lows.begin(), lows.begin()+1);
    prev_close.erase(prev_close.begin(), prev_close.begin()+1);
    
    double a1 = 2.0/(n+1);
    vector<double> atr_values;
    double di_plus=0;
    double di_minus=0;
    double dx = 0;
    double adx =0 ;
    double atr =0 ;
    vector<double> adx_values;
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
    if(!stats_file.is_open()){cout<<"Order stats not created"<<endl; return 0;}
    if(!cashflow_file.is_open()){cout<<"Cashflow not created"<<endl; return 0;}
    stats_file << "Date,Order_dir,Quantity,Price"<<endl;
    cashflow_file<<"Date,Cashflow"<<endl;

    double balance = 0;
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

double ADX_strategy(string symbol, int n, int x, double adx_threshold, string from_date, string to_date){
    string comm = "python3 file_generator.py strategy=ADX symbol="+symbol+" n="+to_string(n)+" from_date="+from_date+" to_date="+to_date;
    const char* command = comm.c_str();
    double pnl=0;
    int files_generated = system(command);
    if(files_generated == -1){
        cout<<"Failed to generate files using python"<<endl;
    }else{
        cout<<"File generation using python successful"<<endl;
        pnl = strategize_ADX(n, x, adx_threshold, from_date, to_date);
    }
    remove("ADX_stock_data.csv");
    return pnl;
}

// int main(){
//     ADX_strategy("SBIN",14, 5, 40, "01/01/2023","01/01/2024");
//     return 0;
// }
