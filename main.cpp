#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <ctime>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <termios.h>
#include <chrono>
#define tNum 5      //OR use a loop to count the empty lines
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define RESET "\033[0m"
#define CYAN "\033[36m"
using namespace std;
void read(vector<pair<string, vector<int>>> &text)
{
    srand(time(NULL));
    int num=rand()%tNum, count=0;
    //cout<<num;
    ifstream in("text.in");
    while(!in.eof())
    {
        string line;
        getline(in,line);
        //stringstream is(line);
        if(line.empty())
        {
            count++;
        }
        else if(count==num)
        {
            text.push_back(make_pair(line+"\n", vector<int>(line.size()+1,0)));//.push_back(std::make_pair("apple vagy line+"\n"", std::vector<int>(n, 0)));
        }
        if(count>num)
        {
            return;
        }
    }
    in.close();
}
int getsize(const vector<pair<string, vector<int>>> &text)              // used to calculate the WPM number
{
    int nr=0;
    for(int i=0;i<text.size();i++)
    {
        nr+=text[i].first.size()-1;
    }
    return nr;
}
void display(const vector<pair<string, vector<int>>> &text, vector<char> err)
{
    int errCounter=0;
    for(int i=0;i<text.size();i++)
    {
        for(int j=0;j<text[i].first.size();j++)
        {
            if(text[i].second[j]==0)
            {
                cout<<text[i].first[j];
            }
            else if(text[i].second[j]<0)                            // incorrect char
            {
                cout<<RED<<err[errCounter]<<RESET;
                errCounter++;
            }
            else if(text[i].second[j]>0)                            // correct char
            {
                cout<<GREEN<<text[i].first[j]<<RESET;
            }
        }
    }
}
bool typeCheck(const char &text, const char &c)         // check if the lastly pushed char is correct or not
{
    if(text==c){
        return true;
    }
    else {
        return false;
    }
}
char getch() {                      // read char from keyboard without pushing enter
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    return buf;
}
void score(double time)         // final output
{
    cout<<CYAN<<"Your typing speed is: ";
    if(time<20) {
        cout<<RED;
    } else if(time>=20 && time<=40) {
        cout<<YELLOW;
    } else {
        cout<<GREEN;
    }
    cout<<time<<RESET<<endl;
}
void typing(vector<pair<string, vector<int>>> &text, vector<char> err)
{
    auto start=chrono::high_resolution_clock::now();
    int line=0, poz=0, pozCpy;
    bool Correct = false;
    while(true)
    {
        system("clear");            //clear screen
        display(text, err);
        char c;
        c=getch();
        if(c=='\n' && line<text.size()-1)//text[line].first[poz]=='\n')
        {
            line++;
            pozCpy=poz;
            poz=0;
            continue;
        }
        else if(c=='\n' && line==text.size()-1)
        {
            c='\0';
            continue;
        }
        if (c == 8 || c == 127) {
            if (poz > 0) {
                poz--;
                if (text[line].second[poz] < 0 && !err.empty()) {
                    err.pop_back();  // Remove the last incorrect character
                }
                text[line].second[poz] = 0;  // Reset the character state
            } else if (line > 0) {
                line--;
                poz = pozCpy;
                if (text[line].second[poz] < 0 && !err.empty()) {
                    err.pop_back();  // Remove the last incorrect character
                }
                text[line].second[poz] = 0;  // Reset the character state
            }
            continue;
        }
        if (text[line].first[poz] != 8 && typeCheck(text[line].first[poz], c)) {
            text[line].second[poz] = 1; // correct character
            poz++;
            if (poz >= text[line].first.size()) // if end of line is reached
                {
                line++;
                pozCpy = poz;
                poz = 0;
                }
        } else if(text[line].first[poz] != 8 && !typeCheck(text[line].first[poz], c)) {
            text[line].second[poz] = -1;
            err.push_back(c);
            poz++;
            if (poz >= text[line].first.size()) // if end of line is reached
                {
                line++;
                pozCpy = poz;
                poz = 0;
                }
        }
        system("clear");
            if(line==text.size()-1 && poz==text[text.size()-1].first.size()-1)
                break;
    }
    auto end=chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::minutes>(end - start).count();
    double t=(((double)getsize(text)/5.0)/(double)duration);
    score(t);
}
int main() {
    string input;
    vector<pair<string, vector<int>>> text;//pair<string, vector<int>>
    vector<char> failure;
    read(text);
    typing(text, failure);
    return 0;
}
