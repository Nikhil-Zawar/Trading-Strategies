#include "base.h"

double strategize_DMA(int n, int x, double p)
{
    std::cout << "the strategy here" << endl;
    vector<string> dates;
    vector<double> prices;
    csv_parser("DMA_stock_data.csv", dates, prices);
    ofstream file3("daily_cashflow.csv");
    ofstream file2("order_statistics.csv");
    ofstream file4("final_pnl.txt");
    if (!file2.is_open())
    {
        std::cerr << "Error opening file" << std::endl;
        return 0;
    }
    if (!file3.is_open())
    {
        std::cerr << "Error opening file" << std::endl;
        return 0;
    }
    if (!file4.is_open())
    {
        std::cerr << "Error opening file" << std::endl;
        return 0;
    }
    file2 << "Date,Order_dir,Quantity,Price" << endl;
    file3 << "Date,Cashflow" << endl;
    double balance = 0;
    int position = 0;
    double avgData = 0;
    double stDev = 0;

    for (int i = n - 1; i < dates.size(); i++)
    {

        string currdate = dates[i];
        if (i == n - 1)
        {
            for (int j = 0; j < n; j++)
            {
                avgData += prices[j];
            }
            avgData = avgData / n;
            for (int j = 0; j < n; j++)
            {
                stDev += (prices[j]-avgData)*(prices[j]-avgData);
            }
            stDev = stDev / n;
            stDev = sqrt(stDev);
        }
        else
        {
            avgData = avgData + (prices[i]-prices[i-n])/n;
            stDev=0;
            for (int j = 0; j < n; j++)
            {
                stDev += (prices[i-j]-avgData)*(prices[i-j]-avgData);
                
            }
            stDev = stDev / n;
            stDev = sqrt(stDev);
        }
        double diff1 = prices[i] - avgData - p * stDev;
        if (diff1 >= 0) // condition for buying
        {
            if (position >= x) // position limit
            {
                file3 << currdate << "," << balance << endl;
                continue;
                
            }
            file2 << currdate<<",BUY,1,"<<prices[i] << endl;
            balance -= prices[i];
            file3 << currdate << "," << balance << endl;
            position++;
            continue;
        }
        double diff2 = avgData - prices[i] - p * stDev;
        if (diff2 >= 0) // condition for selling
        {
            if (position <= -x) // position limit
            {
                file3 << currdate << "," << balance << endl;
                continue;
            }
            file2 << currdate<<",SELL,1,"<<prices[i] << endl;
            balance += prices[i];
            file3 << currdate << "," << balance << endl;
            position--;
            continue;
        }
        file3 << currdate << "," << balance << endl; // if no buying or selling
    }
    balance = balance + position * (prices.back());

    file4 << balance<<" " ;
    file2.close();
    file3.close();
    file4.close();
    return balance;
}

double DMA_strategy(string symbol, int n, int x, double p, string from_date, string to_date)
{
    string comm = "python3 file_generator.py strategy=DMA symbol=" + symbol + " n=" + to_string(n - 1) + " from_date=" + from_date + " to_date=" + to_date;
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
        pnl = strategize_DMA(n, x, p);
    }
    remove("DMA_stock_data.csv");
    return pnl;
}

// int main(){
//     DMA_strategy("SBIN",14, 5, 3, "01/01/2023", "01/01/2024");
//     return 0;
// }