#include<iostream>
#include <fstream>
#include <sstream>
#include <bitset>
#include<math.h>
#include<string.h>
 
using namespace std;
string global = "";
int mis_prediction = 1;
int m,n,k,m1;
int count = 0;

//for converting hexadecimal number to binary

string hex_to_binary(string hex1)
{
    stringstream ss;
    ss << hex << hex1;
    unsigned n;
    ss >> n;
    bitset<32> b(n);
    return b.to_string();
}

// for bimodal branch predictor

int bimodal(int m,string file)
{
    string stdin;
    ifstream fin;
    fin.open(file,ifstream::in);
    string hex,type;
    int size = (int)pow(2,m);
    int prediction_table[size];
    for(int i = 0;i<size;i++)
        prediction_table[i] = 4;
    while(getline(fin,stdin))
    {
        count ++;
        fin>>hex>>type;
        string binary = hex_to_binary(hex);
        binary = binary.substr(0,binary.length()-2);
        binary = binary.substr(binary.length()-m,binary.length());
        int index = stoi(binary, nullptr, 2);
        if(type == "t")
        {
            if(prediction_table[index]<4)
                mis_prediction++;
            if(prediction_table[index]!=7)
                prediction_table[index] = prediction_table[index] + 1;            
        }
        else
        {
            if(prediction_table[index]>=4)
                mis_prediction++;
            if(prediction_table[index]!=0)
                prediction_table[index] = prediction_table[index] - 1;        
        }
    }
    cout<<"number of predictions : "<<count<<endl;
    cout<<"number of mispredictions : "<<mis_prediction<<endl;
    cout<<"misprediction rate : "<<((float)((mis_prediction*100.0)/count))<<endl;
    cout<<"Final bimodal contents"<<endl;
    for(int i = 0;i<size;i++)
        cout<< i << "  "<<prediction_table[i]<<endl;
    fin.close();
}

//for gshare branch predictor

int gshare(int m,int n,string file)
{
    string stdin;
    ifstream fin;
    fin.open(file,ifstream::in);
    string hex,type;
    int size = (int)pow(2,m);
    int prediction_table[size];

    
    for(int i = 0;i<size;i++)
        prediction_table[i] = 4;
    for(int i = 0;i<n;i++)
        global.append("0");
    while(getline(fin,stdin))
    {
        count ++;
        fin>>hex>>type;
        string binary = hex_to_binary(hex);
        binary = binary.substr(0,binary.length()-2);
        binary = binary.substr(binary.length()-m,binary.length());
        int index = stoi(binary,nullptr,2);
        int temp =  stoi(global,nullptr,2);
        index = index ^ temp;
        if(type == "t")
        {
            global = "1" + global.substr(0,n-1);
            if(prediction_table[index]<4)
                mis_prediction++;
            if(prediction_table[index]!=7)
                prediction_table[index] = prediction_table[index] + 1;
        }
        else
        {
            global = "0" + global.substr(0,n-1);
            if(prediction_table[index]>=4)
                mis_prediction++;
            if(prediction_table[index]!=0)
                prediction_table[index] = prediction_table[index] - 1;
        }
    
    }
    cout<<"number of predictions : "<<count<<endl;
    cout<<"number of mispredictions : "<<mis_prediction<<endl;
    cout<<"misprediction rate : "<<((float)((mis_prediction*100.0)/count))<<endl;
    cout<<"Final gshare contents"<<endl;
    for(int i = 0;i<size;i++)
        cout<< i << "  "<<prediction_table[i]<<endl;
    fin.close();

}

//for hybrid branch predictor

