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
void strategize_Advanced_DMA(int n, int x, float p, float c1, float c2, int max_hold_days)
{
    std::cout << "the strategy here" << endl;
    vector<string> dates;
    vector<float> prices;
    csv_parser("Advanced_DMA_stock_data.csv", dates, prices);

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
    vector<pair<int, int>> holdings;

    float diff1;                // condition for buying
    float diff2;                // condition for selling

    float SF;                    // smoothening factor
    float AM;                      
    for (int i = n; i < dates.size(); i++)
    {
        string currdate = dates[i];
        for (int j = 0; j < holdings.size(); j++)
        {
            holdings[j].first += 1;                          // increasing day for each holding
        }
        if (i == n)
        {
            SF = 0.5;
            AM = (prices[i]);
        }
        else
        {
            vector<float> tempData; // vector containing prices of last n days
            float sumPriceChange = 0;
            float changeInPrice = prices[i] - prices[i - n]; // change in price over last n days

            for (int j = 0; j < n; j++) // storing the dates and the prices
            {

                tempData.push_back(prices[i - j]);
                sumPriceChange += abs(prices[i - j] - prices[i - j - 1]);
            }
            if (sumPriceChange != 0)
            {                                                        // if denominator is not 0 then
                float ER = changeInPrice / sumPriceChange;          // claculated Efficiency ratio
                float temp = (2 * ER) / (1 + c2);
                SF = SF + c1 * ((temp - 1) / (temp + 1) - SF);
                AM = AM + SF * ((prices[i]) - AM);
            }
            else                                                    // when deno is 0 then check for max hold days and then 
            {
                if (holdings.size() > 0 && holdings[0].first >= max_hold_days)
                {
                    if (holdings[0].second == 1)
                    {
                        diff2 = 10;
                    }
                    else
                    {
                        diff1 = 10;
                    }
                }
            }
        }

        diff1 = prices[i] - (1 + p / 100) * AM; // for buying
        diff2 = (1 - p / 100) * AM - prices[i]; /// for selling

        if (holdings.size() > 0 && holdings[0].first >= max_hold_days)
        {
            // cout << currdate << " "<< "here" << endl;
            if (holdings[0].second == 1)
            {
                if (diff1 < 0 || position >= x)
                {
                    diff2 = 10;
                }
            }
            else
            {
                if (diff2 < 0 || position <= -x)
                {
                    diff1 = 10;
                }
            }
        }

        if (diff1 >= 0) // condition for buying
        {
            if (position >= x) // position limit
            {
                file3 << currdate << "," << balance << endl;
                continue;
                ;
            }
            if (holdings.size() == 0 || holdings[0].second != -1)
            {
                holdings.push_back({0, 1});
            }
            else
            {
                holdings.erase(holdings.begin());
            }

            string line = currdate + ",BUY,1," + to_string(prices[i]);
            file2 << line << endl;
            balance -= (prices[i]);
            file3 << currdate << "," << balance << endl;
            position++;
            continue;
        }

        if (diff2 >= 0) // condition for selling
        {
            if (position <= -x) // position limit
            {
                file3 << currdate << "," << balance << endl;
                continue;
            }
            if (holdings.size() == 0 || holdings[0].second != 1)
            {
                holdings.push_back({0, -1});
            }
            else
            {
                holdings.erase(holdings.begin());
            }

            string line = currdate + ",SELL,1," + to_string(prices[i]);
            file2 << line << endl;
            balance += (prices[i]);
            file3 << currdate << "," << balance << endl;
            position--;
            continue;
        }
        file3 << currdate << "," << balance << endl; // if no buying or selling
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

// int main(){
//     int n = 15;
//     int x = 3;
//     float p = 1;
//     float c1 = 2;
//     float c2 = 0.2;
//     int max_hold_days = 1;
//     const char *file_command = "python3 improved_DMA.py symbol=SBIN n=15 x=8 p=2 from_date=01/01/2024 to_date=06/02/2024";
//     int files_generated = system(file_command);
//     if (file_command == 0)
//     {
//         std::cout << "Failed : file generation did not happen from the python file";
//     }
//     else
//     {
//         strategize(n, x, p, c1, c2, max_hold_days);
//     }
//     // remove("trend_strat.csv");
//     return 0;
// }