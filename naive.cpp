#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <iostream>
#include <fstream>
#include <cassert>

#include <sstream>
#include <typeinfo>

#include <vector>
#include <list>
#include <utility>
#include <functional>
#include <algorithm>
#include <bitset>
#include <time.h>

using namespace std;

#define LINE_BUF_SIZE 512

char *word;
char *tmp;

struct Record
{
    vector<string> cell;
};

struct CSVDATA
{
    list<Record> data;
    char *name;
};

CSVDATA csvData_1;
CSVDATA csvData_2;
vector<int> record;

// // prototype

string StrToBitStr(string str);

// //

void print_csv(CSVDATA csvdata)
{
    for (auto rec = csvdata.data.begin(); rec != csvdata.data.end(); rec++)
    {
        for (auto col = rec->cell.begin(); col != rec->cell.end(); col++)
        {
            string str = *col;
            printf("%s ", str.c_str());
            // cout << str.c_str(); << endl;
        }
    }
}

// insert csv data into struct
CSVDATA insert_structure(CSVDATA csvdata, const char *path)
{
    string word_hash;
    FILE *fp = fopen(path, "r");
    // auto rec  = csvdata.data.begin();
    // auto hrec = csvdata_hash.data.begin();
    char linebuf[LINE_BUF_SIZE];

    while (fgets(linebuf, LINE_BUF_SIZE, fp))
    {
        Record rec;
        char *word = strtok_r(linebuf, ",", &tmp);
        // std::cout << word << std::endl;
        do
        {
            string str = string(word);
            rec.cell.push_back(string(word));
        } while ((word = strtok_r(NULL, ",", &tmp)) != NULL);
        csvdata.data.push_back(rec);
    }
    fclose(fp);
    return csvdata;
}

CSVDATA get_record(const char *path, CSVDATA csvdata)
{
    csvdata = insert_structure(csvdata, path);

    return csvdata;
}

void compare(CSVDATA csvData_1, CSVDATA csvData_2)
{
    int line = 0;
    auto rec1 = csvData_1.data.begin();

    for (; rec1 != csvData_1.data.end();
         rec1++, line++)
    {
        if (line % 100 == 0)
        {
            printf("line: %d\n", line);
        }
        auto rec2 = csvData_2.data.begin();
        for (; rec2 != csvData_2.data.end(); rec2++)
        {
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
    sort(record.begin(), record.end());
    record.erase(unique(record.begin(), record.end()), record.end());
    cout << record.size() << endl;
}

int main(void)
{
    clock_t begin = clock();
    string filepath1_1 = "csv/customer.csv";
    string filepath2_1 = "csv/supplier.csv";
    const char *filepath1 = filepath1_1.c_str();
    const char *filepath2 = filepath2_1.c_str();
    csvData_1 = get_record(filepath1, csvData_1);

    csvData_2 = get_record(filepath2, csvData_2);
    compare(csvData_1, csvData_2);

    clock_t end = clock();
    cout << "Running time: " << (double)(end - begin) / CLOCKS_PER_SEC * 1000 << "ms" << endl;
}
