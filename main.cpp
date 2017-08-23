#include <stdio.h>
#include <iostream>
#include <fstream>
#include <ios>
#include <Parser.h>
#include <unistd.h>
#include <getopt.h>

using namespace std;

// For Options
const struct option long_options[] = {
    {"ast",  0, NULL, 'a'},
    {"debug",0, NULL, 'd'},
    {NULL,0,NULL,0}
};

// For "-l"
int Scanner_Test(ifstream & inputFile)
{
    long StreamPosition;
    if(!inputFile.is_open())
    {
        cerr<<"ERROR OPENING" <<endl;
        return -1;
    }
    while(1)
    {
        StreamPosition = Scanner(inputFile, &temp_token);
        if(StreamPosition != EOF_POSITION)
        {
            cout/*<<temp_token.Token_type << "  "*/ <<temp_token.value <<endl;
            ReadToken(inputFile,StreamPosition);
        }
        else break;
    }
    return 0;
}

// For "-ast"
int Parser_Test(ifstream & inputFile)
{
    //ifstream  inputFile ("./test1",ios::in);
    Parser(inputFile);           // The AST has been created
    PrintTree(v[0]);             // Print the AST
    free(v[0]);
    return 0;
}

int main(int argc, char * const argv[])
{
    int opt;
    int index = 0;
    ifstream  inputFile;
    bool      fflag = false;

    for(index = 1; index < argc ; ++index)          // Find the filename
    {
        if('-' !=  *argv[index])
        {

            inputFile.open(argv[index]);
            if(!inputFile.is_open())
            {
                cerr<<"ERROR OPENING" <<endl;
                return -1;
            }
            else
            {
                inputFile.close();
                fflag  = true;
                break;
            }

            fflag = true;
            break;
        }

    }

    if(!fflag)
    {
        cerr<< "No valid File" << endl;
        exit(1);
    }

// Actions decided by options
    while((opt = getopt_long(argc,argv,"a:ld:",long_options,NULL))!= -1)
    {
        switch(opt)
        {
            case 'a' : inputFile.open(argv[index]); Parser_Test(inputFile);  inputFile.close(); break;
            case 'l' : inputFile.open(argv[index]); Scanner_Test(inputFile); inputFile.close(); break;
            case 'd' :                                                                          break;
            case '?' : cout<<"?\n" <<endl;                                                      break;
            default  : cout<<"Option: "<<opt<<" Not Found"<<endl;                               break;
        }
    }

    return 0;
}
