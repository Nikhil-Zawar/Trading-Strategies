#include <iostream>
#include <vector>
#include <pthread.h>
#include <string>
#include <unordered_map>
#include"ADX_strat.cpp"
#include"RSI_strat.cpp"
#include"MACD_strat.cpp"
#include"BASIC_strat.cpp"
#include"Advanced_DMA_strat.cpp"
#include"DMA_strat.cpp"

using namespace std;

struct StrategyInfo {
    std::string symbol;
    int n;
    int x;
    double p;
    double c1;
    double c2;
    int max_hold_days;
    double adx_threshold;
    double oversold_threshold;
    double overbought_threshold;
    std::string from_date;
    std::string to_date;
    double result;
};

void* executeBASICStrategy(void* arg) {
    StrategyInfo* info = static_cast<StrategyInfo*>(arg);
    info->result = BASIC_strategy(info->symbol, info->n, info->x, info->from_date, info->to_date);
    return nullptr;
}

void* executeDMAStrategy(void* arg) {
    StrategyInfo* info = static_cast<StrategyInfo*>(arg);
    info->result = DMA_strategy(info->symbol, info->n, info->x, info->p, info->from_date, info->to_date);
    return nullptr;
}

void* executeAdvancedDMAStrategy(void* arg) {
    StrategyInfo* info = static_cast<StrategyInfo*>(arg);
    info->result = Advanced_DMA_strategy(info->symbol, info->n, info->x, info->p, info->c1, info->c2, info->max_hold_days, info->from_date, info->to_date);
    return nullptr;
}

void* executeMACDStrategy(void* arg) {
    StrategyInfo* info = static_cast<StrategyInfo*>(arg);
    info->result = MACD_strategy(info->symbol, info->x, info->from_date, info->to_date);
    return nullptr;
}

void* executeRSIStrategy(void* arg) {
    StrategyInfo* info = static_cast<StrategyInfo*>(arg);
    info->result = RSI_strategy(info->symbol, info->n, info->x, info->overbought_threshold, info->oversold_threshold, info->from_date, info->to_date);
    return nullptr;
}

void* executeADXStrategy(void* arg) {
    StrategyInfo* info = static_cast<StrategyInfo*>(arg);
    info->result = ADX_strategy(info->symbol, info->n, info->x, info->adx_threshold, info->from_date, info->to_date);
    return nullptr;
}

int Best_of_all(string from_date, string to_date) {
    int num_of_strategies = 6;
    StrategyInfo BASIC_object = {"SBIN", 7, 5, 0, 0, 0, 0, 0, 0, 0, from_date, to_date};
    StrategyInfo DMA_object = {"SBIN", 50, 5, 2, 0, 0, 0, 0, 0, 0, from_date, to_date};
    StrategyInfo Advanced_DMA_object = {"SBIN", 14, 5, 5, 2, 0.2, 28, 0, 0, 0, from_date, to_date};
    StrategyInfo MACD_object = {"SBIN", 14, 5, 0, 0, 0, 0, 0, 0, 0, from_date, to_date};
    StrategyInfo RSI_object = {"SBIN", 14, 5, 0, 0, 0, 0, 0, 30, 70, from_date, to_date};
    StrategyInfo ADX_object = {"SBIN", 14, 5, 0, 0, 0, 0, 25, 0, 0, from_date, to_date};

    pthread_t threads[num_of_strategies];

    for (size_t i = 0; i < num_of_strategies; ++i) {
        switch (i) {
            case 0:
                pthread_create(&threads[i], nullptr, executeBASICStrategy, &BASIC_object);
                break;
            case 1:
                pthread_create(&threads[i], nullptr, executeDMAStrategy, &DMA_object);
                break;
            case 2:
                pthread_create(&threads[i], nullptr, executeAdvancedDMAStrategy, &Advanced_DMA_object);
                break;
            case 3:
                pthread_create(&threads[i], nullptr, executeMACDStrategy, &MACD_object);
                break;
            case 4:
                pthread_create(&threads[i], nullptr, executeRSIStrategy, &RSI_object);
                break;
            case 5:
                pthread_create(&threads[i], nullptr, executeADXStrategy, &ADX_object);
                break;
            default:
                break;
        }
    }

    // Wait for all threads to finish
    for (size_t i = 0; i < num_of_strategies; ++i) {
        pthread_join(threads[i], nullptr);
    }

    // Store the results in a vector
    std::vector<double> results;
    //ORder of results - BASIC, DMA, Advanced DMA, MACD, RSI, ADX
    results.push_back(BASIC_object.result);
    results.push_back(DMA_object.result);
    results.push_back(Advanced_DMA_object.result);
    results.push_back(MACD_object.result);
    results.push_back(RSI_object.result);
    results.push_back(ADX_object.result);

    // Do something with the results
    for (double result : results) {
        std::cout << "Result: " << result << std::endl;
    }

    return 0;
}


int main(){
    string from_date = "01/01/2023";
    string to_date = "01/01/2024";
    int a = Best_of_all(from_date, to_date);
    return 1;
}
