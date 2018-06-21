//
//  main.cpp
//  FinancialMathematics
//
//  Created by Christian Wayi-Wayi on 20/06/2018.
//  Copyright © 2018 Christian Wayi-Wayi. All rights reserved.
// A program to compute Equity Trade Analytics

#include <iostream>
#include <cmath>
#include <string>
#include <stdexcept>
#include <time.h>
#include <array>
#include <vector>


using namespace std;


//Class Ticker
class Ticker{
    
    //Member Variable
private:
    
    string tickerSymbol;
    bool isCommon; //Type
    int lastDividend;
    double fixedDividend;// n/100
    int parValue;
    
public:
    
    // default constructor so that class instance could be passed as a parameter
    Ticker(){
        
    }
    
    
    //constructs instance Variables - instantiate object in meaningful state
    Ticker(const string ts, const bool cp, const int ld, const double fd, const int pv){
        tickerSymbol = ts;
        isCommon = cp;
        lastDividend = ld;
        fixedDividend = fd;
        parValue = pv;
    }
    
    //Access Methods
    string getTickerSymb(){
        return tickerSymbol;
    }
    
    
    double getTickerMarketPrice(){
        
        return 255; // Set to fixed value for example, I assume the market ticker price would be externally sourced via Market data API
    }
    
    //i. calculate the dividend yield
    double getDividendYield(){
        if(isCommon == 1){
            return lastDividend / getTickerMarketPrice();
        }else{
            return ((fixedDividend/100) * parValue) / getTickerMarketPrice();
        }
    }
    
    //ii. calculate the PE Ratio
    double getPERatio(){
        return getTickerMarketPrice()/ getDividendYield();
    }
    
    //Stock Type
    string getStockType(int cp){
        if(isCommon == 1){
            return "Common";
        }else{
            return "Preferred";
        }
    }
    
    
};

//Class Trade
class Trade{
    
    //Member Variable
private:
    
    int tradeID;
    Ticker tradeTicker;
    struct tm *timeStamp;
    int quantity;
    bool isBuy;
    double tradePrice;
    
    
public:
    
    // Default constructor so that class instance could be passed as a parameter
    Trade(){
        
    }
    
    //constructor instance variables
    
    //iii.   construct a trade, with timestamp, quantity of shares, buy or sell indicator and price
    Trade(const int id, const Ticker &tt, const int q, const bool bs, const double tp){
        tradeID = id;
        tradeTicker = tt;
        quantity = q;
        isBuy = bs;
        tradePrice = tp;
        
        // create time stamp
        time_t timer;
        timer = time(NULL); // get current time value
        timeStamp = localtime (&timer); // convert to tm struct
    }
    
    //Access Methods
    
    string getTickerSymbols(){
        return tradeTicker.getTickerSymb();
    }
    
    struct tm getTimeStamp(){
        return *timeStamp;
    }
    
    double getTradePrice(){
        return tradePrice;
    }
    
    double getQuantity(){
        return quantity;
    }
    
    string getBuySell(int bs){
        if(isBuy == 1){
            return "Buy";
        }else{
            return "Sell";
        }
    }
    
    // Formatting - consider table
    void printTrade(){
        cout << "TradeId " << " Ticker " << " Quantity " << " Direction " << " TradedPrice " << endl;
        cout << tradeID << "   \t  ";
        cout << tradeTicker.getTickerSymb()<< "   \t  ";
        cout << quantity << " \t  ";
        cout << getBuySell(isBuy) << "   \t     ";
        cout << tradePrice << " \n\n\n ";
        
    }
    
};

//Class Blotter
class Blotter{
    
    //Member Variables
private:
    
    //Array of trade objects
    std::vector<Trade> tradeStore;
    
    // number of trades in array - always initially points empty index
    int tradeCounter;
    
    
public:
    
    //simple Constructor Blotter instance
    Blotter(){
        tradeCounter = 0;
    }
    
    //Access Methods
    
