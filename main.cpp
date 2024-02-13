#include"base.h"
#include"ADX_strat.cpp"
#include"BASIC_strat.cpp"
#include"DMA_strat.cpp"
#include"Advanced_DMA_strat.cpp"
#include"MACD_strat.cpp"
#include"RSI_strat.cpp"
#include"unordered_map"
#include"helper.cpp"

// make strategy=BASIC symbol=SBIN n=5 x=2 start_date="b" end_date="a"
// make strategy=DMA symbol=SBIN n=50 x=3 p=2 start_date="a" end_date="b"
// make strategy="DMA++" symbol=SBIN x=4 p=5 n=14 max_hold_days=28 c1=2 c2=0.2 start_date="a" end_date="b"
// make strategy=MACD symbol=SBIN x=3 start_date="a" end_date="b"
// make strategy=RSI symbol=SBIN x=3 n=14 oversold_threshold=30 overbought_threshold=70 start_date="a" end_date="b"
// make strategy=ADX symbol=SBIN x=3 n=14 adx_threshold=25 start_date="a" end_date="b"
// <<make strategy="LINEAR_REGRESSION" symbol=SBIN x=3 p=2 train_start_date="a" train_end_date="b" start_date="c" end_date="d"
// make strategy="BEST_OF_ALL" symbol=SBIN start_date="a" end_date="b"


int main(int argc, char* argv[]) {
    // Check if arguments are provided
    cout<<"entered main fun"<<endl;
    string strategy = argv[1];
    if(strategy == "BASIC"){
        string symbol = argv[2];
        int n = stoi(argv[3]);
        int x = stoi(argv[4]);
        string from_date = argv[5];
        string to_date = argv[6];
        double basic_pnl = BASIC_strategy(symbol, n, x, from_date, to_date);
    }else if(strategy == "DMA"){
        string symbol = argv[2];
        int n = stoi(argv[3]);
        int x = stoi(argv[4]);
        int p = stoi(argv[5]);
        string from_date = argv[5];
        string to_date = argv[6];
        double dma_pnl = DMA_strategy(symbol, n, x, p, from_date, to_date);
    }else if(strategy == "DMA++"){
        string symbol = argv[2];
        int x = stoi(argv[3]);
        int p = stoi(argv[4]);
        int n = stoi(argv[5]);
        int max_hold_days = stoi(argv[6]);
        double c1 = stod(argv[7]);
        double c2 = stod(argv[8]);
        string from_date = argv[9];
        string to_date = argv[10];
        double adv_dma_pnl = Advanced_DMA_strategy(symbol, n, x, p, c1, c2, max_hold_days, from_date, to_date);
    }else if(strategy == "MACD"){
        string symbol = argv[2];
        int x = stoi(argv[3]);
        string from_date = argv[4];
        string to_date = argv[5];
        double macd_pnl = MACD_strategy(symbol, x, from_date, to_date);
    }else if(strategy == "RSI"){
        string symbol = argv[2];
        int x = stoi(argv[3]);
        int n = stoi(argv[4]);
        double oversold_threshold = stod(argv[5]);
        double overbought_threshold = stod(argv[6]);
        string from_date = argv[7];
        string to_date = argv[8];
        double rsi_pnl = RSI_strategy(symbol, n, x, overbought_threshold, oversold_threshold, from_date, to_date);
    }else if(strategy == "ADX"){
        string symbol = argv[2];
        int x = stoi(argv[3]);
        int n = stoi(argv[4]);
        double adx_threshold = stod(argv[5]);
        string from_date = argv[5];
        string to_date = argv[6];
        double adx_pnl = ADX_strategy(symbol, n, x, adx_threshold, from_date, to_date);
    }else if(strategy == "BEST"){
        cout<<"best of all"<<endl;
    }else if(strategy == "LR"){
        cout<<"linear regression"<<endl;
    }else{

    }
    return 0;
}