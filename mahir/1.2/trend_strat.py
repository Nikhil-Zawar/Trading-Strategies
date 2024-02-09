import sys
from datetime import date, timedelta
from jugaad_data.nse import stock_csv, stock_df
import pandas as pd

symbol = sys.argv[1]
n = sys.argv[2]
from_date = sys.argv[5]
to_date = sys.argv[6]


def parse_date(strng):
    lst = []
    c = ""
    for i in range(len(strng)):
        if strng[i] == "/":
            lst.append(int(c))
            c = ""
        else:
            c = c + strng[i]
    lst.append(int(c))
    return lst


def gen_files(symbol, n, start_date, end_date):
    f = parse_date(start_date)
    t = parse_date(end_date)
    fdd, fmm, fyyyy = f[0], f[1], f[2]
    tdd, tmm, tyyyy = t[0], t[1], t[2]
    adjusted_date=date(fyyyy,fmm,fdd)-timedelta(days=2*int(n))
    df = stock_df(symbol=symbol, from_date=date(fyyyy,fmm,fdd), to_date=date(tyyyy, tmm, tdd), series="EQ")
    df2 = stock_df(symbol=symbol, from_date=adjusted_date, to_date=date(fyyyy, fmm, fdd)-timedelta(days=1), series="EQ")
    columns_to_keep=['DATE','CLOSE','SYMBOL']
    df = df[columns_to_keep]
    df2 = df2[columns_to_keep]
    df2=df2.tail(int(n)-1)
    df = df.iloc[::-1]
    df2 = df2.iloc[::-1]
    df = pd.concat([df2,df],ignore_index=True)
    df.to_csv('trend_strat.csv')


gen_files(symbol, n, from_date, to_date)
