#include<iostream>
#include<fstream>
#include<cstdlib>
#include<algorithm>
#include<stack>
#include<map>
#include<cstring>
#include<cassert>
#define CodeLength 5000
#define CodeLimit 50000
#define CommandLength 500
#define CommandWidth 5000
#define FILENAME "cat_program.txt"
using namespace std;

ifstream in;
string CommandAry[CodeLength];
stack<int> Stack;
map<string,int> Label;
map<pair<int,int>,int> BefungeCode;

int StackPop(){
    int s = Stack.top();
    Stack.pop();
    return s;
}

int main(){
    in.open(FILENAME);
    if(in.fail()){
        cout << "couldn't find the file.\n";
        exit(1);
    }

    int line = 0;
    string str;
    while(getline(in, str)){
        while(str[0] == '\t' || str[0] == ' '){
            str.replace(0,str.length(),str,1,str.length()-1);
        }
        if(str == "if0{"){
            Stack.push(line);
            CommandAry[line++] = str;
        }
        else if(str[0] == '}'){
            string s0 = "_if0" + to_string(StackPop());
            Label[s0] = line;
        }
        else if(str[0] == '('){
            str.replace(0, str.length(), str, 1, str.length()-2);
            Label[str] = line;
        }
        else{
            CommandAry[line++] = str;
        }
    }
    in.close();

    
    int a,b;
    char c;
    for(int i=0,l=0;i<line && l<CodeLimit;i++,l++){
        //+
        if(CommandAry[i] == "+"){
            a = StackPop();
            b = StackPop();
            Stack.push(a+b);
        }
        //-
        else if(CommandAry[i] == "-"){
            a = StackPop();
            b = StackPop();
            Stack.push(b-a);
        }
        //*
        else if(CommandAry[i] == "*"){
            a = StackPop();
            b = StackPop();
            Stack.push(a*b);
        }
        // /
        else if(CommandAry[i] == "/"){
            a = StackPop();
            b = StackPop();
            assert(a != 0);
            Stack.push(b/a);
        }
        //%
        else if(CommandAry[i] == "%"){
            a = StackPop();
            b = StackPop();
            Stack.push(b%a);
        }
        //!
        else if(CommandAry[i] == "not"){
            a = StackPop();
            Stack.push((a==0)? 1:0);
        }
        //`
        else if(CommandAry[i] == "greater"){
            a = StackPop();
            b = StackPop();
            Stack.push((b>a)? 1:0);
        }
        //_ |
        else if(CommandAry[i] == "if0{"){
            a = StackPop();
            if(a != 0){
                string s0 = "_if0" + to_string(i);
                i = Label[s0] - 1;
            }
        }
        //:
        else if(CommandAry[i] == "duplicate"){
            Stack.push(Stack.top());
        }
        //'\'
        else if(CommandAry[i] == "swap"){
            a = StackPop();
            b = StackPop();
            Stack.push(a);
            Stack.push(b);
        }
        //$
        else if(CommandAry[i] == "pop"){
            Stack.pop();
        }
        //.
        else if(CommandAry[i] == "printi"){
            a = StackPop();
            cout<<(int)a;
        }
        //,
        else if(CommandAry[i] == "printc"){
            c = StackPop();
            cout<<c;
        }
        //&
        else if(CommandAry[i] == "inputi"){
            cin>>a;
            Stack.push(a);
        }
        //~
        else if(CommandAry[i] == "inputc"){
            cin>>c;
            Stack.push(c);
        }
        //p
        else if(CommandAry[i].compare(0, 3, "put")==0){
            b = StackPop();
            a = StackPop();
            int v = StackPop();
            BefungeCode[{a,b}] = v;
        }
        //g
        else if(CommandAry[i] == "get"){
            b = StackPop();
            a = StackPop();
            if(a<0 || a>=CommandWidth || b<0 || b>=CommandLength) Stack.push(0);
            else Stack.push(BefungeCode[{a,b}]);
        }
        //@
        else if(CommandAry[i] == "return"){
            break;
        }
        //push
        else if(CommandAry[i].compare(0, 4, "push")==0){
            if(CommandAry[i][5]=='"'){
                for(int j=6;j<CommandAry[i].length()-1;j++){
                    Stack.push(CommandAry[i][j]);
                }
            }
            else{
                Stack.push(CommandAry[i][5]-'0');
            }
        }
        //unconditional jump
        else if(CommandAry[i].compare(0, 4, "jump")==0){
            string s0;
            s0.append(CommandAry[i], 5, CommandAry[i].length()-5);
            if(Label.count(s0) == 0){
                cout<<"invalid label\n";
            }
            else{
                i = Label[s0]-1;
            }
        }
        else{
            cout << "invalid command in line " << i+1 << "\n";
        }

    }
}