CXX = g++
CXXFLAGS = -std=c++11 -Wall
TARGET = main

.PHONY: all clean

all: $(TARGET)
	./$(TARGET) $(strategy) $(ARGS)

$(TARGET): $(TARGET).o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TARGET).o: main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(TARGET).o

ifeq ($(strategy),BASIC)
ARGS += $(symbol) $(n) $(x) $(start_date) $(end_date)
endif

ifeq ($(strategy),DMA)
ARGS += $(symbol) $(n) $(x) $(p) $(start_date) $(end_date)
endif

ifeq ($(strategy),DMA++)
ARGS += $(symbol) $(x) $(p) $(n) $(max_hold_days) $(c1) $(c2) $(start_date) $(end_date)
endif

ifeq ($(strategy),MACD)
ARGS += $(symbol) $(x) $(start_date) $(end_date)
endif

ifeq ($(strategy),RSI)
ARGS += $(symbol) $(x) $(n) $(oversold_threshold) $(overbought_threshold) $(start_date) $(end_date)
endif

ifeq ($(strategy),ADX)
ARGS += $(symbol) $(x) $(n) $(adx_threshold) $(start_date) $(end_date)
endif