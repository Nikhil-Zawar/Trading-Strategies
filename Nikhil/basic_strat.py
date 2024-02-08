import sys
from datetime import date
from jugaad_data.nse import stock_csv, stock_df

symbol = sys.argv[1]
n = sys.argv[2]
from_date = sys.argv[3]
to_date = sys.argv[4]

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


def gen_files(symbol, n, start_date, end_date):
    f = parse_date(start_date)
    t = parse_date(end_date)
    fdd, fmm, fyyyy = f[0],f[1],f[2]
    tdd, tmm, tyyyy = t[0],t[1],t[2]
    df = stock_df(symbol=symbol, from_date=date(fyyyy,fmm,fdd), to_date=date(tyyyy,tmm,tdd), series="EQ")
    print(df.head())


gen_files(symbol, n, from_date, to_date)
