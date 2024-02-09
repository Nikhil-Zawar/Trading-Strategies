#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
using namespace std;

void strategize(int n, int x, int p)
{
    std::cout << "the strategy here" << endl;
    ifstream file("trend_strat.csv");
    if (!file.is_open())
    {
        std::cout << "Error opening file" << endl;
        return;
    }
    string line;
    vector<vector<string>> data;
    vector<string> col1;
    vector<string> col2;
    getline(file, line); // ignore first line
    while (getline(file, line))
    {
        stringstream ss(line);
        string cell;
        getline(ss, cell, ','); // ignore first word

        getline(ss, cell, ','); // take first word (date)
        col1.push_back(cell);

        getline(ss, cell, ',');
        col2.push_back(cell); // take second word (close price)
    }
    data.push_back(col1);
    data.push_back(col2);
    file.close();
    ofstream file3("daily_cashflow.csv");
    ofstream file2("order_statistics.csv");
    if (!file2.is_open())
    {
        std::cerr << "Error opening file" << std::endl;
        return;
    }
    file2 << "Date,Order_dir,Quantity,Price" << endl;
    file3 << "Date,Cashflow" << endl;
    float balance = 0;
    int position = 0;
    for (int i = n - 1; i < data[0].size(); i++)
    {

        string currdate = data[0][i];
        vector<float> tempData;                 // vector containing prices of last n days
        float avgData = 0;                      // double for calculating average
        int z = 0;
        for (int j = 0; true; j--)              // storing the dates and the prices
        {

            if (z == n)
            {
                break;
            }
            if (data[0][i + j] == "")
            {
                continue;
            }
            tempData.push_back(std::stof(data[1][i + j]));
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
        float diff = std::stof(data[1][i]) - avgData - p * stDev;   

        if (diff >= 0)                                                  // condition for buying
        {
            if (position >= x)                                          // position limit
            {
                file3 << currdate << "," << balance << endl;
                continue;
                ;
            }
            string line = currdate + ",BUY,1," + data[1][i];
            file2 << line << endl;
            balance -= stof(data[1][i]);
            file3 << currdate << "," << balance << endl;
            position++;
            continue;
        }

        diff = avgData - std::stof(data[1][i]) - p * stDev;
        if (diff >= 0)                                                  // condition for selling
        {
            if (position <= -x)                                         // position limit
            {
                file3 << currdate << "," << balance << endl;
                continue;
            }
            string line = currdate + ",SELL,1," + data[1][i];
            file2 << line << endl;
            balance += stof(data[1][i]);
            file3 << currdate << "," << balance << endl;
            position--;
            continue;
        }
        file3 << currdate << "," << balance << endl;                    // if no buying or selling
    }

    file2.close();
    file3.close();
}

int main()
{
    int n = 5;
    int x = 3;
    int p = 1;
    const char *file_command = "python3 trend_strat.py SBIN 5 8 2 01/01/2023 01/01/2024";
    int files_generated = system(file_command);
    if (file_command == 0)
    {
        std::cout << "Failed : file generation did not happen from the python file";
    }
    else
    {
        strategize(n, x, p);
    }
    remove("trend_strat.csv");
    return 0;
}