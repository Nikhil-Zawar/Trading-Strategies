#include"base.h"

double strategize_BASIC(int n, int x, string start_date, string end_date){
    vector<string> dates;
    vector<double> prices;
    csv_parser("BASIC_stock_data.csv", dates, prices);

    ofstream stats_file("order_statistics.csv");
    ofstream cashflow_file("daily_cashflow.csv");
    if(!stats_file.is_open()){cout<<"Order stats not created"<<endl; return 0;}
    if(!cashflow_file.is_open()){cout<<"Cashflow not created"<<endl; return 0;}
    stats_file << "Date,Order_dir,Quantity,Price"<<endl;
    cashflow_file<<"Date,Cashflow"<<endl;

    double balance = 0;
    int position = 0;
    
    for(int i=0;i<dates.size(); i++){
    int counter_buy = 0;
    int counter_sell =0;
        if((i + n) > dates.size()){
            break;
        }else{
            for(int j =i; j<n+i+1; j++){
                if(prices[j] < prices[j+1]){
                    counter_buy++;
                }else if(prices[j] > prices[j+1]){
                    counter_sell++;
                }else{}
            }
            if(counter_buy == n && position < x){
                string s = dates[i+n] + ",BUY,1," + to_string(prices[i+n]);
                stats_file << s << endl;
                balance = balance - prices[i+n];
                string c = dates[i+n] + "," + to_string(balance);
                cashflow_file << c << endl;
                position++;
            }else if (counter_sell ==  n && position > ((-1)*x)){
                string s = dates[i + n] + ",SELL,1," + to_string(prices[i+n]);
                stats_file<< s << endl;
                balance = balance + prices[i+n];
                string c = dates[i+n] + "," + to_string(balance);
                cashflow_file << c << endl;
                position--;
            }else{
                string c = dates[i+n] + "," + to_string(balance);
                cashflow_file << c << endl; 
            }
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


double BASIC_strategy(string symbol, int n, int x, string from_date, string to_date){
    string comm = "python3 file_generator.py strategy=BASIC symbol="+symbol+" n="+to_string(n)+" from_date="+from_date+" to_date="+to_date;
    const char* command = comm.c_str();
    double pnl =0;
    int files_generated = system(command);
    if(files_generated == -1){
        cout<<"Failed to generate files using python"<<endl;
    }else{
        cout<<"File generation using python successful"<<endl;
        pnl = strategize_BASIC(n, x, from_date, to_date);
    }
    remove("BASIC_stock_data.csv");
    return pnl;
}

// int main(){
//     BASIC_strategy("SBIN",14, 5, "01/01/2023", "01/01/2024");
//     return 0;
// }
