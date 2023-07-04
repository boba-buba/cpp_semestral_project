#include "DataRetrieval.h"
#include "Utilities.h"
#include "data.h"


static size_t
WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct* mem = (struct MemoryStruct*)userp;

    char* ptr = (char*)realloc(mem->memory, mem->size + realsize + 1);
    if (!ptr) {
        //printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

void Data::ExtractHTMLString(const char*& url)
{
    CURL* curl_handle;
    CURLcode res;

    this->chunk.memory = (char*)malloc(1);  // will be grown as needed by the realloc above 
    this->chunk.size = 0;    // no data at this point 

    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)&chunk);

    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    res = curl_easy_perform(curl_handle);

    if (res != CURLE_OK) {
        throw std::runtime_error( "Check your Internet Connection\n");
    }
    
    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();
}

int Data::GetHistoricalData()
{
    Parser::GetDataFromNodeTable(this->chunk.memory, this->ClosePrices, this->Dates);
    free(this->chunk.memory); this->chunk.size = 0;
    for (std::size_t i = 0; i < this->Dates.size(); ++i)
    {
        this->Dates[i] = Time::ToStandardDateFormat(this->Dates[i]);
    }
    return 0;
}

void Stock::GetEPSData()
{
    std::string currentDate = Dates[0];
    auto it = eps_.rbegin(); 

    std::string ReleasedEPSDate = (*it)[0];
    if (ReleasedEPSDate.size() < 10) ReleasedEPSDate = '0' + ReleasedEPSDate;
    this->ReleaseDate = ReleasedEPSDate;
    this->pos = std::find(Dates.begin(), Dates.end(), ReleasedEPSDate) - Dates.begin();
    this->actualEPS = std::stod((*it)[2]);
    this->estimateEPS =  std::stod((*it)[1]);
}

void Stock::CalculateAbnormalReturns(const std::vector<double>& spyReturns)
{
    if (pos == ClosePrices.size()) throw std::runtime_error("During this date stock prices were unknown.\nAbnormal Returns can not be calculated.\n");
    if (pos == 0) throw std::runtime_error("Actual EPS was released today.\nAbnormal Returns can not be calculated.\n");
    std::vector<std::size_t> intervals {30, pos, Returns.size() - pos-1, spyReturns.size() - pos - 1 };
    this->interval = *(std::min_element(intervals.begin(), intervals.end()));
    std::size_t priceCount = (interval) * 2;
    for (std::size_t i = 0; i < priceCount; ++i)
    {
        this->AbnormalReturns.push_back(this->Returns[pos - interval+i] - spyReturns[pos - interval + i]);
    }
}

int Stock::GetData()
{
    Data::ExtractHTMLString(this->urlHistory_);
    int DataToStock = this->GetHistoricalData();
    if (DataToStock != 0) { return -1; }
    CalculateReturns();
    Data::ExtractHTMLString(this->urlAnalysis_);
    this->eps_ = std::move(Parser::GetEPSTable(this->chunk.memory));
    free(this->chunk.memory); this->chunk.size = 0;
    GetEPSData();
    return 0;
}

int Standard::GetData()
{    
    Data::ExtractHTMLString(this->urlHistory_);
    int DataToStock = this->GetHistoricalData();
    if (DataToStock != 0) { return -1; }
    CalculateReturns();
    return 0;
}

void Stock::CalculateReturns()
{
    for (std::size_t i = 1; i < ClosePrices.size(); i++)
    {
        Returns.push_back((ClosePrices[i] - ClosePrices[i - 1]) / ClosePrices[i - 1]);
    }
}

void Standard::CalculateReturns()
{
    for (std::size_t i = 1; i < ClosePrices.size(); i++)
    {
        ReturnsStandard.push_back((ClosePrices[i] - ClosePrices[i - 1]) / ClosePrices[i - 1]);
    }
}

void Stock::ShowData()
{
    std::cout << "Estimated EPS " << this->estimateEPS << std::endl;
    std::cout << "Actual EPS " << this->actualEPS << std::endl;
    std::cout << "Date of Release " << this->ReleaseDate << std::endl;
    std::cout << "Date\t      Close Price\n";
    for (std::size_t i = 0; i < this->Dates.size(); ++i)
    {
        std::cout << this->Dates[i] << "\t" << this->ClosePrices[i] << std::endl;
    }
    if (AbnormalReturns.size() == 0) std::cout << "Abnormal Returns were not calculated\n";
    else
    {
        std::cout << "Abnormal Returns of the stock\n";
        std::cout << "Date\t      Abnormal Return\t      Return\n";
        for (std::size_t i = 0; i < AbnormalReturns.size(); ++i)
        {
            std::size_t j = pos - interval + i;
            std::cout << Dates[j] << "\t" << AbnormalReturns[i] << "\t" << Returns[j] << std::endl;
        }
    }

}
