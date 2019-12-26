#include<iostream>
#include <fstream>
#include <sstream>
#include <bitset>
#include<math.h>
#include<stdlib.h>
#include<string.h>
#include<vector>
using namespace std;
int count;
//for converting hexadecimal number to binary
string int_to_string(int);
string hex_to_binary(string hex1)
{
    stringstream ss;
    ss << hex << hex1;
    unsigned n;
    ss >> n;
    bitset<32> b(n);
    return b.to_string();
}


int main(int argc,char *argv[])
{
    if(argc != 9)
    {
        cout<<"Incorrect number of arguments";
        return 0;
    }
    string traceFile = "";
    int blockSize = atoi(argv[1]);
    int l1Size = atoi(argv[2]);
    int l1Assoc = atoi(argv[3]);
    int l2Size = atoi(argv[4]);
    int l2Assoc = atoi(argv[5]);
    int rPolicy = atoi(argv[6]);
    int iProperty = atoi(argv[7]);
    traceFile = argv[8];

    string stdin;
    ifstream fin(traceFile);
    int reads = 0,writes = 0,l1readsmiss = 0,l1writemiss = 0,l1writebacks = 0;
    int l2readsmiss = 0,l2writemiss = 0,l2writebacks = 0;
    int l1set = (int)l1Size/(l1Assoc * blockSize);
    int l2set = 0,l2index;
    if(l2Size!=0)
    {
        l2set = (int)l2Size/(l2Assoc * blockSize);
        l2index = (int)log2(l2set);
    }
    int l1array[l1set][l1Assoc];
    int l2array[l2set][l2Assoc];
    int dirty[l1set][l1Assoc];
    int dirty2[l2set][l2Assoc];
    vector<int> l1[l1set];
    vector<int> l2[l2set];

    for(int i=0; i<l1set; i++)
    {
        vector<int> v;
        l1[i] = v;
        for(int j=0; j<l1Assoc; j++){
            l1array[i][j] = 0;
            dirty[i][j] = 0;
        }
    }
    for(int i=0; i<l2set; i++)
    {
        vector<int> v;
        l2[i] = v;
        for(int j=0; j<l2Assoc; j++){
            l2array[i][j] = 0;
            dirty2[i][j] = 0;
        }
    }
    //int l1_earray[l1set][l1Assoc-1];
    int l1index = (int)log2(l1set);
    int l1offset = (int)log2(blockSize);
    int l2offset = (int)log2(blockSize);
    //fin.open(traceFile.c_str());
    cout<<"===== Simulator configuration =====\n";
    cout<<"BLOCKSIZE:\t"<<blockSize;
    cout<<"\nL1_SIZE:\t"<<l1Size;
    cout<<"\nL1_ASSOC:\t"<<l1Assoc;
    cout<<"\nL2_SIZE:\t"<<l2Size;
    cout<<"\nL2_ASSOC:\t"<<l2Assoc;
    if(rPolicy == 0)
        cout<<"\nREPLACEMENT POLICY: LRU";
    if(rPolicy == 1)
        cout<<"\nREPLACEMENT POLICY: FIFO";
    if(rPolicy == 2)
        cout<<"\nREPLACEMENT POLICY: optimal";
    if(iProperty == 0)
        cout<<"\nINCLUSION PROPERTY: non-inclusive";
    if(iProperty == 1)
        cout<<"\nINCLUSION PROPERTY: inclusive";
    cout<<"\ntrace_file:\t"<<traceFile;
    while(getline(fin,stdin))
    {
    	string hex1 = stdin.substr(2,stdin.length()-2);
    	if(stdin.at(0) == 'r')
    		reads++;
    	if(stdin.at(0) == 'w')
    		writes++;
        //count++;
        //fin>>type>>hex1;
        //converting hexadecimal to binary
        string binary = hex_to_binary(hex1);

        // discarding the offset bits
        binary = binary.substr(0,binary.length() - l1offset);

        // getting the index

        string index_binary = binary.substr(binary.length()-l1index,binary.length());
        string index_binary1;
        //converting binary to decimal
        int l1_index = stoi(index_binary,nullptr,2);
        int l2_index = 0;

        //getting a tag

        string tag_binary = binary.substr(0,binary.length()-l1index);
        int l2tag=0;
        if(l2Size!=0){
        	index_binary1 = binary.substr(binary.length()-l2index,binary.length());
        	l2_index = stoi(index_binary1,nullptr,2);
	        string tag_binary1 = binary.substr(0,binary.length()-l2index);
        	l2tag = stoi(tag_binary1,nullptr,2);
	    }
        //converting binary to hexadecimal
        int l1tag = stoi(tag_binary,nullptr,2);
        //cout<<"index"<<dec<<l2_index<<endl;
        //cout<<"l2tag"<<hex<<l2tag<<endl;
        /*if(type == "r")
        {
            reads++;
        }
        if(type == "w")
        {
            writes++;
        }*/

        bool flag=true; bool is_l2_read=false; bool is_l2_write=false;
        int  temp_l2_tag;

        //l1 cache implementation

        for(int j=0; j<l1Assoc; j++)
        {
            if(l1array[l1_index][j] == l1tag)
            {
                bool flag1 = false;
                int i;
                if(!l1[l1_index].empty())
                {
                    for(i = 0;i<l1Assoc-1;i++)
                    {
                        if(l1[l1_index][i] == l1tag)
                        {
                            flag1=true;
                            break;
                        }
                    }
                }
                if(flag1)
                {
                    if(l1Assoc>1) {
                        int temp = l1[l1_index][i];
                        l1[l1_index].erase(l1[l1_index].begin() + i);
                        l1[l1_index].push_back(temp);
                    }
                }
                else
                {
                    if(l1Assoc>1) {
                        if (l1[l1_index].size() == l1Assoc - 1)
                            l1[l1_index].erase(l1[l1_index].begin());
                        l1[l1_index].push_back(l1tag);
                    }
                }
                if(stdin.at(0)=='w')
                    dirty[l1_index][j]=1;
                flag=false;
                break;
            }
        }
        if(flag)
        {
            if(stdin.at(0)=='r')
                l1readsmiss++;
            else
                l1writemiss++;
            is_l2_read=true;
            bool isVac = false;
            for(int j=0; j<l1Assoc; j++)
            {
                if(l1array[l1_index][j] == 0)
                {
                    l1array[l1_index][j] = l1tag;
                    if(l1[l1_index].size()<l1Assoc-1)
                    {
                        l1[l1_index].push_back(l1tag);
                    }
                    else
                    {
                        if(l1Assoc>1) {
                            l1[l1_index].erase(l1[l1_index].begin());
                            l1[l1_index].push_back(l1tag);
                        }
                    }
                    isVac=true;
                    if(stdin.at(0)=='w')
                        dirty[l1_index][j]=1;
                    break;
                }
            }
            if(!isVac)
            {
                bool loop=false;
                for(int j=0; j<l1Assoc; j++)
                {
                    bool partofVec = false;
                    for(int i=0; i<l1Assoc-1; i++)
                    {
                        if(l1array[l1_index][j]==l1[l1_index][i])
                        {
                            partofVec = true;
                        }
                        if(!partofVec)
                        {
                            temp_l2_tag = l1array[l1_index][j];
                            l1array[l1_index][j]=l1tag;
                            l1[l1_index].push_back(l1tag);
                            l1[l1_index].erase(l1[l1_index].begin());
                            loop=true;
                            if(dirty[l1_index][j]==1){
                                l1writebacks++;
                                is_l2_write=true;
                            }
                            if(stdin.at(0)=='w')
                                dirty[l1_index][j]=1;
                            else
                                dirty[l1_index][j]=0;
                            break;
                        }
                    }
                    if(l1Assoc==1) {
                        temp_l2_tag = l1array[l1_index][j];
                        l1array[l1_index][j]=l1tag;
                        loop=true;
                        if(dirty[l1_index][j]==1){
                            l1writebacks++;
                            is_l2_write=true;
                        }
                        if(stdin.at(0)=='w')
                            dirty[l1_index][j]=1;
                        else
                            dirty[l1_index][j]=0;
                        break;
                    }
                    if(loop)
                        break;
                }
            }
        }

        //l2cache write implementation

        if(l2Size !=0 && is_l2_write)
        {

        	//cout<<"inside "<<endl;
        	long long temp1 = temp_l2_tag<< l1index;
        	//cout<<"inside "<<endl;
        	long long temp2 = temp1 | l1_index;
        	//cout<<"inside "<<endl;
        	string temp3 = int_to_string(temp2);
        	//cout<<"inside "<<endl;
        	string temp4 = temp3.substr(temp3.length()-l2index, l2index);
        	//cout<<"inside "<<endl;
        	long long temp_L2_index = stoi(temp4,nullptr,2);
        	//cout<<"inside "<<endl;
        	temp_l2_tag = temp2>>l2index;
        	//cout<<" temp tag : "<<hex<<temp_l2_tag<<" temp_L2_index "<<dec<<temp_L2_index<<endl;

        	
            //cout<<"here";
            //int l2tagwrite = temp_l2_tag | l1index;
            //int l2tag_write = temp_l2_tag << l1index;
            //l2tag_write = l2tag_write + l1_index;
            //cout<<"l2tag:"<<l2tagwrite<<endl;

           /* index_binary=index_binary.substr(index_binary.length()-l1index, index_binary.length());
 
            string abc = to_string(temp_l2_tag);
            //cout<<" string l2_tag: "<<hex<<temp_l2_tag<<endl;
            //cout<<"\nstring abc: "<<hex<<abc<<endl;
            string binary = hex_to_binary(to_string(temp_l2_tag));
            cout<<"hexa"<<hex1<<endl;
            binary = binary.substr(binary.length()-(32-(l1index+l1offset)), binary.length());
			//cout<<"index_binary "<<index_binary<<endl;
			//cout<<"Binary L1 Tag "<<binary<<endl;
            string ext = binary.substr(binary.length()-(l2index-l1index), binary.length());
			//cout<<"ext bits "<<ext<<endl;
            //binary = binary<<index_binary;
            //binary = binary+index_binary;
            int temp_L2_index = stoi((ext+index_binary),nullptr,2);
            temp_l2_tag = stoi(binary.substr(0, binary.length()-(l2index-l1index)), nullptr,2);
            //temp_l2_tag = binary<<l2index;
            //temp_l2_tag = binary+l2index;
            cout<<"l2 tag "<<hex<<l2tag<<" temp tag : "<<hex<<temp_l2_tag<<" temp_L2_index "<<dec<<temp_L2_index<<endl;*/
            bool flag=true;

            for(int j=0; j<l2Assoc; j++)
            {

                if(l2array[temp_L2_index][j] == temp_l2_tag)
                {
                    bool flag1 = false;
                    int i;
                    if(!l2[temp_L2_index].empty())
                    {
                        for(i = 0;i<l2Assoc-1;i++)
                        {
                            if(l2[temp_L2_index][i] == temp_l2_tag)
                            {
                                flag1=true;
                                break;
                            }
                        }
                    }
                    if(flag1)
                    {
                        int temp = l2[temp_L2_index][i];
                        //cout<<i<<endl;
                        l2[temp_L2_index].erase(l2[temp_L2_index].begin()+i);
                        l2[temp_L2_index].push_back(temp);
                    }else{
                        if(l2[temp_L2_index].size()==l2Assoc-1)
                            l2[temp_L2_index].erase(l2[temp_L2_index].begin());
                        l2[temp_L2_index].push_back(temp_l2_tag);
                    }
                    dirty2[temp_L2_index][j]=1;
                    flag=false;
                    break;
                }
            }
            //cout<<"Miss "<<endl;
            if(flag)
            {
                l2writemiss++;
                bool isVac = false;
                for(int j=0; j<l2Assoc; j++)
                {
                    if(l2array[temp_L2_index][j] == 0)
                    {
                        l2array[temp_L2_index][j] = temp_l2_tag;
                        if(l2[temp_L2_index].size()<l2Assoc-1)
                        {
                            l2[temp_L2_index].push_back(temp_l2_tag);
                        }else
                        {
                            l2[temp_L2_index].erase(l2[temp_L2_index].begin());
                            l2[temp_L2_index].push_back(temp_l2_tag);
                        }
                        isVac=true;
                        break;
                    }
                }
                //cout<<"Not new "<<endl;
                if(!isVac)
                {
                    bool loop=false;
                    //cout<<"Eviction "<<endl;
                    for(int j=0; j<l2Assoc; j++)
                    {
                        bool partofVec = false;
                        for(int i=0; i<l2Assoc-1; i++)
                        {                        	
                            if(l2array[temp_L2_index][j]==l2[temp_L2_index][i])
                            {
                                partofVec = true;
                                break;
                            }
                        }                        
                        if(!partofVec)
                        {
                            l2array[temp_L2_index][j]=temp_l2_tag;
                            l2[temp_L2_index].push_back(temp_l2_tag);
                            l2[temp_L2_index].erase(l2[temp_L2_index].begin());
                            loop=true;
                            if(dirty2[temp_L2_index][j]==1)
                                l2writebacks++;
                            dirty2[temp_L2_index][j]=1;
                            break;
                        }
                        if(loop)
                            break;
                    }
                }
            }
            
        }
        //l2 cache read implementation
        //cout<<"Read "<<endl;
        if(l2Size != 0 && is_l2_read)
        {
        	
        	//cout<<"tag "<<hex<<l2tag<<" index "<<dec<<l2_index<<endl;
            bool flag=true;
            
            //cout<<"Is Miss "<<endl;
            for(int j=0; j<l2Assoc; j++)
            {

                if(l2array[l2_index][j] == l2tag)
                {
                    bool flag1 = false;
                    int i;
                    //l1array[l1_index][j] = l1tag;
                    if(!l2[l2_index].empty())
                    {
                        for(i = 0;i<l2Assoc-1;i++)
                        {
                            if(l2[l2_index][i] == l2tag)
                            {
                                flag1=true;
                                break;
                            }
                        }
                    }
                    if(flag1)
                    {
                        int temp = l2[l2_index][i];
                        //cout<<i<<endl;
                        l2[l2_index].erase(l2[l2_index].begin()+i);
                        l2[l2_index].push_back(temp);
                    }else{
                        if(l2[l2_index].size()==l2Assoc-1)
                            l2[l2_index].erase(l2[l2_index].begin());
                        l2[l2_index].push_back(l2tag);
                    }
                    flag=false;
                    break;
                }
            }
            //cout<<"Miss "<<endl;
            if(flag)
            {
                l2readsmiss++;
                bool isVac = false;
                for(int j=0; j<l2Assoc; j++)
                {
                    if(l2array[l2_index][j] == 0)
                    {
                    	
                        l2array[l2_index][j] = l2tag;
                        if(l2[l2_index].size()<l2Assoc-1)
                        {
                            l2[l2_index].push_back(l2tag);
                        }else
                        {
                            l2[l2_index].erase(l2[l2_index].begin());
                            l2[l2_index].push_back(l2tag);
                        }
                        isVac=true;
                        break;
                    }
                }
                for(int j=0; j<l2Assoc; j++)
                {
                	//cout<<l2array[l2_index][j]<<" ";
                }
                //cout<<endl;
                for(int i=0; i<l2[l2_index].size(); i++)
                {
                	//cout<<l2[l2_index][i]<<" ";
                }
                //cout<<endl;
                //cout<<"Not new "<<endl;
                if(!isVac)
                {
                    bool loop=false; int inx=0; 
                    for(int j=0; j<l2Assoc; j++)
                    {
                    	bool partofVec = false;
                        for(int i=0; i<l2Assoc-1; i++)
                        {
                        	if(l2array[l2_index][j]==l2[l2_index][i])
                            {
                                partofVec = true;
                                break;
                            }
                        }                        
                        if(!partofVec)
                        {
                            l2array[l2_index][j]=l2tag;
                            l2[l2_index].push_back(l2tag);
                            l2[l2_index].erase(l2[l2_index].begin());
                            loop=true;
                            if(dirty2[l2_index][j]==1)
                                l2writebacks++;
                            dirty2[l2_index][j]=0;
                            break;
                        }
                        if(loop)
                            break;
                    }
                }
            }
            
        }
    }

    cout<<"\n===== L1 contents =====\n";
    for(int i = 0;i<l1set;i++)
    {
        cout<<"Set\t"<<dec<<i<<":\t";
        for(int j = 0;j<l1Assoc;j++)
        {
            if(dirty[i][j]==1)
                cout<<hex<<l1array[i][j]<<" "<<"D"<<" ";
            else
                cout<<hex<<l1array[i][j]<<" "<<" "<<" ";
        }
        cout<<"\n";
    }
    if(l2Size != 0)
        cout<<"===== L2 contents =====\n";
    for(int i = 0;i<l2set;i++)
    {
        cout<<"Set\t"<<dec<<i<<":\t";
        //int j =0;
        for(int j = 0;j<l2Assoc;j++)
        {
            if(dirty2[i][j]==1)
                cout<<hex<<l2array[i][j]<<" "<<"D"<<" ";
            else
                cout<<hex<<l2array[i][j]<<" "<<" "<<" ";
        }
        cout<<"\n";
    }
    double l1missrate = (double)(l1readsmiss + l1writemiss)/(reads + writes);
    double l2missrate = 0;
    int l2reads = 0;
    int l2writes = 0;
    if(l2Size != 0)
    {
        l2reads = l1readsmiss + l1writemiss;
        l2writes = l1writebacks;
        l2missrate = (double)l2readsmiss/l2reads;
    }
    int l1memorytraffic =0;
    if(l2Size != 0)
    {
    	l1memorytraffic = l2readsmiss + l2writemiss + l2writebacks;
    }
    else
    	l1memorytraffic = l1readsmiss + l1writemiss + l1writebacks;
    cout<<"\n===== Simulation results (raw) =====\n";
    cout<<"a. number of L1 reads: "<<dec<<reads;
    cout<<"\nb. number of L1 read misses: "<<dec<<l1readsmiss;
    cout<<"\nc. number of L1 writes: "<<dec<<writes;
    cout<<"\nd. number of L1 write misses: "<<dec<<l1writemiss;
    cout<<"\ne. L1 miss rate: "<<dec<<l1missrate;
    cout<<"\nf. number of L1 writebacks: "<<dec<<l1writebacks;
    cout<<"\ng. number of L2 reads: "<<dec<<l2reads;
    cout<<"\nh. number of L2 read misses: "<<dec<<l2readsmiss;
    cout<<"\ni. number of L2 writes: "<<dec<<l2writes;
    cout<<"\nj. number of L2 write misses: "<<dec<<l2writemiss;
    cout<<"\nk. L2 miss rate: "<<dec<<l2missrate;
    cout<<"\nl. number of L2 writebacks: "<<dec<<l2writebacks;
    cout<<"\nm. total memory traffic: "<<dec<<l1memorytraffic<<endl;

}

string int_to_string(int a)
{
    string tag1  ("");
    int temp = 1;
    for(int i = 0; i < 31; i++)
    {
        if((temp&a) >= 1)
            tag1 = "1"+tag1;
        else
            tag1 = "0"+tag1;
        temp<<=1;
    }
    return tag1;    
}
