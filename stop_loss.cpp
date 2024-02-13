#include"base.h"
using namespace std;

double  strategize_stop_loss(int x, int n, double threshold, double stop_loss_threshold)
{
    std::cout << "the strategy here" << endl;
    vector<string> dates;
    vector<double> spread;
    vector<double> prices1;
    vector<double> prices2;

    csv_parser_pairs("pairs-1.csv", "pairs-2.csv", dates, prices1, prices2, spread);

    ofstream file3("daily_cashflow.csv");
    ofstream file4("final_pnl.txt");
    ofstream file5("order_statistics_1.csv");
    ofstream file6("order_statistics_2.csv");
    if (!file5.is_open())
    {
        std::cerr << "Error opening file" << std::endl;
        return 0;
    }
    if (!file6.is_open())
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
    file5 << "Date,Order_dir,Quantity,Price" << endl;
    file6 << "Date,Order_dir,Quantity,Price" << endl;
    file3 << "Date,Cashflow" << endl;
    double balance = 0;
    int position1 = 0;
    vector<pair<double, double>> holdings; // to store mean,std
    int trade;

    double std = 0;
    double avg = 0;

    for (int i = n - 1; i < spread.size(); i++)
    {
        string currDate = dates[i];
        int lastPos = position1;
        string line1;
        string line2;
        if (i == n - 1)
        {
            for (int j = 0; j < n; j++)
            {
                avg += spread[j];
            }
            avg = avg / n;
            for (int j = 0; j < n; j++)
            {
                std += (spread[j] - avg) * (spread[j] - avg);
            }
            std = std / n;
            std = sqrt(std);
        }
        else
        {
            avg = avg + (spread[i] - spread[i - n]) / n;
            std = 0;
            for (int j = 0; j < n; j++)
            {
                std += (spread[i - j] - avg) * (spread[i - j] - avg);
            }
            std = std / n;
            std = sqrt(std);
        }
        double z_score = (spread[i] - avg) / std;
        if (z_score > threshold)                // sell 
        {
            if (position1 <= -x)
            {
                file3 << currDate << "," << balance << endl;
            }
            else
            {
                line1 = currDate +  ",SELL,1," + to_string(prices1[i]);
                line2 = currDate  + ",BUY,1," + to_string(prices2[i]);
                position1--;
                if (position1 <= 0)
                {
                    holdings.push_back({avg, std}); // if pos<0 then only push else pop
                }
                else
                {
                    holdings.pop_back();
                }
            }
        }
        else if (z_score < -threshold)
        {
            if (position1 >= x)
            {
                file3 << currDate << "," << balance << endl;
                cout<<currDate<<endl;
            }
            else
            {
                line1 = currDate + ",BUY,1," + to_string(prices1[i]);
                line2 = currDate + ",SELL,1," + to_string(prices2[i]);
                position1++;
                if (position1 >= 0)
                {
                    holdings.push_back({avg, std}); // if pos<0 then only push else pop
                }
                else
                {
                    holdings.pop_back();
                }
            }
        }
        if (position1 == 0)
        {

            if (lastPos < 0)
            {
                file5 << line1 << "," << z_score << endl;
                file6 << line2 << "," << z_score << endl;
                balance = balance - prices1[i] + prices2[i];
            }
            else if (lastPos > 0)
            {
                file5 << line1 << "," << z_score << endl;
                file6 << line2 << "," << z_score << endl;
                balance = balance + prices1[i] - prices2[i];
            }

            file3 << currDate << "," << balance << endl;

            continue;
        }
        trade = holdings.size();
        for (int j = holdings.size() - 1; j >= 0; j--)
        {
            double score = (spread[i] - holdings[j].first) / holdings[j].second;
            if (abs(score) > stop_loss_threshold)
            {
                holdings.erase(holdings.begin() + j);
            }
        }
        trade = trade - holdings.size();
        if(position1>0){trade = -trade;}
        trade = trade + position1-lastPos;
        if (trade > 0)
        {
            position1 = lastPos + trade;
            line1 = currDate + "," + "BUY," + to_string(trade) + "," + to_string(prices1[i]);
            line2 = currDate + "," + "SELL," + to_string(trade) + "," + to_string(prices2[i]);
            file5 << line1 << "," << z_score << endl;
            file6 << line2 << "," << z_score << endl;
            balance += trade*(-prices1[i] + prices2[i]);
        }
        else if (trade < 0)
        {
            position1 = lastPos + trade;
            line1 = currDate + "," + "SELL," + to_string(-trade) + "," + to_string(prices1[i]);
            line2 = currDate + "," + "BUY," + to_string(-trade) + "," + to_string(prices2[i]);
            file5 << line1 << "," << z_score << endl;
            file6 << line2 << "," << z_score << endl;
            balance += trade*(-prices1[i] + prices2[i]);
        }
        file3 << currDate << "," << balance << endl;
    }
    balance = balance + position1 * (prices1.back()) - position1 * (prices2.back());

    file4 << balance << endl;

    file5.close();
    file6.close();
    file3.close();
    file4.close();
    return balance;
}

double stop_loss_strategy(string symbol1, string symbol2, int x, int n, double threshold, double stop_loss_threshold, string from_date, string to_date){
    string comm = "python3 file_generator.py strategy=PAIRS symbol1=" + symbol1 + " symbol2=" + symbol2 + " n=" + to_string(n) + " from_date=" + from_date + " to_date=" + to_date;
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
        double pnl = strategize_stop_loss(x, n, threshold,stop_loss_threshold);
    }
    remove("pairs-1.csv");
    remove("pairs-2.csv");
    return pnl;
}