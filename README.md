# Trading-Strategies

### file_generator.py
- Note that this python file will be used to generate stock data across all the strategies
- Add here if any changes are required in the python file to generate specific data
- The file generated will be named as 'stock_data.csv' by default.
- Adhere to the python command to be passed in the cpp file : command : python3 file_generator.py strategy="" symbol="" n="" from_date="" end_date=""
   - For Basic Strategy : python3 file_generator.py strategy=basic symbol=SBIN n=11 from_date=01/01/2023 end_date=01/01/2024
   - For DMA Strategy : python3 file_generator.py strategy=DMA symbol=SBIN n=11 from_date=01/01/2023 end_date=01/01/2024
   - For DMA++ Strategy : python3 file_generator.py strategy=DMA_advanced symbol=SBIN n=11 from_date=01/01/2023 end_date=01/01/2024
   - For MACD Strategy : python3 file_generator.py strategy=MACD symbol=SBIN n=1 from_date=01/01/2023 end_date=01/01/2024 (Note, here n does not matter)
   - For RSI Strategy : python3 file_generator.py strategy=RSI symbol=SBIN n=11 from_date=01/01/2023 end_date=01/01/2024
   - For ADX Strategy : python3 file_generator.py strategy=ADX symbol=SBIN n=11 from_date=01/01/2023 end_date=01/01/2024
