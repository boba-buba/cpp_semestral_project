#include "GumboParser.h"
#include "DataRetrieval.h"
#include "ECalculator.h"
#include "data.h"


static bool CheckNumber(std::string& n, int maxNumber)
{
    for (std::size_t i = 0; i < n.size(); ++i)
    {
        if (!isdigit(static_cast<unsigned char>(n[i]))) return false;  //
    }
    int nInt = std::stoi(n);
    if (nInt < 1 || nInt > maxNumber) return false;
   return true;
}


static void OptionsMenu()
{ 
    std::cout << "\nMenu of options\n";
    std::cout << "1. Add new comapny\n";
    std::cout << "2. Calculate MA and EMA\n";
    std::cout << "3. Calculate Volatility\n";
    std::cout << "4. Compare two companies with correlation\n";
    std::cout << "5. Show Companies in Memory\n";
    std::cout << "6. Calculate Abnormal Returns before and after EPS was announced\n";
    std::cout << "7. Show Company Data\n";
    std::cout << "8. Remove Company from Memory\n";
    std::cout << "9. Exit\n";
}

static void Options(std::size_t& num)
{
    std::string temp;
    OptionsMenu();
    std::cin >> temp;
    while (!CheckNumber(temp, 9))
    {
        std::cout << "Wrong symbol entered, choose from the Menu of options\n";
        std::cin >> temp;
    }
    num = std::stoi(temp);
    
}

static void NameToUpper(std::string& s)
{
    std::string aa = "";
    for (std::size_t i = 0; i < s.size(); ++i)
        aa += toupper(s[i]);
    s = std::move(aa);
}

int main()
{
    try 
    {
        AllStocks stocks;
        std::size_t option = 0;
        std::string name;
        Options(option);
switchStart:
        switch (option)
        {
        case 1:
            {
                std::cout << "Enter symbol of the Company\n";
                std::cin >> name;
                NameToUpper(name);
                if (!stocks.StockInMem(name))
                {
                    try
                    {
                        stocks.addNewStock(name);
                        std::cout << "Stocks of the Company were successfully added\n";
                    }
                    catch (const std::exception& e)
                    {
                        std::cerr << e.what() << std::endl;
                    }
                }
                else
                {
                    std::cout << "Company data already in the Memory,\nif You want to add new data, you must remove\nCompany data from the Memory first with command 8.\nAnd then add data again\n";
                }   
                Options(option);
                goto switchStart;
            }
        case 2:
            {
                std::string periods;
                std::cout << "Enter symbol of the Company\n";
                std::cin >> name; NameToUpper(name);
                try
                {
                    auto stock = stocks.GetStock(name);
                    const vd& prices = (*(stock->second)).GetTimeSeries();
                    
                    std::size_t maxPeriods = prices.size();
                    std::cout << "Enter number of periods, it must be less than\nnumber of prices stored in memory, which is " << maxPeriods << std::endl;

                    std::cin >> periods; 
                    while (!CheckNumber(periods, maxPeriods-1))
                    {
                        std::cout << "number, which is less than "<< maxPeriods << " must be entered\n";
                        std::cin >> periods;
                    }
                    
                    if (maxPeriods <= std::stoi(periods))
                    {
                        throw std::runtime_error("number of periods must be less than number of prices above");
                    }
                        
                    MACalculator calculator(std::stoi(periods), prices);
                    vd ma = calculator.calculateMA();
                    vd ema = calculator.calculateEMA();
                    std::cout << "MA        EMA\n";
                    for (std::size_t i = 0; i < ma.size(); ++i)
                    {
                        std::cout << ma[i] << "   " << ema[i] << std::endl;
                    }
                }
                catch (std::exception& e)
                {
                    std::cout << e.what() << std::endl;
                } 
                Options(option);
                goto switchStart;
            }
        case 3:
            { 
                try {
                    std::cout << "Enter symbol of the Company\n";
                    std::cin >> name; NameToUpper(name);
                    auto stock = stocks.GetStock(name);
                    const vd& prices = (*(stock->second)).GetTimeSeries();
                    VolatilityCalculator calc(prices);
                    double first = calc.rangeVolatility();
                    double second = calc.avgDailyRange();
                    double third = calc.stdDev(prices);
                    std::cout << "Range volatility: " << first << "\n";
                    std::cout << "Average range for given time period: " << second << "\n";
                    std::cout << "Standard Deviation: " << third;
                    std::cout <<std::endl;
                    std::cin ;
                }
                catch (std::exception& e)
                {
                    std::cout << e.what() << std::endl;
                }
                Options(option);
                goto switchStart;
            }
        case 4:
            {
                try {
                    std::cout << "Enter symbol of the first Company to compare\n";
                    std::cin >> name; NameToUpper(name);
                    auto stock1 = stocks.GetStock(name);
                    std::cout << "Enter symbol of the second Company to compare\n";
                    std::cin >> name; NameToUpper(name);
                    auto stock2 = stocks.GetStock(name);
                    const vd& prices1 = (*(stock1->second)).GetTimeSeries();
                    const vd& prices2 = (*(stock2->second)).GetTimeSeries();
                    CorrelationCalculator cCalc(prices1, prices2);
                    auto correlation = cCalc.correlation();
                    std::cout << "Correlation of the " << stock1->first<< " and " << stock2->first << " is " <<  correlation << std::endl;    
                }
                catch (std::exception& e)
                {
                    std::cout << e.what() << std::endl;
                }
                Options(option);
                goto switchStart;
            }
        case 5:
            {
                std::cout << "Companies with data saved in Memory\n";
                stocks.ShowCompanies();
                Options(option);
                goto switchStart;
            }
        case 6:
            {
                try {
                    std::cout << "Enter symbol of the Company\n";
                    std::cin >> name; NameToUpper(name);
                    auto stock = stocks.GetStock(name);
                    std::string spyTicker = "SPY";
                    auto spy = stocks.GetStock(spyTicker);
                    const vd& spyReturns = (*spy->second).GetReturns();
                    (*(stock->second)).CalculateAbnormalReturns(spyReturns);
                    std::cout << "Abnormal Returns were successfully calculated, EPS Release Date was " << (*(stock->second)).getEPSDate() << std::endl;
                }
                catch (std::exception& e)
                {
                    std::cout << e.what() << std::endl;
                }
                Options(option);
                goto switchStart;
            }
        case 7:
            {
                try {
                    std::cout << "Enter symbol of the Company\n";
                    std::cin >> name; NameToUpper(name);
                    auto stock = stocks.GetStock(name);
                    (*(stock->second)).ShowData();
                }
                catch (std::exception& e)
                {
                    std::cout << e.what() << std::endl;
                }
                Options(option);
                goto switchStart;
            }
        case 8:
            {
                std::cout << "Enter symbol of the Company\n";
                std::cin >> name; NameToUpper(name);
                int del = stocks.RemoveCompany(name);
                (del == 1) ? std::cout << "Company data were successfully deleted from Memory\n" :
                    std::cout <<"Company Data were not in the Memory\n";
                Options(option);
                goto switchStart;
            }
        case 9:
            {
                option = 8;
                break;
            }
        }
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
        return 0;
}
