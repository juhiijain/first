# Stock Market Simulator

This project is a C++ console-based Stock Market Simulator that allows users to simulate trading stocks, manage a portfolio, and track stock prices in real time. The application features live price updates, a watchlist, transaction history, and portfolio management.

## Features

- **Live Stock Prices:** View real-time updates of stock prices with volatility simulation.
- **Buy/Sell Stocks:** Trade stocks using a virtual balance.
- **Portfolio Management:** Track your holdings, balance, and total portfolio value.
- **Watchlist:** Add or remove stocks to a personal watchlist for quick access.
- **Transaction History:** View a detailed log of all buy/sell transactions.
- **Deposit/Withdraw Funds:** Manage your virtual cash balance.
- **Stock Details:** View detailed information about each stock, including recent price history, volatility, and peak prices.

## How It Works

- The simulator loads stock names, abbreviations, and initial prices from three text files: `stock_names.txt`, `stock_prices.txt`, and `stock_abbr.txt`.
- Stock prices are updated every second in a background thread, simulating market volatility.
- The user interacts with the application through a menu-driven interface.

## Menu Options

1. View All Stocks (Live)
2. Buy Stock
3. Sell Stock
4. View Portfolio
5. Add to Watchlist
6. Remove from Watchlist
7. View Watchlist
8. View Stock Details
9. Deposit Funds
10. Withdraw Funds
11. View Transaction History
12. Exit

## Getting Started

### Prerequisites
- C++ compiler (e.g., g++, MSVC)
- Windows OS (uses `conio.h` and `system("cls")`)

### Files Needed
- `beginning.cpp` (main source code)
- `stock_names.txt` (list of stock names, one per line)
- `stock_prices.txt` (list of initial stock prices, one per line)
- `stock_abbr.txt` (list of stock abbreviations, one per line)

### Compilation

Open a terminal in the project directory and run:

```
g++ -std=c++11 -o stock_simulator beginning.cpp
```

### Running the Program

```
./stock_simulator
```

## Notes
- The program uses ANSI escape codes and Windows-specific headers for screen clearing and keyboard input.
- The initial balance is set to $10,000.
- All data is stored in memory; no persistent storage is used for transactions or portfolio.


## License
This project is for educational purposes.
