#include <iostream>
#include <string>
#include <unordered_map>
#include<ADX_strat.cpp>
#include<RSI_strat.cpp>
#include<MACD_strat.cpp>
#include<basic_strat.cpp>
using namespace std;

void ADX_strategy(string symbol, int n, int x, double adx_threshold, string from_date, string to_date){
    string comm = "python3 file_generator.py strategy=ADX symbol="+symbol+" n="+to_string(n)+" from_date="+from_date+" to_date="+to_date;
    const char* command = comm.c_str();
    int files_generated = system(command);
    if(files_generated == -1){
        cout<<"Failed to generate files using python"<<endl;
    }else{
        cout<<"File generation using python successful"<<endl;
        strategize_ADX(n, x, adx_threshold, from_date, to_date);
    }
    remove("stock_data.csv");
}

void RSI_strategy(string symbol, int n, int x, double overbought_threshold, double oversold_threshold, string from_date, string to_date){
    string comm = "python3 file_generator.py strategy=RSI symbol="+symbol+" n="+to_string(n)+" from_date="+from_date+" to_date="+to_date;
    const char* command = comm.c_str();
    int files_generated = system(command);
    if(files_generated == -1){
        cout<<"Failed to generate files using python"<<endl;
    }else{
        cout<<"File generation using python successful"<<endl;
        strategize_RSI(n, x, overbought_threshold, oversold_threshold, from_date, to_date);
    }
    remove("stock_data.csv");
}

void MACD_strategy(string symbol, int x, string from_date, string to_date){
    string comm = "python3 file_generator.py strategy=MACD symbol="+symbol+" n="+to_string(n)+" from_date="+from_date+" to_date="+to_date;
    const char* command = comm.c_str();
    int files_generated = system(command);
    if(files_generated == -1){
        cout<<"Failed to generate files using python"<<endl;
    }else{
        cout<<"File generation using python successful"<<endl;
        strategize_MACD(x, from_date, to_date);
    }
    remove("short_stock_data.csv");
    remove("long_stock_data.csv");
}

void BASIC_strategy(string symbol, int n, int x, double overbought_threshold, double oversold_threshold, string from_date, string to_date){
    string comm = "python3 file_generator.py strategy=BASIC symbol="+symbol+" n="+to_string(n)+" from_date="+from_date+" to_date="+to_date;
    const char* command = comm.c_str();
    int files_generated = system(command);
    if(files_generated == -1){
        cout<<"Failed to generate files using python"<<endl;
    }else{
        cout<<"File generation using python successful"<<endl;
        strategize_BASIC(n, x);
    }
    remove("stock_data.csv");
}

// Define your functions for each strategy
void DMA_plus_plus_strategy(const std::string& symbol, int x, int p, int n, int max_hold_days, double c1, double c2, const std::string& start_date, const std::string& end_date) {
    std::cout << "DMA++ strategy called with symbol: " << symbol << ", x: " << x << ", p: " << p << ", n: " << n << ", max hold days: " << max_hold_days << ", c1: " << c1 << ", c2: " << c2 << ", start date: " << start_date << ", end date: " << end_date << std::endl;
    // Implement DMA++ strategy here
}

void DMA_strategy(const std::string& symbol, int n, int x, int p, const std::string& start_date, const std::string& end_date) {
    std::cout << "DMA strategy called with symbol: " << symbol << ", n: " << n << ", x: " << x << ", p: " << p << ", start date: " << start_date << ", end date: " << end_date << std::endl;
    // Implement DMA strategy here
}


void LINEAR_REGRESSION_strategy(const std::string& symbol, int x, int p, const std::string& train_start_date, const std::string& train_end_date, const std::string& start_date, const std::string& end_date) {
    std::cout << "LINEAR REGRESSION strategy called with symbol: " << symbol << ", x: " << x << ", p: " << p << ", train start date: " << train_start_date << ", train end date: " << train_end_date << ", start date: " << start_date << ", end date: " << end_date << std::endl;
    // Implement LINEAR REGRESSION strategy here
}




int main(int argc, char* argv[]) {
    // Check if arguments are provided
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " strategy=<strategy_name> symbol=<symbol_name> [other_arguments...]" << std::endl;
        return 1;
    }

    // Parse arguments into a map
    std::unordered_map<std::string, std::string> args;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        size_t pos = arg.find('=');
        if (pos != std::string::npos) {
            std::string key = arg.substr(0, pos);
            std::string value = arg.substr(pos + 1);
            args[key] = value;
        }
    }

    // Check if strategy argument is provided
    if (args.find("strategy") == args.end()) {
        std::cerr << "Error: Strategy argument is missing!" << std::endl;
        return 1;
    }

    // Extract strategy name
    std::string strategy = args["strategy"];

    // Call appropriate function based on strategy
    if (strategy == "DMA++") {
        DMA_plus_plus_strategy(args["symbol"], std::stoi(args["x"]), std::stoi(args["p"]), std::stoi(args["n"]), std::stoi(args["max_hold_days"]), std::stod(args["c1"]), std::stod(args["c2"]), args["start_date"], args["end_date"]);
    } else if (strategy == "DMA") {
        DMA_strategy(args["symbol"], std::stoi(args["n"]), std::stoi(args["x"]), std::stoi(args["p"]), args["start_date"], args["end_date"]);
    } else if (strategy == "BASIC") {
        // BASIC_strategy(args["symbol"], std::stoi(args["n"]), std::stoi(args["x"]), args["start_date"], args["end_date"]);
    } else if (strategy == "MACD") {
        // MACD_strategy(args["symbol"], std::stoi(args["x"]), args["start_date"], args["end_date"]);
    } else if (strategy == "RSI") {
        // RSI_strategy(args["symbol"], std::stoi(args["x"]), std::stoi(args["n"]), std::stod(args["oversold_threshold"]), std::stod(args["overbought_threshold"]), args["start_date"], args["end_date"]);
    } else if (strategy == "ADX") {
        // ADX_strategy(args["symbol"], std::stoi(args["x"]), std::stoi(args["n"]), std::stod(args["adx_threshold"]), args["start_date"], args["end_date"]);
    } else if (strategy == "LINEAR_REGRESSION") {
        LINEAR_REGRESSION_strategy(args["symbol"], std::stoi(args["x"]), std::stoi(args["p"]), args["train_start_date"], args["train_end_date"], args["start_date"], args["end_date"]);
    } else {
        std::cerr << "Error: Unknown strategy!" << std::endl;
        return 1;
    }

    return 0;
}