int hybrid(int k,int m1,int n,int m,string file)
{
    ifstream fin;
    fin.open(file,ifstream::in);
    string hex,type;
    int size = (int)pow(2,m);
    int prediction_table[size];
    int size1 = (int) pow (2, m1);
    int prediction_table1[size1];
    int size2 = (int) pow (2, k);
    int prediction_table2[size2];
    string stdin;
        for(int i=0; i<n; i++)
            global.append("0");
        int index1 = 0;
        int index2 = 0;

        //initializing prediction table

        for (int i = 0; i < size; i++)
            prediction_table[i] = 4;
        
        for (int i = 0; i < size1; i++)
            prediction_table1[i] = 4;
        
        for (int i = 0; i < size2; i++)
            prediction_table2[i] = 1;
        int d=0; int e=0;
        while (getline(fin, stdin)) 
        {
            count++;
            d=1; e=1;
            fin>>hex>>type;
            string binary = hex_to_binary(hex);
            binary = binary.substr(0, binary.length() - 2);
            string bstring = binary.substr(binary.length() - m, binary.length());
            int index = stoi(bstring, nullptr, 2);
            string gstring = binary.substr(binary.length() - m1, binary.length());
            int temp = stoi(global, nullptr, 2);
            index1 = stoi(gstring, nullptr, 2);
            index1 = index1 ^ temp;
            string hstring = binary.substr(binary.length() - k, binary.length());
            index2 = stoi(hstring, nullptr, 2);
            if (type == "t")
            {
                if (prediction_table[index] < 4)
                    d = 0;
                if (prediction_table1[index1] < 4)
                    e = 0;
                if (prediction_table[index] != 7 && prediction_table2[index2]<2) 
                    prediction_table[index] = prediction_table[index] + 1;
                if (prediction_table1[index1] != 7 && prediction_table2[index2]>=2) 
                    prediction_table1[index1] = prediction_table1[index1] + 1;
                if((d==0 && prediction_table2[index2]<2) || (e==0 && prediction_table2[index2]>=2))
                      mis_prediction++;
            }
            else
            {
                if (prediction_table[index] >= 4)
                    d=0;
                if (prediction_table1[index1] >= 4)
                    e=0;
                if (prediction_table[index] != 0 && prediction_table2[index2]<2)
                    prediction_table[index] = prediction_table[index] - 1;
                if (prediction_table1[index1] != 0 && prediction_table2[index2]>=2)
                    prediction_table1[index1] = prediction_table1[index1] - 1;
                if((d==0 && prediction_table2[index2]<2) || (e==0 && prediction_table2[index2]>=2))
                      mis_prediction++;
            } 
            if(d!=e)
            {
                if(d==1)
                {
                    if(prediction_table2[index2]!=0)
                        prediction_table2[index2] = prediction_table2[index2]-1;
                }
                else
                {
                    if(prediction_table2[index2]!=3)
                        prediction_table2[index2] = prediction_table2[index2]+1;
                }
            }
            if (type == "t")
                global = "1" + global.substr(0, n - 1);     
            else
                global = "0" + global.substr(0, n - 1);
                
        }
        cout<<"number of predictions : "<<count<<endl;
        cout<<"number of mispredictions : "<<mis_prediction<<endl;
        cout<<"misprediction rate : "<<((float)((mis_prediction*100.0)/count))<<endl;
        cout<<"Final chooser contents"<<endl;
        for (int i = 0; i < size2; i++)
            cout<<i<<"  "<<prediction_table2[i]<<endl;
        cout<<"Final gshare contents"<<endl;
        for (int i = 0; i < size1; i++)
            cout<<i<<"  "<<prediction_table1[i]<<endl;
        cout<<"Final bimodal contents"<<endl;
        for (int i = 0; i < size; i++)
            cout<<i<<"  "<<prediction_table[i]<<endl;
    fin.close ();
}
    

// main function used for calling

int main(int argc,char *argv[])
 {
     string file = "";
     if(argc == 4)
     {
         m = atoi(argv[2]);
         file = argv[3];
         bimodal(m,file);
     }
     if(argc == 5)
     {
         m = atoi(argv[2]);
         n = atoi(argv[3]);
         file = argv[4];
         gshare(m,n,file);
     }
     if(argc == 7)
     {
         k = atoi(argv[2]);
         m1 = atoi(argv[3]);
         n = atoi(argv[4]);
         m = atoi(argv[5]);
         file = argv[6];
         hybrid(k,m1,n,m,file);
     }

 }

	
	
	
