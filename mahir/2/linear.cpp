#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
using namespace std;
string format_date(string date)
{
    // 2022-11-30
    string new_date = "";
    string temp = "";
    // new format = dd/mm/yyyy
    for (int i = date.size() - 1; i >= 0; i--)
    {
        if (date[i] == '-')
        {
            int num = stoi(temp);
            new_date = new_date + temp + "/";
            temp = "";
        }
        else
        {
            temp = date[i] + temp;
        }
    }
    new_date = new_date + temp;
    return new_date;
}
void csv_parser(string file_name, vector<string> &dates, vector<double> &target, vector<vector<double>> &prices)
{
    ifstream file(file_name);
    if (!file.is_open())
    {
        cout << "Error opening the " << file_name << " file" << endl;
        return;
    }
    string line;
    getline(file, line); // skip first line

    while (getline(file, line))
    {
        stringstream ss(line);
        string cell;
        getline(ss, cell, ','); // skip first words

        getline(ss, cell, ','); // take first word (date)
        dates.push_back(format_date(cell));
        vector<double> v;
        // prices[0].push_back(1);
        v.push_back(1);
        for (int i = 1; i < 7; i++)
        {

            getline(ss, cell, ',');
            v.push_back(stod(cell)); // take second word (close price)
        }
        prices.push_back(v);
    }
    // prices.push_back({});                           // last column i.e. open price of current date
    for (int i = 1; i < prices.size(); i++)
    {
        prices[i - 1].push_back(prices[i][2]);
        target.push_back(prices[i][1]);
    }
    prices.erase(prices.end() - 1);
    dates.erase(dates.begin());
    // cout << target[7] << endl;

    file.close();
}
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

// Function to calculate the inverse of a matrix
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


void strategize(int x, double p, string train_start_date, string train_end_date)
{
    cout << "the strategy here" << endl;
    vector<string> dates;
    vector<vector<double>> prices;
    vector<double> target;
    csv_parser("linear.csv", dates, target, prices);

    vector<vector<double>> transposed_prices(prices[0].size(), vector<double>(prices.size()));
    transpose(prices, transposed_prices);
    vector<vector<double>> temp = matrix_mul(transposed_prices, prices);
    vector<vector<double>> temp2=inverseMatrix(temp);
    temp2 = matrix_mul(temp2, transposed_prices);
    vector<double> result = multiply(temp2,target);

    dates.clear();
    target.clear();
    prices.clear();
    csv_parser("stock_data.csv",dates,target,prices);

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
    float diff1;                // condition for buying
    float diff2;                // condition for selling
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
            string line = currDate + ",BUY,1," + to_string(target[i]);
            file2 << line << endl;
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
            string line = currDate + ",SELL,1," + to_string(target[i]);
            file2 << line << endl;
            balance += target[i];
            file3 << currDate << "," << balance << endl;
            position--;
            continue;
        }
        file3 << currDate << "," << balance << endl;                    // if no buying or selling

    }   
    balance = balance + position *(target.back());
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
    remove("linear.csv")
    remove("stock_data.csv")

}

int main()
{
    int x = 3;
    double p = 1;
    string train_start_date = "01/01/2020";
    string train_end_date = "03/01/2024";
    const char *file_command = "python3 linear.py symbol=SBIN x=3 p=2 train_start_date=01/01/2020 train_end_date=03/01/2024 start_date=12/01/2023 end_date=31/01/2024";
    int files_generated = system(file_command);
    if (file_command == 0)
    {
        cout << "Failed : file generation did not happen from the python file";
    }
    else
    {
        strategize(x, p, train_start_date, train_end_date);
    }
    // remove("trend_strat.csv");
    return 0;
}