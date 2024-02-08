import time
import csv
import os
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import sys
from io import StringIO
from datetime import date, datetime, timedelta
from jugaad_data.nse import stock_csv, stock_df
from dateutil.relativedelta import relativedelta

if len(sys.argv) != 3:
    print("not enough arguements")
    sys.exit(1)

sym = sys.argv[1]
num_years = int(sys.argv[2])

today_new = datetime.now()
today = datetime.now().date()
yesterday = today_new - timedelta(days=1)

one_year_ago = yesterday - relativedelta(years=1)
to_day, to_month, to_year = yesterday.day, yesterday.month, yesterday.year
from_day, from_month, from_year = one_year_ago.day, one_year_ago.month, one_year_ago.year

# to_day = yesterday.day
# to_month = yesterday.month
# to_year = yesterday.year

# from_day = to_day
# from_month = to_month
# from_year = to_year - num_years

st_time = time.time()
main_dataframe = stock_df(symbol=sym, from_date = date(from_year,from_month,from_day),
                            to_date = date(to_year,to_month,to_day), series = "EQ")
main_dataframe = main_dataframe[['DATE','OPEN','CLOSE','HIGH','LOW','LTP','VOLUME','VALUE','NO OF TRADES']]

get_data = time.time()
file_name = sym
# Write as CSV
main_dataframe.to_csv(file_name+'.csv', index=False)
check1 = time.time()
csv_time = (check1 - get_data)*1000
csv_size = os.path.getsize(file_name+'.csv')/1024
# Write as Text
main_dataframe.to_csv(file_name+'.txt', sep='\t', index=False)
check2 = time.time()
txt_time = 1000*(check2 - check1)
txt_size = os.path.getsize(file_name+'.txt')/1024
# Write in binary
main_dataframe.to_pickle(file_name+'.pkl')
check3 = time.time()
binary_time = 1000*(check3 - check2)
binary_size = os.path.getsize(file_name+'.pkl')/1024
# Write in Parquet
main_dataframe.to_parquet(file_name+'.parquet')
check4 = time.time()
parquet_time = 1000*(check4 - check3)
parquet_size = os.path.getsize(file_name+'.parquet')/1024
# Write in Feather
main_dataframe.to_feather(file_name+'.feather')
check5 = time.time()
feather_time = 1000*(check5 - check4)
feather_size = os.path.getsize(file_name+'.feather')/1024
# Write in HDF5
# main_dataframe.to_hdf(file_name+'.h5', key='data', mode='w')
check6 = time.time()
hdf5_time = 1000*(check6 - check5)
# hdf5_size = os.path.getsize(file_name+'.h5')/1024
# Write in MessagePack
# main_dataframe.to_msgpack(file_name+'_as_msgpack.msgpack')
check7 = time.time()
msgpack_time = (check7 - check6)*1000
# msgpack_size = os.path.getsize(file_name+'_as_msgpack.msgpack')/1024
# Write in Apache Arrow
# main_dataframe.to_arrow(file_name+'_as_arrow.arrow')
check8 = time.time()
arrow_time = 1000*(check8 - check7)
# arrow_size = os.path.getsize(file_name+'_as_arrow.arrow')/1024
# Write in ORC(Optimized Row Columnar)
main_dataframe.to_orc(file_name+'.orc')
check9 = time.time()
orc_time = 1000*(check9 - check8)
orc_size = os.path.getsize(file_name+'.orc')/1024
# Write in ORC(Optimized Row Columnar)

x = np.array(["CSV","Text","Binary","Parquet","Feather","ORC"])
y_time = np.array([csv_time, txt_time, binary_time, parquet_time, feather_time,orc_time])
y_size = np.array([csv_size,txt_size,binary_size,parquet_size,feather_size,orc_size])

plt.subplot(1, 2, 1)
plt.bar(x, y_time, color='blue')
plt.title("Time taken to generate the files")
plt.xlabel("File format")
plt.ylabel("Time (in milli seconds)")

plt.subplot(1, 2, 2)
plt.bar(x, y_size, color='red')
plt.title("Size of the generated files")
plt.xlabel("File format")
plt.ylabel("Size (in kilobytes)")

plt.show()
