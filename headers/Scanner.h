#ifndef SCANNER_H
#define SCANNER_H



#include <Token.h>

/*----------------Const Nums--------------------*/
#define NUM_LETTER         52
#define NUM_DIGIT          10
#define NUM_OPERATOR       26
#define NUM_KEYWORD        20
#define NUM_BASE           18
/*----------------Characters--------------------*/
#define TYPE_CHAR           1        /* <CHAR>  */
#define TYPE_DIGIT          2        /* <DIGIT> */
#define TYPE_OPERATOR       3        /*  <OP>   */
#define TYPE_QUOTA          4        /*   ''''  */
#define TYPE_SLASH          5        /*    /    */
#define TYPE_LPAREN         6        /*    (    */
#define TYPE_RPAREN         7        /*    )    */
#define TYPE_SEMICOLON      8        /*    ;    */
#define TYPE_COMMA          9        /*    ,    */
/*-----------------Tokens-----------------------*/
#define TOKEN_IDENTIFIER   10        /*  <ID>   */
#define TOKEN_INTEGER      11        /*<INTEGER>*/
#define TOKEN_OPERATOR     12        /*   OPS   */
#define TOKEN_STRING       13        /* <STRING>*/
#define TOKEN_LPAREN       14        /*    (    */
#define TOKEN_RPAREN       15        /*    )    */
#define TOKEN_SEMICOLON    16        /*    ;    */
#define TOKEN_COMMA        17        /*    ,    */
/*----------------Key_Words---------------------*/
#define TOKEN_LET          18        /*   let   */
#define TOKEN_IN           19        /*   in    */
#define TOKEN_FN           20        /*   fn    */
#define TOKEN_WHERE        21        /*  where  */
#define TOKEN_AUG          22        /*   aug   */
#define TOKEN_OR           23        /*    ,    */
#define TOKEN_NOT          24        /*   not   */
#define TOKEN_GR           25        /*   gr    */
#define TOKEN_GE           26        /*   ge    */
#define TOKEN_LS           27        /*   ls    */
#define TOKEN_LE           28        /*   le    */
#define TOKEN_EQ           29        /*   eq    */
#define TOKEN_NE           30        /*   ne    */
#define TOKEN_TRUE         31        /*  true   */
#define TOKEN_FALSE        32        /*  false  */
#define TOKEN_NIL          33        /*   nil   */
#define TOKEN_DUMMY        34        /*  dummy  */
#define TOKEN_WITHIN       35        /*  within */
#define TOKEN_AND          36        /*   and   */
#define TOKEN_REC          37        /*   rec   */
/*----------------EOF_Flag----------------------*/
#define EOF_POSITION       -1        /*   EOF   */

using namespace std;
typedef unsigned int Char_Type;



/*----------------Const Values------------------*/
const char Letter[NUM_LETTER]            = {'A','B','C','D','E','F','G','H','I','J',
                                            'K','L','M','N','O','P','Q','R','S','T',
                                            'U','V','W','X','Y','Z','a','b','c','d',
                                            'e','f','g','h','i','j','k','l','m','n',
                                            'o','p','q','r','s','t','u','v','w','x',
                                            'y','z'};

const char Digit[ NUM_DIGIT ]            = {'0','1','2','3','4','5','6','7','8','9'};

const char Operator_sysbol[NUM_OPERATOR] = {'+','-','*','<','>','&','.','@','/',':',
                                            '=','~','|','$','!','#','%','^','_','[',
                                            ']','{','}','"','`','?'};


// When keywords list changed, need to modify this list & NUM_BASE & NUM_KEYWORD

const char * Key_Words[NUM_KEYWORD]      = {"let", "in", "fn", "where", "aug", "or",
                                            "not", "gr", "ge", "ls", "le", "eq", "ne",
                                            "true", "false", "nil", "dummy", "within",
                                            "and", "rec"};


/*---------------Unit Functions-----------------*/
inline bool IS_Letter(char c)
{
    for(int i = 0; i < NUM_LETTER; ++i)
    {
        if(Letter[i] == c) return true;
    }
    return false;
}

inline bool IS_Digit(char c)
{
    for(int i = 0; i < NUM_DIGIT; ++i)
    {
        if(Digit[i] == c) return true;
    }
    return false;
}

inline bool IS_Operator_sysbol(char c)
{
    for(int i = 0; i < NUM_OPERATOR; ++i)
    {
        if(Operator_sysbol[i] == c) return true;
    }
    return false;
}

