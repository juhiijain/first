#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <thread>
#include <mutex>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <atomic>
#include <iomanip>
#include <queue>   // For storing recent prices
#include <conio.h>
using namespace std;
// Function to clear the console screen
void clearScreen()
{
    cout << "\033[2J\033[H"; // ANSI escape code to clear screen and move cursor to top-left
}
class Stock {
private:
    string fullName;
    string abbreviation;
    double price;
    double volatility;
    double peakHigh;
    double peakLow;
    queue<double> recentPrices;
    int historySize;

public:
    Stock(const string& fullName = "", const string& abbreviation = "", double price = 0.0, double volatility = 0.05,
          int historySize = 60)
        : fullName(fullName),
          abbreviation(abbreviation),
          price(price),
          volatility(volatility),
          peakHigh(price),
          peakLow(price),
          historySize(historySize) {
        for (int i = 0; i < historySize; ++i)
        {
            recentPrices.push(price);
        }
    }

    void updatePrice() {
        int randChange = rand() % 201 - 100;
        double changePercent = randChange / 10000.0 * (1 + volatility);
        price += price * changePercent;

        if (price > peakHigh) peakHigh = price;
        if (price < peakLow) peakLow = price;

        recentPrices.pop();
        recentPrices.push(price);
        // Randomly change volatility
        double volatilityChange = (rand() % 11 - 5) / 1000.0; // Change by +/- 0.005
        volatility += volatilityChange;
        if (volatility < 0.01) volatility = 0.01; // Ensure volatility doesn't go too low.
        if (volatility > 0.10) volatility = 0.10; // Ensure volatility doesn't go too high
    }

    string getFullName() const
    {
         return fullName;
    }
    string getAbbreviation() const
    {
        return abbreviation;
    }
    double getPrice() const
    {
        return price;
    }
    double getVolatility() const
    {
        return volatility;
    }
    double getPeakHigh() const
    {
        return peakHigh;
    }
    double getPeakLow() const
    {
        return peakLow;
    }
    queue<double> getRecentPrices() const
    {
        return recentPrices;
    }

    // Public getter for historySize
    int getHistorySize() const
    {
        return historySize;
    }
};

class StockMarket {
private:
    map<string, Stock> stocks;
    mutex mtx;
    atomic<bool> running{true};
    atomic<int> Userchoice;
    thread priceUpdateThread;
    int numStocks;

public:
    StockMarket() : priceUpdateThread(), numStocks(0), Userchoice(0) {}  // Initialize in constructor

    ~StockMarket()
     {
        stop(); // Ensure thread is stopped in destructor
        if (priceUpdateThread.joinable()) {
            priceUpdateThread.join(); // Wait for thread to finish.
        }
    }

    void loadStocks(const string& namesFile, const string& pricesFile, const string& abbrFile) {
        ifstream names(namesFile);
        ifstream prices(pricesFile);
        ifstream abbrs(abbrFile);
        string name, abbr;
        double price;
        vector<string> namesList, abbrList;
        vector<double> pricesList;

        while (getline(names, name) && getline(abbrs, abbr) && prices >> price)
        {
            namesList.push_back(name);
            abbrList.push_back(abbr);
            pricesList.push_back(price);
            prices.ignore();
        }
        if (namesList.size() != abbrList.size() || namesList.size() != pricesList.size())
        {
            cout << "Mismatch in the number of entries across files!" << endl;
            return;
        }
        for (size_t i = 0; i < namesList.size(); ++i)
        {
            stocks[abbrList[i]] = Stock(namesList[i], abbrList[i], pricesList[i]);
        }
        numStocks = namesList.size();
    }

