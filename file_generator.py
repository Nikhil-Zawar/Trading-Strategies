import sys
from datetime import date, datetime
from jugaad_data.nse import stock_csv, stock_df
import pandas as pd
from dateutil.relativedelta import relativedelta
from jugaad_data.nse import NSELive

strategy = sys.argv[1][9:]
sym = sys.argv[2][7:]
n = sys.argv[3][2:]
from_date = sys.argv[4][10:]
to_date = sys.argv[5][8:]

# This function takes a string input and returns a list output
# The elements in the list are integers in the order - [dd, mm, yyyy]
def parse_date(str):
    lst = []
    c = ""
    for i in range(len(str)):
        if str[i] == "/":
            lst.append(int(c))
            c = ""
        else:
            c = c + str[i]
    lst.append(int(c))
    return lst

def get_series(symbol):
    n = NSELive()
    q = n.stock_quote(symbol)
    q = q['metadata']
    series = q['series']
    return series

def generate_files(symbol, n, start_date, end_date, file_name, strategy):
    f = parse_date(start_date)
    t = parse_date(end_date)
    fdd, fmm, fyyyy = f[0], f[1], f[2]
    tdd, tmm, tyyyy = t[0], t[1], t[2]
    adjusted_date=date(fyyyy,fmm,fdd)-relativedelta(days=2*int(n))
    df_main = stock_df(symbol=symbol, from_date=date(fyyyy,fmm,fdd), to_date=date(tyyyy, tmm, tdd), series=get_series(symbol))
    # print("The number of days we will calculate for is : " + str(df_main.shape[0]))
    df_before_start = stock_df(symbol=symbol, from_date=adjusted_date, to_date=date(fyyyy, fmm, fdd)-relativedelta(days=1), series=get_series(symbol))
    columns_to_keep=[]
    if strategy=="ADX":
        columns_to_keep=['DATE','CLOSE','HIGH','LOW','PREV. CLOSE','SYMBOL']
    else:
        columns_to_keep=['DATE','CLOSE','SYMBOL']
    df_main = df_main[columns_to_keep]
    df_before_start = df_before_start[columns_to_keep]
    df_before_start = df_before_start.iloc[:int(n)]
    df_main = df_main.iloc[::-1]
    df_before_start = df_before_start.iloc[::-1]
    df_main = pd.concat([df_before_start, df_main], ignore_index=True)
    df_main.to_csv(file_name)


if strategy == "MACD":
    generate_files(sym, 12, from_date, to_date, 'short_stock_data.csv', strategy)
    generate_files(sym, 26, from_date, to_date, 'long_stock_data.csv', strategy)
else:
    generate_files(sym, n, from_date, to_date, 'stock_data.csv', strategy)

