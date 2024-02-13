#include "base.h"

double strategize_BASIC(int n, int x, string start_date, string end_date)
{
    vector<string> dates;
    vector<double> prices;
    csv_parser("BASIC_stock_data.csv", dates, prices);

    ofstream stats_file("order_statistics.csv");
    ofstream cashflow_file("daily_cashflow.csv");
    ofstream pnl_file("final_pnl.txt");
    if (!stats_file.is_open())
    {
        cout << "Order stats not created" << endl;
        return 0;
    }
    if (!cashflow_file.is_open())
    {
        cout << "Cashflow not created" << endl;
        return 0;
    }
    stats_file << "Date,Order_dir,Quantity,Price" << endl;
    cashflow_file << "Date,Cashflow" << endl;

    double balance = 0;
    int position = 0;
    for (int i = n; i < dates.size(); i++)
    {
        string currDate=dates[i];
        bool check1 = true;
        bool check2 = true;
        for (int j = 0; j < n; j++)
        {
            if(prices[i-j] <= prices[i-j-1]){
                check1 = false;
                break;
            }
        }
        if(check1)               // cond for buy
        {            
            if(position>=x){
                cashflow_file<<currDate<<","<<balance<<endl;
                continue;
            }
            stats_file<< currDate<<",BUY,1,"<<prices[i]<<endl;
            position++;
            balance = balance - prices[i];
            cashflow_file<<currDate<<","<<balance<<endl;
            continue;
        }
        for (int j = 0; j < n; j++)
        {
            if(prices[i-j] >= prices[i-j-1]){
                check2=false;
                break;
            }
        }
        if(check2)              // cond for sell
        {
            if(position<=-x){
                cashflow_file<<currDate<<","<<balance<<endl;
                continue;
            }
            stats_file<< currDate<<",SELL,1,"<<prices[i]<<endl;
            position--;
            balance = balance + prices[i];
            cashflow_file<<currDate<<","<<balance<<endl;
            continue;
        }  
        
    }
    balance = balance + position*(prices.back());
    pnl_file<<balance;
    cashflow_file.close();
    stats_file.close();
    pnl_file.close();
    return balance;
    
}

double BASIC_strategy(string symbol, int n, int x, string from_date, string to_date)
{
    string comm = "python3 file_generator.py strategy=BASIC symbol=" + symbol + " n=" + to_string(n) + " from_date=" + from_date + " to_date=" + to_date;
    const char *command = comm.c_str();
    double pnl = 0;
    int files_generated = system(command);
    if (files_generated == -1)
    {
        cout << "Failed to generate files using python" << endl;
    }
    else
    {
        cout << "File generation using python successful" << endl;
        pnl = strategize_BASIC(n, x, from_date, to_date);
    }
    // remove("BASIC_stock_data.csv");
    return pnl;
}
