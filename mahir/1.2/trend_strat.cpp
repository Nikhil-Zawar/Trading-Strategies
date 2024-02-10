#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
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
void csv_parser(string file_name, vector<string> &dates, vector<float> &prices){
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
        prices.push_back(stof(cell)); // take second word (close price)
    }
    file.close();
}
void strategize(int n, int x, int p)
{
    std::cout << "the strategy here" << endl;
    vector<string> dates;
    vector<float> prices;
    csv_parser("trend_strat.csv", dates, prices);

    ofstream file3("daily_cashflow.csv");
    ofstream file2("order_statistics.csv");
    ofstream file4("final_pnl.txt");
    if (!file2.is_open())
    {
        std::cerr << "Error opening file" << std::endl;
        return;
    }
    if (!file3.is_open())
    {
        std::cerr << "Error opening file" << std::endl;
        return;
    }
    if (!file4.is_open())
    {
        std::cerr << "Error opening file" << std::endl;
        return;
    }
    file2 << "Date,Order_dir,Quantity,Price" << endl;
    file3 << "Date,Cashflow" << endl;
    float balance = 0;
    int position = 0;
    for (int i = n - 1; i < dates.size(); i++)
    {

        string currdate = dates[i];
        vector<float> tempData;                 // vector containing prices of last n days
        float avgData = 0;                      // double for calculating average
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
        float stDev = 0;
        for (int j = 0; j < 5; j++)
        {
            stDev += (tempData[j] - avgData) * (tempData[j] - avgData);
        }
        stDev = stDev / 5;
        stDev = sqrt(stDev);
        float diff = prices[i] - avgData - p * stDev;   

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

}

int main()
{
    int n = 5;
    int x = 3;
    int p = 1;
    const char *file_command = "python3 trend_strat.py symbol=SBIN n=15 x=8 p=2 from_date=01/01/2024 to_date=06/02/2024";
    int files_generated = system(file_command);
    if (file_command == 0)
    {
        std::cout << "Failed : file generation did not happen from the python file";
    }
    else
    {
        strategize(n, x, p);
    }
    // remove("trend_strat.csv");
    return 0;
}