    void startPriceUpdates()
    {
        if (!priceUpdateThread.joinable())
        { // Only start if not already running.
            priceUpdateThread = thread
            ([this]()
                {
                    while (running)
                    {
                        this_thread::sleep_for(chrono::seconds(1));
                        lock_guard<mutex> lock(mtx);
                        for (auto& [abbr, stock] : stocks)
                        {
                            stock.updatePrice();
                        }
                    }
                }
            );
        }
    }
    void stop()
    {
        running = false;
        if (priceUpdateThread.joinable()) {
            priceUpdateThread.join(); // Wait for the thread to finish.
        }
    }
    void showAllStocksLive()
     {
        system("cls"); //clears console screen
        cout << "\n--- Live Stock Prices ---\n";
        cout << left << setw(35) << "Stock" << "Price" << setw(15) << "Volatility" << endl;
        cout << string(50, '-') << endl;
        for (const auto& [abbr, stock]; : stocks)
        {
            cout << setw(35) << stock.getFullName() + " (" + stock.getAbbreviation() + "): "
                 << fixed << setprecision(2) << setw(10) << stock.getPrice()
                 << fixed << setprecision(2) << setw(15) << stock.getVolatility() * 100 << "%" << endl;
        }
        cout << endl;
        cout << "Press any character key" << endl;
    }
    void showMenu()
    {
        cout << "\n--- Stock Market Menu ---\n";
        cout << "1. View All Stocks (Live)\n";
        cout << "2. Buy Stock\n";
        cout << "3. Sell Stock\n";
        cout << "4. View Portfolio\n";
        cout << "5. Add to Watchlist\n";
        cout << "6. Remove from Watchlist\n";
        cout << "7. View Watchlist\n";
        cout << "8. View Stock Details\n";
        cout << "9. Deposit Funds\n";
        cout << "10. Withdraw Funds\n";
        cout << "11. View Transaction History\n";
        cout << "12. Exit\n";
        cout << "Enter choice: ";
    }
    Stock* getStock(const string& abbreviation)
    {
        lock_guard<mutex> lock(mtx);
        auto it = stocks.find(abbreviation);
        if (it != stocks.end())
        {
            return &it->second;
        }
        return nullptr;
    }

    void showStockDetails(const string& abbreviation) {
        lock_guard<mutex> lock(mtx);
        auto it = stocks.find(abbreviation);
        if (it != stocks.end())
        {
            const Stock& stock = it->second;
            cout << "\n--- Stock Details: " << stock.getFullName() << " (" << stock.getAbbreviation() << ") ---\n";
            cout << "Current Price: $" << fixed << setprecision(2) << stock.getPrice() << endl;
            cout << "Volatility: " << fixed << setprecision(2) << stock.getVolatility() * 100 << "%" << endl;
            cout << "Peak High (Last " << stock.getHistorySize() << " seconds): $" << fixed
                 << setprecision(2) << stock.getPeakHigh() << endl;
            cout << "Peak Low (Last " << stock.getHistorySize() << " seconds): $" << fixed
                 << setprecision(2) << stock.getPeakLow() << endl;
            cout << "Recent Prices (Last " << stock.getHistorySize() << " seconds): ";
            queue<double> tempQueue = stock.getRecentPrices();
            while (!tempQueue.empty())
            {
                cout << fixed << setprecision(2) << tempQueue.front() << "||";
                tempQueue.pop();
            }
            cout << endl;
        }
        else
        {
            cout << "Stock not found." << endl;
        }
    }
};
// Define a class for transaction history
class Transaction
{
public:
    string stockAbbreviation;
    int quantity;
    double price;
    time_t timestamp;
    Transaction(const string& abbr, int qty, double p, time_t ts)
        : stockAbbreviation(abbr), quantity(qty), price(p), timestamp(ts) {}
};

class Portfolio {
private:
    map<string, int> holdings;
    double balance = 10000.0;
    vector<Transaction> transactionHistory;
public:
    void deposit(double amount)
    {
        balance += amount;
        cout << "Deposited $" << fixed << setprecision(2) << amount << endl;
    }

    void withdraw(double amount)
    {
        if (amount <= balance)
        {
            balance -= amount;
            cout << "Withdrew $" << fixed << setprecision(2) << amount << endl;
        } else
        {
            cout << "Insufficient balance for withdrawal." << endl;
        }
    }
    void buyStock(Stock* stock, int quantity)
    {
        if (!stock)
        {
            cout << "Stock not found." << endl;
            return;
        }
        double cost = stock->getPrice() * quantity;
        if (cost <= balance)
        {
            holdings[stock->getAbbreviation()] += quantity;
            balance -= cost;
            transactionHistory.emplace_back(stock->getAbbreviation(), quantity, stock->getPrice(), time(0));
            cout << "Bought " << quantity << " shares of " << stock->getAbbreviation() << " at $" << fixed
                << setprecision(2) << stock->getPrice() << endl;
        }
        else
        {
            cout << "Insufficient balance." << endl;
        }
    }