    //iii.    record a trade, with timestamp, quantity of shares, buy or sell indicator and price
    //        using trade constructor
    Trade recordTrade(const Ticker &tt, const int q, const bool bs, const double tp){
            // construct instance trade object
            Trade newTrade(tradeCounter,tt, q, bs, tp);
            //place trade into blotter
            tradeStore.push_back(newTrade);
            tradeCounter++;
            
        return newTrade;
        
    }
    
    
    //iv.    Calculate Stock Price based on trades recorded in past 15 minutes
    const double getStockPrice(Ticker stock){
        double stockPrice = 0;
        
        // upper portion of the eqution
        double sumTradePriceQuant = 0;
        // bottom portion of the equation
        double sumQuant = 0;
        
        // get the current time in some tm struct
        time_t timer;
        timer = time(NULL);
        struct tm *timeNow;
        timeNow = localtime (&timer);
        
        for(int i = 0; i < tradeCounter; i++){
            Trade currentTrade = tradeStore[i];
            // get trades for the stock we're after
            if (currentTrade.getTickerSymbols()==stock.getTickerSymb()){
                // check if the trade on stock has been within the last 15min
                if ((currentTrade.getTimeStamp().tm_min - timeNow -> tm_min) < 15){
                    sumTradePriceQuant = sumTradePriceQuant + currentTrade.getTradePrice() * currentTrade.getQuantity();
                    sumQuant = sumQuant + currentTrade.getQuantity();
                
                }
            }
            
            
        }
        
        // Compute Stock price
        if(sumQuant != 0){
            stockPrice = sumTradePriceQuant / sumQuant;
        }
        return stockPrice;
        
    }
    
    //Print Blotter
    void printBlotter(){
        cout <<  " Number of Trades Booked:  " << tradeCounter << "\n" << endl;
        for(int i = 0; i < tradeStore.size(); i++){
            tradeStore[i].printTrade();
        }
        
    }
    
    
    
};

// function to calculate geometric mean
double getGBCEAllShareIndex(vector<double>stockPrices){
    
    //number of elements within vector array
    long n = stockPrices.size();
    
    double sum = 0;
    
    // Compute the sum of all the
    // elements in the vector array.
    for(int i = 0; i < n; i++){
        sum = sum + log(stockPrices[i]);
    }
    
    // compute geometric mean through formula
    // antilog(((log(1) + log(2) + . . . + log(n))/n)
    // and return the value to main function.
    sum = sum / n;
    
    return exp(sum);
    
}


//main
int main( int argc, char ** argv ) {
    
    // Test Ticker Instances/Objects
    Ticker ticker1("TAE", 1, 0, 0.0,100);
    Ticker ticker2("POP", 1, 8, 0.0,100);
    Ticker ticker3("ALE", 1, 23, 0.0,60);
    Ticker ticker4("GIN", 0, 8, 2.0,100);
    Ticker ticker5("JOE", 1, 13, 0.0,250);
    
    // check whether ticker is common or preferred
    cout << " Stock Type of " << ticker2.getTickerSymb() << " is " << ticker2.getStockType(1) << endl;
    
    //a. For a given Stock,
    //i. Calculate the dividend yield
    cout << "The Dividend for " << ticker5.getTickerSymb()<< " is: " <<ticker3.getDividendYield() << endl;
    
    //ii. Calculate the PE Ratio
    cout << "The PE Ratio for " << ticker3.getTickerSymb()<< " is: " <<ticker3.getPERatio() << "\n" << endl;
    

    
    //Blotter
    //iii. Record a trade, //with timestamp, quantity of shares, buy or sell indicator and price
    Blotter bookPortfolio;
    //                       ticker, quantity, b/s, tradePrice
    bookPortfolio.recordTrade(ticker4, 100,    1,   210);
    bookPortfolio.recordTrade(ticker4, 50,    0,   221);
    bookPortfolio.recordTrade(ticker5, 250,    1,   215);
    bookPortfolio.recordTrade(ticker3, 1,    1,   355);
    
    //iii. Record a trade, //with timestamp, quantity of shares, buy or sell indicator and price
   //   1. Examples Trade
    bookPortfolio.printBlotter();

    
    // Making a record of Share Index
    std::vector<Ticker> tickerArr = {ticker1, ticker2, ticker3, ticker4, ticker5};
    int tickerCounter = 0;
    
    tickerCounter = tickerArr.size();
    std::vector<double> stockPriceArr;
    
    for (int i=0; i < tickerCounter; i++){
        Ticker currentTicker = tickerArr[i];
        // 0 price would mean there are no trades for this stock
        if(bookPortfolio.getStockPrice(currentTicker) != 0){
            double temp = bookPortfolio.getStockPrice(currentTicker);
            stockPriceArr.push_back(temp);
        }
    }
    
    //iv.    Calculate Stock Price based on trades recorded in past 15 minutes
    cout << "The Stock Price for POP is : " << bookPortfolio.getStockPrice(ticker5) << "\n"  << endl;
    
    
    //b.    Calculate the GBCE All Share Index using the geometric mean of prices for all stocks
    cout << "The GBCE for all share indeces is : " << getGBCEAllShareIndex(stockPriceArr)<< "\n"  << endl;
 
    return 0;
}
