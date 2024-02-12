#include<iostream>
#include<cstdlib>
#include<vector>
#include<fstream>
#include<sstream>

using namespace std;

struct csv_row_elements{
    string date;
    string for_date;
    double close;
    string symbol;
};

class transaction{
    public:
    double price;
    string date;
    string direction;
    int quantity = 1;
    double cashflow ;
    public:
    transaction(string da, double p, string dir){
        price = p;
        date = da;
        direction = dir;
        quantity = 1;
        cashflow = 0;
    }

};

// class cashflow{}

csv_row_elements parse_row_of_csv(string &line){
    csv_row_elements row;
    stringstream data(line);
    string single;
    std::getline(data, single, ',');
    row.date = single;
    std::getline(data, single, ',');
    row.close = stod(single);
    std::getline(data, single, ',');
    row.symbol = single;
    std::getline(data, single, ',');
    row.for_date = single;
    return row;
}

vector<csv_row_elements> all_data(){
    ifstream file("BASIC_stock_data.csv");
    vector<csv_row_elements> rows;
    if(!file.is_open()){
        std::cerr<<"Error Opening CSV file for stock data"<<endl;
        return rows;
    }else{
        string line;
        int i=0;
        while(std::getline(file,line)){
            if(i==0){
                i++;
            }else{
                csv_row_elements row = parse_row_of_csv(line);
                rows.push_back(row);
            }
        }
        file.close();
        return rows;
    }
}

void write_order_stat(vector<transaction> order_stats){
    ofstream csv_file("order_statistics.csv");
    csv_file << "Date,Order_dir,Quantity,Price" << endl;
    for (const auto& t : order_stats) {
        csv_file << t.date << "," << t.direction << "," << t.quantity << "," << t.price << endl;
    }
    csv_file.close();
    cout << "CSV file created successfully." << endl;
}

void write_cashflow(vector<transaction> order_stats, string from_date, string end_date){
    // Convert string dates to time points
    struct tm start_tm = {};
    struct tm end_tm = {};
    strptime(from_date.c_str(), "%Y-%m-%d", &start_tm);
    strptime(end_date.c_str(), "%Y-%m-%d", &end_tm);

    // Convert tm structs to time_t
    time_t start_time = mktime(&start_tm);
    time_t end_time = mktime(&end_tm);
    // Iterate through dates and print them
    while (start_time <= end_time) {
        char buffer[11];
        strftime(buffer, sizeof(buffer), "%d/%m/%Y", localtime(&start_time));
        // cout << buffer << endl;
        // Increment to the next day
        start_time += 86400; // 86400 seconds = 1 day
    }
}

void strategize_BASIC(int n, int x){
    vector<csv_row_elements> data = all_data();
    vector<transaction> order_stats;
    int num_days = (data.size()/n);
    int num_transactions = 0;
    for(int i=0;i<num_days;i++){ //this loop for the number of days we are going to check
        int curr_idx = i*n;
        int transaction_status = 0;
        // add 1 to this when the price is decreasing 
        // add 2 to this when the price is increasing
        // if the price decreases for n days then transaction_status = (n-1)*1
        // if the price increases for n days then transaction_status = (n-1)*2
        // if the value is anything else, then there is no monotonicity
        for(int j=n-1;j>0;j--){
            if(data[curr_idx + j].close > data[curr_idx + j -1].close){
                //this is the case where the price is decreasing strictly monotonically
                transaction_status =  transaction_status + 1;
            }else{
                //here the price is strictly increasing
                transaction_status = transaction_status + 2;
            }
        }
        double curr_price = data[i*n].close;
        string check_on_date = data[i*n].for_date;
        if(transaction_status == (n-1)){
            //strict decrease
            //Sell stock
            if(num_transactions*(-1) < x){
                transaction new_trans(check_on_date, curr_price, "SELL");
                num_transactions--;
                order_stats.push_back(new_trans);
                if(order_stats.size() == 1){
                    order_stats[0].cashflow = order_stats[0].price;
                }else{
                    order_stats[order_stats.size() - 1].cashflow = order_stats[order_stats.size() - 2].cashflow + order_stats[order_stats.size() - 1].price;
                }
                // cout<<"     SELL stock on : "<<check_on_date<<" , Current Position : "<<num_transactions<<endl;
            }else{
                // cout<<"         NO TRADE : No more positions allowed on : "<<check_on_date<<endl;

            }
        }
        if(transaction_status == 2*(n-1)){
            //strict increase
            //Buy Stock
            if(num_transactions < x){
                transaction new_trans(check_on_date, curr_price, "BUY");
                num_transactions++;
                order_stats.push_back(new_trans);
                if(order_stats.size() == 1){
                    order_stats[0].cashflow = (-1)*(order_stats[0].price);
                }else{
                    order_stats[order_stats.size() - 1].cashflow = order_stats[order_stats.size() - 2].cashflow - order_stats[order_stats.size() - 1].price;
                }
                // cout<<"     BUY stock on : "<<check_on_date<<" , Current Position : "<<num_transactions<<endl;
            }else{
                // cout<<"         NO TRADE : No more positions allowed on : "<<check_on_date<<endl;
            }
        }else{
            //No monotonicity
            //dont do anything
            // cout<<"         NO TRADE : No pattern observed on : "<<check_on_date<<endl;
        }
    }
    write_order_stat(order_stats);
}