    void sellStock(Stock* stock, int quantity)
    {
        if (!stock)
        {
            cout << "Stock not found." << endl;
            return;
        }
        string abbr = stock->getAbbreviation();
        if (holdings.count(abbr) && holdings[abbr] >= quantity)
        {
            holdings[abbr] -= quantity;
            balance += stock->getPrice() * quantity;
            transactionHistory.emplace_back(stock->getAbbreviation(), -quantity, stock->getPrice(), time(0));
            cout << "Sold " << quantity << " shares of " << abbr << " at $" << fixed
                << setprecision(2) << stock->getPrice() << endl;
        } else
        {
            cout << "Not enough shares to sell." << endl;
        }
    }

    void showPortfolio(StockMarket& market)
    {
        cout << "\n--- Your Portfolio ---\n";
        cout << "Balance: $" << fixed << setprecision(2) << balance << endl;
        double totalValue = balance;
        for (const auto& [abbr, qty] : holdings)
            {
                Stock* stock = market.getStock(abbr);
                if (stock)
                {
                    double currentValue = stock->getPrice() * qty;
                    cout << abbr << ": " << qty << " shares, Current Price: $" << fixed << setprecision(2)
                        << stock->getPrice() << ", Value: $" << fixed << setprecision(2) << currentValue;
                    // Calculate profit/loss (requires tracking buying price) - for simplicity, skipping for now
                    cout << endl;
                    totalValue += currentValue;
                }
            }
        cout << "Total Portfolio Value: $" << fixed << setprecision(2) << totalValue << endl;
    }
    void showTransactionHistory(StockMarket& market)
    {
        cout << "\n--- Transaction History ---\n";
        if (transactionHistory.empty())
        {
            cout << "No transactions yet." << endl;
            return;
        }
        cout << left << setw(15) << "Stock" << setw(10) << "Quantity" << setw(15) << "Price"
            << "Timestamp\n";
        for (const auto& transaction : transactionHistory)
        {
            char sign = transaction.quantity > 0 ? '+' : '-';
            time_t t = transaction.timestamp;
            tm* tm_val = localtime(&t);
            if (tm_val)
            {
                cout << left << setw(15) << transaction.stockAbbreviation << sign << setw(10) << abs(transaction.quantity)
                    << setw(15) << fixed << setprecision(2) << transaction.price
                    << put_time(tm_val, "%Y-%m-%d %H:%M:%S") << endl;
            }
            else
            {
                cout<< "Error converting timestamp." << endl;
            }
        }
    }
};
class Watchlist {
private:
    set<string> watchlist;
public:
    void add(const string& abbr)
    {
        if (watchlist.find(abbr) != watchlist.end())
        {
            cout << "Error: Stock '" << abbr << "' is already in the watchlist." << endl;
        }
        else
        {
            watchlist.insert(abbr);
            cout << abbr << " added to watchlist" << endl;
        }
    }
    void remove(const string& abbr)
    {
        if(watchlist.erase(abbr)==0)
        {
            cout << "Stock not present" << endl;
        }
        else
        {
            watchlist.erase(abbr);
            cout << abbr <<" erased successfully" << endl;
        }
    }
    void view(StockMarket& market) {
        cout << "\n--- Watchlist ---\n";
        if (watchlist.empty()) {
            cout << "Your watchlist is empty." << endl;
            return;
        }
        for (const string& abbr : watchlist) {
            if (auto stock = market.getStock(abbr)) {
                cout << left << setw(10) << abbr << ": $" << fixed << setprecision(2) << stock->getPrice()
                    << endl;
            } else {
                cout << abbr << " (Not found)" << endl;
            }
        }
    }
    const set<string>& getWatchlist() const
    {
        return watchlist;
    }
};
atomic<bool> running{true};
atomic<int> Userchoice{0};
void inputThread()
{
    while (running)
    {
        if (_kbhit())
        {
            char ch = _getch();
            if (isdigit(ch))
            {
                Userchoice = ch - '0'; // convert char to int
                running = false;   // stop live update
            }
        }
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}
class StockExchange
{
private:
    StockMarket market;
    Portfolio portfolio;
    Watchlist watchlist;
    bool showingLivePrices = false;
    thread inputThread;
public:
    StockExchange(const string& namesFile, const string& pricesFile, const string& abbrFile)
        : market()
    {
        market.loadStocks(namesFile, pricesFile, abbrFile);
        market.startPriceUpdates();
        inputThread = thread(inputThreadFunc);
    }
    ~StockExchange()
    {
        market.stop();
        if(inputThread.joinable())
        {
            inputThread.join();
        }
    }
    static void inputThreadFunc()
    {
        while (running)
        {
        if (_kbhit())
        {
            char ch = _getch();
            if (isdigit(ch))
            {
                Userchoice = ch - '0'; // convert char to int
                running = false;   // stop live update
            }
        }
        this_thread::sleep_for(chrono::milliseconds(100));
        }
    }
    void run()
    {
        while (true)
        {
            menu();
        }
    }
private:
    void menu()
    {
        int choice;
        while (true)
        {
            if (!showingLivePrices)
            {
                clearScreen();
                market.showMenu();
            }
            if (showingLivePrices) {
                if (_kbhit()) {
                    _getch(); // Consume the key press
                    showingLivePrices = false; // Exit the live price display
                    continue; // Return to the beginning of the menu loop
                }
                else
                {
                     market.showAllStocksLive();
                     this_thread::sleep_for(chrono::seconds(2));
                }
            }
            else
            {
                cin >> choice;
                if (cin.fail())
                {
                    cout << "Invalid input. Please enter a number" << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max());
                    continue;
                }
                string abbr;
                int qty;
                double amount;
                Stock* stock;
                switch (choice)
                {
                    case 1:
                        showingLivePrices = true;
                        break;
                    case 2:
                        showingLivePrices = false;
                        system("cls");
                        cout << "Enter stock abbreviation to buy: ";
                        cin >> abbr;
                        cout << "Enter quantity: ";
                        cin >> qty;
                        stock = market.getStock(abbr);
                        if (stock)
                            portfolio.buyStock(stock, qty);
                        else
                            cout << "Stock not found." << endl;
                        break;
                    case 3:
                        showingLivePrices = false;
                        system("cls");
                        cout << "Enter stock abbreviation to sell: ";
                        cin >> abbr;
                        cout << "Enter quantity: ";
                        cin >> qty;
                        stock = market.getStock(abbr);
                        if (stock)
                            portfolio.sellStock(stock, qty);
                        else
                            cout << "Stock not found." << endl;
                        break;
                    case 4:
                        system("cls");
                        showingLivePrices = false;
                        portfolio.showPortfolio(market);
                        break;
                    case 5:
                        system("cls");
                        showingLivePrices = false;
                        cout << "Enter stock abbreviation to add to watchlist: ";
                        cin >> abbr;
                        watchlist.add(abbr);
                        break;
                    case 6:
                        system("cls");
                        showingLivePrices = false;
                        cout << "Enter stock abbreviation to remove from watchlist: ";
                        cin >> abbr;
                        watchlist.remove(abbr);
                        break;
                    case 7:
                        system("cls");
                        showingLivePrices = false;
                        watchlist.view(market);
                        break;
                    case 8:
                        system("cls");
                        showingLivePrices = false;
                        cout << "Enter stock abbreviation to view details: ";
                        cin >> abbr;
                        market.showStockDetails(abbr);
                        break;
                    case 9:
                        showingLivePrices = false;
                        clearScreen();
                        cout << "Enter amount to deposit: ";
                        cin >> amount;
                        portfolio.deposit(amount);
                        break;
                    case 10:
                        showingLivePrices = false;
                        clearScreen();
                        cout << "Enter amount to withdraw: ";
                        cin >> amount;
                        portfolio.withdraw(amount);
                        break;
                    case 11:
                        system("cls");
                        showingLivePrices = false;
                        portfolio.showTransactionHistory(market);
                        break;
                    case 12:
                        system("cls");
                        showingLivePrices = false;
                        market.stop();
                        if(inputThread.joinable()){
                            inputThread.join();
                        }
                        return;
                    default:
                        cout << "Invalid choice." << endl;
                }
                cin.clear();
            }
        }
    }
};
int main()
{
    srand(time(0));
    StockExchange exchange("stock_names.txt", "stock_prices.txt", "stock_abbr.txt");
    cout << "=====WELCOME TO OUR STOCK MARKET SIMULATOR=====" << endl;
    exchange.run();
    return 0;
}

