#include"base.h"

double strategize_DMA(int n, int x, int p){
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
    for (int i = n - 1; i < dates.size(); i++)
    {

        string currdate = dates[i];
        vector<double> tempData;                 // vector containing prices of last n days
        double avgData = 0;                      // double for calculating average
        int z = 0;
        for (int j = 0; true; j--)              // storing the dates and the prices
        {

            if (z == n)
            {
                break;
            }
            if (dates[i + j] == "")
            {
                continue;
            }
            tempData.push_back(prices[i + j]);
            avgData += tempData[-j];
            z++;
        }
        avgData = avgData / 5;
        double stDev = 0;
        for (int j = 0; j < 5; j++)
        {
            stDev += (tempData[j] - avgData) * (tempData[j] - avgData);
        }
        stDev = stDev / 5;
        stDev = sqrt(stDev);
        double diff = prices[i] - avgData - p * stDev;   

        if (diff >= 0)                                                  // condition for buying
        {
            if (position >= x)                                          // position limit
            {
                file3 << currdate << "," << balance << endl;
                continue;
                ;
            }
            string line = currdate + ",BUY,1," + to_string(prices[i]);
            file2 << line << endl;
            balance -= prices[i];
            file3 << currdate << "," << balance << endl;
            position++;
            continue;
        }

        diff = avgData - prices[i] - p * stDev;
        if (diff >= 0)                                                  // condition for selling
        {
            if (position <= -x)                                         // position limit
            {
                file3 << currdate << "," << balance << endl;
                continue;
            }
            string line = currdate + ",SELL,1," + to_string(prices[i]);
            file2 << line << endl;
            balance += prices[i];
            file3 << currdate << "," << balance << endl;
            position--;
            continue;
        }
        file3 << currdate << "," << balance << endl;                    // if no buying or selling
    }
    balance = balance + position * (prices.back());

    if (balance < 0)
    {
        file4 << "Loss : " << balance << endl;
    }
    else if (balance > 0)
    {
        file4 << "Profit: " << balance << endl;
    }
    else
    {
        file4 << "No profit or loss" << endl;
    }
    file2.close();
    file3.close();
    file4.close();
    return balance;
}

double DMA_strategy(string symbol, int n, int x, double p, string from_date, string to_date){
    string comm = "python3 file_generator.py strategy=DMA symbol="+symbol+" n="+to_string(n)+" from_date="+from_date+" to_date="+to_date;
    const char* command = comm.c_str();
    double pnl =0;
    int files_generated = system(command);
    if(files_generated == -1){
        cout<<"Failed to generate files using python"<<endl;
    }else{
        cout<<"File generation using python successful"<<endl;
        pnl = strategize_DMA(n, x, p);
    }
    remove("DMA_stock_data.csv");
    return pnl;
}

// int main(){
//     DMA_strategy("SBIN",14, 5, 3, "01/01/2023", "01/01/2024");
//     return 0;
// }
