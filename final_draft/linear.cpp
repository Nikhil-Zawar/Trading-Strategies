#include"base.h"

void transpose(const vector<vector<double>> &mat, vector<vector<double>> &transposedMat)
{
    int n = mat.size();
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < mat[0].size(); ++j)
        {
            transposedMat[j][i] = mat[i][j];
        }
    }
}

vector<vector<double>> matrix_mul(vector<vector<double>> A, vector<vector<double>> B)
{
    vector<vector<double>> result(A.size(), vector<double>(B[0].size()));
    for (int i = 0; i < A.size(); i++)
    {
        for (int j = 0; j < B[0].size(); j++)
        {
            double sum = 0;
            for (int k = 0; k < A[0].size(); k++)
            {
                sum += A[i][k] * B[k][j];
            }
            result[i][j] = sum;
        }
    }
    return result;
}
vector<double> multiply(vector<vector<double>> temp, vector<double> target)
{
    vector<double> coefficients(temp.size());
    for (int i = 0; i < temp.size(); i++)
    {
        double sum = 0;
        for (int j = 0; j < target.size(); j++)
        {
            sum += temp[i][j] * target[j];
        }
        coefficients[i] = sum;
    }
    return coefficients;
}
double valuePrice(vector<double> result, vector<double> v){
    double value =0;
    for (int i = 0; i < result.size(); i++)
    {
        value += v[i]* result[i];

    }
    return value;
    
}

void gaussianElimination(vector<vector<double>>& matrix, vector<vector<double>>& identity) {
    int n = matrix.size();

    // Forward elimination
    for (int i = 0; i < n; ++i) {
        // Partial pivoting
        int maxRow = i;
        for (int j = i + 1; j < n; ++j) {
            if (abs(matrix[j][i]) > abs(matrix[maxRow][i])) {
                maxRow = j;
            }
        }
        if (maxRow != i) {
            swap(matrix[i], matrix[maxRow]);
            swap(identity[i], identity[maxRow]);
        }

        double pivot = matrix[i][i];
        for (int j = i + 1; j < n; ++j) {
            double ratio = matrix[j][i] / pivot;
            for (int k = 0; k < n; ++k) {
                matrix[j][k] -= ratio * matrix[i][k];
                identity[j][k] -= ratio * identity[i][k];
            }
        }
    }

    // Back substitution
    for (int i = n - 1; i > 0; --i) {
        for (int j = i - 1; j >= 0; --j) {
            double ratio = matrix[j][i] / matrix[i][i];
            for (int k = 0; k < n; ++k) {
                matrix[j][k] -= ratio * matrix[i][k];
                identity[j][k] -= ratio * identity[i][k];
            }
        }
    }

    // Scaling rows to make diagonal elements 1
    for (int i = 0; i < n; ++i) {
        double scale = matrix[i][i];
        for (int j = 0; j < n; ++j) {
            matrix[i][j] /= scale;
            identity[i][j] /= scale;
        }
    }
}

vector<vector<double>> inverseMatrix(const vector<vector<double>>& matrix) {
    int n = matrix.size();

    // Create an identity matrix
    vector<vector<double>> identity(n, vector<double>(n, 0));
    for (int i = 0; i < n; ++i) {
        identity[i][i] = 1;
    }

    // Make a copy of the original matrix
    vector<vector<double>> tempMatrix = matrix;

    // Perform Gaussian elimination with partial pivoting
    gaussianElimination(tempMatrix, identity);

    return identity;
}


double strategize_linear(int x, double p)
{
    cout << "the strategy here" << endl;
    vector<string> dates;
    vector<vector<double>> prices;
    vector<double> target;
    csv_parser_linear("linear1.csv", dates, target, prices);

    vector<vector<double>> transposed_prices(prices[0].size(), vector<double>(prices.size()));
    transpose(prices, transposed_prices);
    vector<vector<double>> temp = matrix_mul(transposed_prices, prices);
    vector<vector<double>> temp2=inverseMatrix(temp);
    temp2 = matrix_mul(temp2, transposed_prices);
    vector<double> result = multiply(temp2,target);

    dates.clear();
    target.clear();
    prices.clear();
    csv_parser_linear("linear2.csv",dates,target,prices);

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
    double diff1;                // condition for buying
    double diff2;                // condition for selling
    for (int i = 0; i < prices.size(); i++)
    {
        string currDate = dates[i];
        double predictedPrice = valuePrice(result,prices[i]);
        diff1 = predictedPrice - (1+p/100)*target[i];
        if (diff1>=0)
        {
            if(position >=x ){
                file3 << currDate << "," << balance << endl;
                continue;
                
            }
            file2 << currDate<<",BUY,1,"<<target[i] << endl;
            balance -= target[i];
            file3 << currDate << "," << balance << endl;
            position++;
            continue;
        }
        diff2 = (1-p/100)*target[i] - predictedPrice;
        if(diff2 >=0){
            if (position <= -x)                                         // position limit
            {
                file3 << currDate << "," << balance << endl;
                continue;
            }
            file2 << currDate<<",SELL,1,"<<target[i] << endl;
            balance += target[i];
            file3 << currDate << "," << balance << endl;
            position--;
            continue;
        }
        file3 << currDate << "," << balance << endl;                    // if no buying or selling

    }   
    balance = balance + position *(target.back());
    file4<<balance;
    file2.close();
    file3.close();
    file4.close();
    return balance;
}

double linear_strategy(string symbol, int x, double p, string train_start_date, string train_end_date, string from_date,string to_date){
    string comm = "python3 file_generator.py strategy=LINEAR_REGRESSION symbol=" + symbol + " train_start_date="+train_start_date+" train_end_date="+train_end_date+ " from_date=" + from_date + " to_date=" + to_date;
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
        pnl = strategize_linear( x, p);
    }
    remove("linear1.csv");
    remove("linear2.csv");
    return pnl;
}