#include "base.h"

double strategize_Advanced_DMA(int n, int x, double p, double c1, double c2, int max_hold_days)
{
    std::cout << "the strategy here" << endl;
    vector<string> dates;
    vector<double> prices;
    csv_parser("Advanced_DMA_stock_data.csv", dates, prices);

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
    vector<pair<int, int>> holdings;

    int day = 0;
    double diff1; // condition for buying
    double diff2; // condition for selling
    bool direct;

    double SF; // smoothening factor
    double AM;
    double ER;
    for (int i = n - 1; i < dates.size(); i++)
    {
        day++;
        string currdate = dates[i];
        if (i == n)
        {
            SF = 0.5;
            AM = (prices[i]);
        }
        else
        {
            vector<double> tempData; // vector containing prices of last n days
            double sumPriceChange = 0;
            double changeInPrice = prices[i] - prices[i - n]; // change in price over last n days

            for (int j = 0; j < n; j++) // storing the dates and the prices
            {

                tempData.push_back(prices[i - j]);
                sumPriceChange += abs(prices[i - j] - prices[i - j - 1]);
            }
            if (sumPriceChange != 0)
            {                                        // if denominator is not 0 then
                ER = changeInPrice / sumPriceChange; // claculated Efficiency ratio
                double temp = (2 * ER) / (1 + c2);
                SF = SF + c1 * ((temp - 1) / (temp + 1) - SF);
                AM = AM + SF * ((prices[i]) - AM);
            }
            else // when deno is 0 then check for max hold days and then
            {
                if (holdings.size() > 0 && holdings[0].first <= day)
                {
                    direct = true;
                    if (holdings[0].second == 1) // sell
                    {
                        diff2 = 10;
                        diff1 = -1;
                    }
                    else // buy
                    {
                        diff1 = 10;
                        diff2 = -1;
                    }
                }
            }
        }

        if (direct != true)
        {
            diff1 = prices[i] - (1 + p / 100) * AM; // for buying
            diff2 = (1 - p / 100) * AM - prices[i]; /// for selling

            if (holdings.size() > 0 && holdings[0].first <= day)
            {
                if (holdings[0].second == 1)
                {
                    if (diff1 < 0)
                    {
                        diff2 = 10;
                    }
                    else if (position >= x)
                    {
                        diff2 = 10;
                        diff1 = -1;
                    }
                    else
                    {
                        diff1 = -1;
                        holdings.erase(holdings.begin());
                        holdings.push_back({day + max_hold_days, 1});
                    }
                }
                else
                {
                    if (diff2 < 0)
                    {
                        diff1 = 10;
                    }
                    else if (position <= -x)
                    {
                        diff1 = 10;
                        diff2 = -1;
                    }
                    else
                    {
                        diff2 = -1;
                        holdings.erase(holdings.begin());
                        holdings.push_back({day + max_hold_days, -1});
                    }
                }
            }
        }
        if (diff1 >= 0) // condition for buying
        {
            direct = false;
            if (position >= x) // position limit
            {
                file3 << currdate << "," << balance << endl;
                continue;
            }
            if (holdings.size() == 0 || holdings[0].second != -1)
            {
                holdings.push_back({day + max_hold_days, 1});
            }
            else
            {
                holdings.erase(holdings.begin());
            }

            file2 << currdate<<",BUY,1,"<<prices[i] << endl;
            balance -= (prices[i]);
            file3 << currdate << "," << balance << endl;
            position++;
            continue;
        }

        if (diff2 >= 0) // condition for selling
        {
            direct = false;
            if (position <= -x) // position limit
            {
                file3 << currdate << "," << balance << endl;
                continue;
            }
            if (holdings.size() == 0 || holdings[0].second != 1)
            {
                holdings.push_back({day + max_hold_days, -1});
            }
            else
            {
                holdings.erase(holdings.begin());
            }

            file2 << currdate<<",SELL,1,"<<prices[i] << endl;
            balance += (prices[i]);
            file3 << currdate << "," << balance << endl;
            position--;
            continue;
        }
        file3 << currdate << "," << balance << endl; // if no buying or selling
    }

    balance = balance + position * (prices.back());

    file4 << balance << endl;

    file2.close();
    file3.close();
    file4.close();
    return balance;
}

double Advanced_DMA_strategy(string symbol, int n, int x, double p, double c1, double c2, int max_hold_days, string from_date, string to_date)
{
    string comm = "python3 file_generator.py strategy=Advanced_DMA symbol=" + symbol + " n=" + to_string(n) + " from_date=" + from_date + " to_date=" + to_date;
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
        pnl = strategize_Advanced_DMA(n, x, p, c1, c2, max_hold_days);
    }
    remove("Advanced_DMA_stock_data.csv");
    return pnl;
}

// int main(){
//     Advanced_DMA_strategy("SBIN",14, 5, 3, 2, 0.5, 7, "01/01/2023", "01/01/2024");
//     return 0;
// }