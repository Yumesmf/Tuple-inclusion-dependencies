// multi-thread
#include "faida.h"
#include <sys/time.h>
void print_csv(CSVDATA csvdata)
{
    for (auto rec = csvdata.data.begin(); rec != csvdata.data.end(); rec++)
    {
        for (auto col = rec->cell.begin(); col != rec->cell.end(); col++)
        {
            string str = *col;
            printf("%s ", str.c_str());
        }
    }
}

// insert csv data into struct
pair<CSVDATA, CSVDATA> insert_structure(CSVDATA csvdata, CSVDATA csvdata_hash, const char *path)
{
    string word_hash;
    FILE *fp = fopen(path, "r");
    char linebuf[LINE_BUF_SIZE];

    while (fgets(linebuf, LINE_BUF_SIZE, fp))
    {
        Record rec;
        Record hrec;
        hash<string> hash_obj;
        char *word = strtok_r(linebuf, ",", &tmp);
        do
        {
            string str = string(word);
            rec.cell.push_back(string(word));
            stringstream hash_data;
            hash_data << hash_obj(word);
            hrec.cell.push_back(string(hash_data.str()));
        } while ((word = strtok_r(NULL, ",", &tmp)) != NULL);
        csvdata.data.push_back(rec);
        csvdata_hash.data.push_back(hrec);
    }
    fclose(fp);
    return make_pair(csvdata, csvdata_hash);
}

pair<CSVDATA, CSVDATA> get_record(const char *path, CSVDATA csvdata, CSVDATA csvdata_hash)
{

    csvdata = insert_structure(csvdata, csvdata_hash, path).first;
    csvdata_hash = insert_structure(csvdata, csvdata_hash, path).second;

    return make_pair(csvdata, csvdata_hash);
}

// binary string, inverted index, generate ind
void HLL(CSVDATA csvData_1_hash, CSVDATA csvData_2_hash)
{
    // hashvalue->binary->first four bits
    for (auto &hrec : csvData_1_hash.data)
    {
        string hll = "";
        for (auto &hcol : hrec.cell)
        {
            hll += StrToBitStr(hcol);
        }
        hash_hll_1.push_back(hll);
    }

    for (auto &hrec : csvData_2_hash.data)
    {
        string hll = "";
        for (auto &hcol : hrec.cell)
        {
            hll += StrToBitStr(hcol);
        }
        hash_hll_2.push_back(hll);
    }
}

// get binary string
string StrToBitStr(string str)
{
    str = str.substr(0, 19);
    long binary;
    binary = atol(str.c_str());

    string r;
    while (binary != 0)
    {
        r += (binary % 2 == 0 ? "0" : "1");
        binary /= 2;
    }
    reverse(begin(r), end(r));

    string s = r.substr(0, 4);

    return s;
}

// Nested Loops Join
void IND_check(int start)
{
    uint dataSize = hash_hll_1.size();
    int szRange = dataSize / NUM_THREADS;
    start *= szRange;
    int line = start;
    auto rec1 = csvData_1_hash.data.begin();
    advance(rec1, start);
    auto h1 = hash_hll_1.begin();
    advance(h1, start);
    auto h1_end = h1;
    advance(h1_end, szRange);
    for (; h1 != h1_end;
         h1++, rec1++, line++)
    {
        if (line % 100 == 0)
            printf("line: %d\n", line);
        auto rec2 = csvData_2_hash.data.begin();
        auto h2 = hash_hll_2.begin();
        for (; h2 != hash_hll_2.end(); h2++, rec2++)
        {
            if (*h1 == *h2)
            { // hash is same, false positive check.
                for (int i = 0; i < rec1->cell.size(); i++)
                {
                    auto c1 = rec1->cell[i];
                    auto c2 = rec2->cell[i];
                    if (c1 != c2)
                        break;
                    else
                    { // all match
                        record.push_back(line);
                    }
                }
            }
        }
    }
    sort(record.begin(), record.end());
    record.erase(unique(record.begin(), record.end()), record.end());
    cout << record.size() << endl;
}

int main(void)
{

    string filepath1_1 = "customer.csv";
    string filepath2_1 = "supplier.csv";
    const char *filepath1 = filepath1_1.c_str();
    const char *filepath2 = filepath2_1.c_str();
    csvData_1 = get_record(filepath1, csvData_1, csvData_1_hash).first;
    csvData_1_hash = get_record(filepath1, csvData_1, csvData_1_hash).second;

    csvData_2 = get_record(filepath2, csvData_2, csvData_2_hash).first;
    csvData_2_hash = get_record(filepath2, csvData_2, csvData_2_hash).second;

    HLL(csvData_1_hash, csvData_2_hash);

    struct timeval begin, end;
    gettimeofday(&begin, NULL);
    std::vector<std::thread> threads;
    for (size_t m = 0; m < NUM_THREADS; m++)
    {
        threads.emplace_back(std::thread(IND_check, m));
    }
    for (auto &thread : threads)
    {
        thread.join();
    }
    gettimeofday(&end, NULL);
    long diff = (end.tv_sec - begin.tv_sec) * 1000 * 1000 + (end.tv_usec - begin.tv_usec);
    long mdiff = diff / 1000;
    double sdiff = (double)mdiff / 1000.0;
    cout << "Running time: " << sdiff << "s" << endl;
}
