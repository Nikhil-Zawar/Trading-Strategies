import sys
from datetime import date, datetime
from dateutil.relativedelta import relativedelta
from jugaad_data.nse import stock_csv, stock_df, NSELive
import pandas as pd

symbol = sys.argv[1][7:]
n = sys.argv[2][2:]
from_date = sys.argv[5][10:]
to_date = sys.argv[6][8:]


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

def get_series(symbol):
    n = NSELive()
    q = n.stock_quote(symbol)
    q = q['metadata']
    series = q['series']
    return series
def gen_files(symbol, n, start_date, end_date):
    f = parse_date(start_date)
    t = parse_date(end_date)
    fdd, fmm, fyyyy = f[0], f[1], f[2]
    tdd, tmm, tyyyy = t[0], t[1], t[2]
    adjusted_date=date(fyyyy,fmm,fdd)-relativedelta(days=2*int(n))
    df = stock_df(symbol=symbol, from_date=date(fyyyy,fmm,fdd), to_date=date(tyyyy, tmm, tdd), series=get_series(symbol))
    df2 = stock_df(symbol=symbol, from_date=adjusted_date, to_date=date(fyyyy, fmm, fdd)-relativedelta(days=1), series=get_series(symbol))
    columns_to_keep=['DATE','CLOSE','SYMBOL']
    df = df[columns_to_keep]
    df2 = df2[columns_to_keep]
    df2=df2.head(int(n)-1)
    df = df.iloc[::-1]
    df2 = df2.iloc[::-1]
    df = pd.concat([df2,df],ignore_index=True)
    df.to_csv('stock_data.csv')


gen_files(symbol, n, from_date, to_date)
