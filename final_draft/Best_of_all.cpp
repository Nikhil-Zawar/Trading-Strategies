#include <iostream>
#include <vector>
#include <pthread.h>
#include <string>
#include <unordered_map>

using namespace std;


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
    if (a>b){return a-b;}
    else{return 0;}
}

double dx_calc(double di_plus, double di_minus, vector<bool> &can_trade){
    if(di_plus == 0 && di_minus == 0){
        can_trade.push_back(false);
        return 0;
    }
    can_trade.push_back(true);
    double dx = 100*(di_plus - di_minus)/(di_plus + di_minus);
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
    // double dm_plus = max_calc(highs[1], highs[0]);
    // double dm_minus = max_calc(lows[1], lows[0]);
    double dm_plus = 0;
    double dm_minus = 0;
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

    dates.erase(dates.begin(), dates.begin()+n-2);
    prices.erase(prices.begin(), prices.begin()+n-2);
    highs.erase(highs.begin(), highs.begin()+n-2);
    lows.erase(lows.begin(), lows.begin()+n-2);
    prev_close.erase(prev_close.begin(), prev_close.begin()+n-2);

    for(int i=1;i<dates.size();i++){
        tr = calc_TR(highs[i], lows[i], prev_close[i]);
        if(i==1){
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
        // cout<<"date: "<<dates[i]<<" tr : "<<tr<<" atr: "<<atr<<endl;
        // cout<<"date: "<<dates[i]<<" dm_plus : "<<dm_plus<<" dm_minus: "<<dm_minus<<endl;
        // cout<<"date: "<<dates[i]<<" di_plus : "<<di_plus<<" di_minus: "<<di_minus<<endl;
        // cout<<"date: "<<dates[i]<<" dx : "<<dx<<" adx: "<<adx<<endl;
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
            stats_file<< dates[i] << ",SELL,1," << prices[i] << endl;
            balance = balance + prices[i];
            cashflow_file << dates[i] << "," << balance << endl;
            position--; 
        }else if(adx_values[i] > adx_threshold && position < x && can_trade[i] == true){
            //BUY
            stats_file<< dates[i] << ",SELL,1," << prices[i] << endl;
            balance = balance - prices[i];
            cashflow_file << dates[i] << "," << balance << endl;
            position++;
        }else{
            //Do nothing
            cashflow_file << dates[i] << "," << balance << endl;
        }
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
    // remove("ADX_stock_data.csv");
    return pnl;
}



struct StrategyInfo {
    std::string symbol;
    int n;
    int x;
    double p;
    double c1;
    double c2;
    int max_hold_days;
    double adx_threshold;
    double oversold_threshold;
    double overbought_threshold;
    std::string from_date;
    std::string to_date;
    double result;
};

void* executeBASICStrategy(void* arg) {
    StrategyInfo* info = static_cast<StrategyInfo*>(arg);
    info->result = BASIC_strategy(info->symbol, info->n, info->x, info->from_date, info->to_date);
    return nullptr;
}

void* executeDMAStrategy(void* arg) {
    StrategyInfo* info = static_cast<StrategyInfo*>(arg);
    info->result = DMA_strategy(info->symbol, info->n, info->x, info->p, info->from_date, info->to_date);
    return nullptr;
}

void* executeAdvancedDMAStrategy(void* arg) {
    StrategyInfo* info = static_cast<StrategyInfo*>(arg);
    info->result = Advanced_DMA_strategy(info->symbol, info->n, info->x, info->p, info->c1, info->c2, info->max_hold_days, info->from_date, info->to_date);
    return nullptr;
}

void* executeMACDStrategy(void* arg) {
    StrategyInfo* info = static_cast<StrategyInfo*>(arg);
    info->result = MACD_strategy(info->symbol, info->x, info->from_date, info->to_date);
    return nullptr;
}

void* executeRSIStrategy(void* arg) {
    StrategyInfo* info = static_cast<StrategyInfo*>(arg);
    info->result = RSI_strategy(info->symbol, info->n, info->x, info->overbought_threshold, info->oversold_threshold, info->from_date, info->to_date);
    return nullptr;
}

void* executeADXStrategy(void* arg) {
    StrategyInfo* info = static_cast<StrategyInfo*>(arg);
    info->result = ADX_strategy(info->symbol, info->n, info->x, info->adx_threshold, info->from_date, info->to_date);
    return nullptr;
}

int Best_of_all(string from_date, string to_date) {
    int num_of_strategies = 6;
    cout<<"Strategy started"<<endl;
    StrategyInfo BASIC_object = {"SBIN", 7, 5, 0, 0, 0, 0, 0, 0, 0, from_date, to_date};
    StrategyInfo DMA_object = {"SBIN", 50, 5, 2, 0, 0, 0, 0, 0, 0, from_date, to_date};
    StrategyInfo Advanced_DMA_object = {"SBIN", 14, 5, 5, 2, 0.2, 28, 0, 0, 0, from_date, to_date};
    StrategyInfo MACD_object = {"SBIN", 14, 5, 0, 0, 0, 0, 0, 0, 0, from_date, to_date};
    StrategyInfo RSI_object = {"SBIN", 14, 5, 0, 0, 0, 0, 0, 30, 70, from_date, to_date};
    StrategyInfo ADX_object = {"SBIN", 14, 5, 0, 0, 0, 0, 25, 0, 0, from_date, to_date};
    cout<<"objects made"<<endl;
    pthread_t threads[num_of_strategies];

    for (size_t i = 0; i < num_of_strategies; ++i) {
        switch (i) {
            case 0:
                pthread_create(&threads[i], nullptr, executeBASICStrategy, &BASIC_object);
                break;
            case 1:
                pthread_create(&threads[i], nullptr, executeDMAStrategy, &DMA_object);
                break;
            case 2:
                pthread_create(&threads[i], nullptr, executeAdvancedDMAStrategy, &Advanced_DMA_object);
                break;
            case 3:
                pthread_create(&threads[i], nullptr, executeMACDStrategy, &MACD_object);
                break;
            case 4:
                pthread_create(&threads[i], nullptr, executeRSIStrategy, &RSI_object);
                break;
            case 5:
                pthread_create(&threads[i], nullptr, executeADXStrategy, &ADX_object);
                break;
            default:
                break;
        }
    }

    for (size_t i = 0; i < num_of_strategies; ++i) {
        pthread_join(threads[i], nullptr);
    }

    // Store the results in a vector
    std::vector<double> results;
    //ORder of results - BASIC, DMA, Advanced DMA, MACD, RSI, ADX
    results.push_back(BASIC_object.result);
    results.push_back(DMA_object.result);
    results.push_back(Advanced_DMA_object.result);
    results.push_back(MACD_object.result);
    results.push_back(RSI_object.result);
    results.push_back(ADX_object.result);

    // Do something with the results
    for (double result : results) {
        std::cout << "Result: " << result << std::endl;
    }
    return 0;
}


int main(){
    string from_date = "01/01/2023";
    string to_date = "01/01/2024";
    int a = Best_of_all(from_date, to_date);
    return 1;
}
