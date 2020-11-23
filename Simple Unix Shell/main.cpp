#include <iostream>//Standard input and output stream (to use cout,cin, etc)
#include <vector>//Dynamic Arrays
#include <unistd.h>//Provides access to the OS API(to use fork, pip, etc)
#include <string.h>//use string function instead of array of characters
#include <wait.h>//If the shared state is not yet ready the function blocks the calling thread and waits until it is ready
#include <fstream>// Input/output stream class to operate on files.
#include <stdlib.h>
using namespace std;
ofstream log;
void StartScreen()
{
    cout<<"|-----------------------|\n";
    cout<<"|     Simple Shell      |\n";
    cout<<"|Coded by: Mohamed Saber|\n";
    cout<<"|-----------------------|\n";
}
//Terminate program if input command is "exit"
void TerminateTerminal(string input)
{
    if(input=="exit")
    {
        exit(1);
    }
}
//Check is the input command is empty or having only whitespaces
bool CommandViolation(string input)
{
    bool WhiteSpacesOnly=false;
    char str[input.length()+1];
    strcpy(str,input.c_str());
    int WhiteSpaceCount=NULL;
    //count whitespaces in the command
    for(int i=0; i<input.length()+1; i++)
    {
        if(isspace(str[i]))
        {
            WhiteSpaceCount++;
        }
    }
    //Compare the count of whitespaces and the length of the string
    if(input.length()==WhiteSpaceCount)
    {
        WhiteSpacesOnly= true;
    }
    else WhiteSpacesOnly= false;
    //Check if command is empty or just having empty space
    if(WhiteSpacesOnly || input.empty())
        return true;
    else return false;
}
//Clears whitespaces leading and after the command
string RemoveLeadingLaggingSpaces(string input)
{
    int Begin=0,Last=input.size()-1;
    while(input[Last]==' ')
    {
        Last--;
    }
    input=input.substr(0,Last+1);
    while (input[Begin]==' ')
    {
        Begin++;
    }
    input=input.substr(Begin,Last+1);
    return(input);
}
//Split string into array of strings
vector<string> SplittingCommand(string input)
{
    vector<string> FinalString;
    string TempString ="";
    int i=NULL;
    for(i=0; i<input.length(); i++)
    {
        if(input[i]==' ')
        {
            if(i==0 || input[i-1]==' ')
            {
                continue;
            }
            FinalString.push_back(TempString);
            TempString="";
        }
        else
        {
            TempString.push_back(input[i]);
        }
    }
    if(TempString!="")
    {
        FinalString.push_back(TempString);
    }
    return FinalString;
}
// Checks if command contains & symbol
bool RunBackground(const vector<string>& input)
{
    for(auto& input : input)
    {
        if(input == "&")
        {
            return true;
        }
    }
    return false;
}
// Converts C++ vector to C array of characters
char** ArrayOfCharacters(vector<string> input)
{
    char** FinalArray = new char* ['\0',input.size()+1];
    int i=NULL;
    for(i=0; i<input.size(); i++)
    {
        FinalArray[i] = new char[input[i].size() + 1];
        strcpy(FinalArray[i], input[i].c_str());
    }
    return FinalArray;
}

void SignalHandler(int SignalNumber)
{
    log << "Child "<<SignalNumber<<" terminated" << endl;
}

bool RunCommand(vector<string>& input)
{
//Creates a child process
    int ParentID = fork();
    string TempCommand="";
    //creation of a child process was unsuccessful.
    if(ParentID < 0)
    {
        cout<<"Internal error occured."<<endl;
        return false;
    }
    //Returned to the newly created child process.
    else if(ParentID == 0)
    {
        char** ProcessArray = ArrayOfCharacters(input);
        //The created child process does not have to run the same program as the parent process does
        int Result = execvp(ProcessArray[0], ProcessArray);
        if(Result < 0)
        {
            cout<<ProcessArray[0]<<": command not found."<<"\n";
            return false;
        }

    }
    //Returned to parent or caller. The value contains process ID of newly created child process.
    else
    {
        if(RunBackground(input))
        {
            signal(SIGCHLD, SignalHandler);
        }
        else
        {
            wait(NULL);
        }
    }
    return true;
}
int main()
{
    StartScreen();
    log.open("../log.txt");
    vector<string> commands = {""};
    do
    {
        cout<<"Unix Shell:~$ >> ";
        string input="";
        getline(cin,input);
        while(CommandViolation(input))
        {
            cout<<"Unix Shell:~$ >> ";
            getline(cin,input);
        }
        input=RemoveLeadingLaggingSpaces(input);
        commands = SplittingCommand(input);
        TerminateTerminal(commands[0]);
        if(RunCommand(commands))
        {
            log << input << " executed successfully" << endl;
        }
    }
    while(true);
    log.close();
    return 0;
}
