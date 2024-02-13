import sys
from datetime import date, datetime
from jugaad_data.nse import stock_csv, stock_df
import pandas as pd
from dateutil.relativedelta import relativedelta
from jugaad_data.nse import NSELive

    # string comm = "python3 file_generator.py strategy=MACD symbol="+symbol+" n="+to_string(0)+" from_date="+from_date+" to_date="+to_date;

strategy = sys.argv[1][9:]
if strategy=="PAIRS":
    sym1=sys.argv[2][8:]
    sym2=sys.argv[3][8:]
    n=sys.argv[4][2:]
    from_date=sys.argv[5][10:]
    to_date=sys.argv[6][8:]
elif strategy =="LINEAR_REGRESSION":
    sym=sys.argv[2][7:]
    train_start_date=sys.argv[3][17:]
    train_end_date=sys.argv[4][15:]
    from_date=sys.argv[5][10:]
    to_date=sys.argv[6][8:]
else:
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

#  Strategy can be = "ADX","RSI","MACD","DMA","LR","BASIC","BEST","Advanced_DMA"

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
def generate_files2(symbol1,symbol2, n, start_date,end_date,file_name1,file_name2,strategy):
    f = parse_date(start_date)
    t = parse_date(end_date)
    fdd, fmm, fyyyy = f[0], f[1], f[2]
    tdd, tmm, tyyyy = t[0], t[1], t[2]
    adjusted_date=date(fyyyy,fmm,fdd)-relativedelta(days=2*int(n))
    
    df = stock_df(symbol=symbol1, from_date=date(fyyyy,fmm,fdd), to_date=date(tyyyy, tmm, tdd), series=get_series(symbol1))
    df2 = stock_df(symbol=symbol1, from_date=adjusted_date, to_date=date(fyyyy, fmm, fdd), series=get_series(symbol1))
    columns_to_keep=['DATE','CLOSE']
    df = df[columns_to_keep]
    df2 = df2[columns_to_keep]
    df2=df2.head(int(n)-1)
    df = df.iloc[::-1]
    df2 = df2.iloc[::-1]
    df = pd.concat([df2,df],ignore_index=True)
    df.to_csv(file_name1)
    
    df = stock_df(symbol=symbol2, from_date=date(fyyyy,fmm,fdd), to_date=date(tyyyy, tmm, tdd), series=get_series(symbol2))
    df2 = stock_df(symbol=symbol2, from_date=adjusted_date, to_date=date(fyyyy, fmm, fdd)-relativedelta(days=1), series=get_series(symbol2))
    columns_to_keep=['DATE','CLOSE']
    df = df[columns_to_keep]
    df2 = df2[columns_to_keep]
    df2=df2.head(int(n)-1)
    df = df.iloc[::-1]
    df2 = df2.iloc[::-1]
    df = pd.concat([df2,df],ignore_index=True)
    df.to_csv(file_name2)

def generate_files3(file_name,symbol,from_date,to_date):
    f = parse_date(from_date)
    t = parse_date(to_date)
    fdd, fmm, fyyyy = f[0], f[1], f[2]
    tdd, tmm, tyyyy = t[0], t[1], t[2]
    adjusted_date=date(fyyyy,fmm,fdd)-relativedelta(days=3)
    df = stock_df(symbol=symbol, from_date=date(fyyyy,fmm,fdd), to_date=date(tyyyy, tmm, tdd), series=get_series(symbol))
    columns_to_keep=['DATE','CLOSE','OPEN','VWAP','LOW','HIGH','NO OF TRADES']
    df = df[columns_to_keep]
    df = df.iloc[::-1]

    df2 = stock_df(symbol=symbol, from_date=adjusted_date, to_date=date(fyyyy, fmm, fdd)-relativedelta(days=1), series=get_series(symbol))
    df2 = df2[columns_to_keep]
    df2=df2.head(1)
    df2 = df2.iloc[::-1]
    df = pd.concat([df2,df],ignore_index=True)
    df.to_csv(file_name)
# if strategy == "MACD":
    # generate_files(sym, 0, from_date, to_date, 'MACD_stock_data.csv', strategy)
    # generate_files(sym, 0, from_date, to_date, 'MACD_long_stock_data.csv', strategy)
# else:
if strategy == "PAIRS":
    file_name1 = "pairs-1.csv"
    file_name2 = "pairs-2.csv"
    generate_files2(sym1,sym2,n,from_date,to_date,file_name1,file_name2,strategy)
elif strategy=="LINEAR_REGRESSION":
    generate_files3("linear1.csv",sym,train_start_date,train_end_date)
    generate_files3("linear2.csv", sym, from_date, to_date)
else:
    file_name = strategy + "_stock_data.csv"
    generate_files(sym, n, from_date, to_date, file_name, strategy)