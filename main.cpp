#include <iostream>
#include <bits/stdc++.h>

using namespace std;

string fileName = "mempool.csv";
//max weight according to question
int blockWeight = 4000000;
//tx_id to be included in the answer
set<string> included_tx_id;

class Transaction
{
public:
    string tx_id;
    int fee;
    int weight;
    vector<string> parents;
};

pair<string, Transaction *> createTransaction(vector<string> &row)
{
    auto ans = new Transaction();
    ans->tx_id = row[0];
    ans->fee = stoi(row[1]);
    ans->weight = stoi(row[2]);
    vector<string> p;
    for (int i = 3; i < row.size(); i++)
    {
        p.push_back(row[i]);
    }
    ans->parents = p;
    return {row[0], ans};
}

//reading data from .csv file
void readCSV(string fileName, unordered_map<string, Transaction *> &umap)
{
    ifstream fin(fileName);
    vector<string> row;
    string temp, line, word;
    //skipping the header of csv
    getline(fin, line);
    while (getline(fin, line))
    {
        row.clear();
        stringstream s(line);
        while (getline(s, word, ','))
        {
            //adding tx_details to row
            row.push_back(word);
        }
        pair<string, Transaction *> p = createTransaction(row);
        umap[p.first] = p.second;
    }
    fin.close();
    cout << "num of transactions read : " << umap.size() << endl;
}

bool isValidTx(Transaction *tx, set<string> &included_tx_set)
{
    //if all the parents of tx are present in included_tx_set it is valid
    for (auto parent : tx->parents)
    {
        if (included_tx_set.find(parent) == included_tx_set.end())
            return false;
    }
    return true;
}

//saving final in block in txt format
void writeOutput(vector<string> &included_tx_vector, string fn)
{
    ofstream myfile(fn);
    for (auto s : included_tx_vector)
        myfile << s << endl;
    myfile.close();
}

int main()
{
    //map tx_id to its pointer
    unordered_map<string, Transaction *> umap;
    readCSV(fileName, umap);

    //maintaining order of highest fee/weight
    set<pair<float, Transaction *>, greater<pair<float, Transaction *>>> tx_set;

    //contains all the tx which are included in block
    set<string> included_tx_set;

    //need this for maintaining order of output
    vector<string> included_tx_vector;

    for (auto p : umap)
    {
        tx_set.insert({(float)p.second->fee / (float)p.second->weight, p.second});
    }

    int currBlockWeight = 0;
    int totalFee = 0;
    while (!tx_set.empty() && currBlockWeight < blockWeight)
    {
        bool found = false;
        for (auto itr = tx_set.begin(); itr != tx_set.end(); itr++)
        {
            Transaction *curr_tx = (*itr).second;
            int currFee = curr_tx->fee;
            int currWeight = curr_tx->weight;
            if (isValidTx(curr_tx, included_tx_set) && currBlockWeight + currWeight <= blockWeight)
            {
                currBlockWeight += currWeight;
                included_tx_set.insert(curr_tx->tx_id);
                included_tx_vector.push_back(curr_tx->tx_id);
                totalFee += currFee;
                tx_set.erase(itr);
                found = true;
                break;
            }
        }
        //when no vali tx is found => add nothing
        if (!found)
            break;
    }

    cout << "num of tx in final block " << included_tx_set.size() << endl;
    cout << "total fee in curr block : " << totalFee << " total weight : " << currBlockWeight << endl;

    writeOutput(included_tx_vector, "block.txt");
}