inline int  IS_KeyWord(string s)
{
    for(int i = 0; i != NUM_KEYWORD; ++i)
    {
        if(Key_Words[i] == s) return i + NUM_BASE;
    }
    return TOKEN_IDENTIFIER;
}

inline Char_Type Type_Detector(char c)
{
    if(IS_Letter(c))            return TYPE_CHAR;
    if(IS_Digit(c))             return TYPE_DIGIT;
    if(IS_Operator_sysbol(c))   return TYPE_OPERATOR;
    if('\''== c)                return TYPE_QUOTA;
    if('/' == c)                return TYPE_SLASH;
    if('(' == c)                return TYPE_LPAREN;
    if(')' == c)                return TYPE_RPAREN;
    if(';' == c)                return TYPE_SEMICOLON;
    if(',' == c)                return TYPE_COMMA;
    else
    return -1;
}


bool Comment(ifstream& inputfile)
{
         char inputchar  = inputfile.get();
         if('/' == inputchar)
         {
            string trash;
            getline(inputfile,trash);
            return true;
         }
         else
            inputfile.seekg(-1,ios::cur);
         return false;
}

void Identifier_Token(ifstream& inputfile,string& value)
{
  char inputchar  = inputfile.get();
  while(IS_Letter(inputchar)||IS_Digit(inputchar)||('_' == inputchar))
  {
        value += inputchar;
        inputchar  = inputfile.get();
  }
  inputfile.seekg(-1,ios::cur);
  return;
}

void Digit_Token(ifstream& inputfile,string& value)
{
    char inputchar  = inputfile.get();
    while(IS_Digit(inputchar))
    {
          value += inputchar;
          inputchar  = inputfile.get();
    }
    inputfile.seekg(-1,ios::cur);
    return;
}

void Operator_Token(ifstream& inputfile,string& value)
{
    char inputchar  = inputfile.get();
    while(IS_Operator_sysbol(inputchar))
    {
          value += inputchar;
          inputchar  = inputfile.get();
    }
    inputfile.seekg(-1,ios::cur);
    return;
}

void String_Token(ifstream& inputfile,string& value)
{
    char inputchar;
    do
    {
        inputchar  = inputfile.get();
        value += inputchar;
    }
    while(inputchar != '\'');
}



/*-----------------The Scanner------------------*/
// Read and recongnize a token but with pointer unchanged
// which will change properties of a given Token object
// Return the position after the token, but when reach the
// end of file while somebody still called this function,
// it will return EOF_POSITION which has been defined as -1*/
long Scanner(ifstream & inputFile, TOKEN * pToken)
{
    char inputchar;
    string value = "";
    long finposition;
    long curposition = inputFile.tellg();

    do
    {
        inputchar  = inputFile.get();
    }
    while('\n'== inputchar || ' '== inputchar || '\t'== inputchar);  /* <delete>  */
    if(EOF == inputchar) return EOF_POSITION;

    value += inputchar;
    switch(Type_Detector(inputchar))
    {
        case TYPE_CHAR:      Identifier_Token(inputFile,value)   ;pToken->Token_type = IS_KeyWord(value);pToken->value = value;break;
        case TYPE_DIGIT:     Digit_Token(inputFile,value)        ;pToken->Token_type = TOKEN_INTEGER    ;pToken->value = value;break;
        case TYPE_QUOTA:     String_Token(inputFile,value)       ;pToken->Token_type = TOKEN_STRING     ;pToken->value = value;break;
        case TYPE_LPAREN:    pToken->Token_type = TOKEN_LPAREN   ;pToken->value = value;break;
        case TYPE_RPAREN:    pToken->Token_type = TOKEN_RPAREN   ;pToken->value = value;break;
        case TYPE_COMMA:     pToken->Token_type = TOKEN_COMMA    ;pToken->value = value;break;
        case TYPE_SEMICOLON: pToken->Token_type = TOKEN_SEMICOLON;pToken->value = value;break;
        case TYPE_OPERATOR:
        {
            if('/' == inputchar)       /* <Comment> */
            {
                if(Comment(inputFile))
                {
                    return Scanner(inputFile,pToken);
                }
            }
            Operator_Token(inputFile,value);pToken->Token_type = TOKEN_OPERATOR ;pToken->value = value;break;
        }
        default:             cout<<"UNKNOWN CHAR TYPE!!"<<endl;break;
     }
    finposition = inputFile.tellg();
    inputFile.seekg(curposition);
    return finposition;
}


/*------------------The Read()------------------*/
// Use the pointer returned by Scanner and reset stream pointer
void ReadToken(ifstream& inputfile,long p)
{
    inputfile.seekg(p);
}



#endif // SCANNER_H
