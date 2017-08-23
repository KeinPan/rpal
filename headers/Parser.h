#ifndef PARSER_H
#define PARSER_H

/*----------------Head_Files------------------*/
#include <Scanner.h>
#include <AST.h>






/*----------------Global Vars-----------------*/
TOKEN temp_token  = {0,""};       // Temp token recording the result of each Scanner() call
vector<AST> v;                    // Implenting the stack by using vector for convenience
long StreamPosition;              // Recording current stream pointer position

/*--------------Pre-Declaration---------------*/
void E(ifstream& inputFile);
void D(ifstream& inputFile);




/*---------------Unit Functions---------------*/

//Error Message Output
inline void TokenMissed(string s)
{
    cerr<< "Syntax Error:  ->'"<<s<<"'<-   Expected. But Got:  "
        <<temp_token.value<<"  "<<temp_token.Token_type <<endl;
    exit(1);
}

// Called when we assure there is a digit followed
inline void Integer_Node(ifstream& inputFile)
{
    StreamPosition = Scanner(inputFile,&temp_token);
    if (TOKEN_INTEGER == temp_token.Token_type)
    {
        ReadToken(inputFile,StreamPosition);
        BuildNode(v,IntegerNode,temp_token.value);
    }
    else TokenMissed("<int>");
    return;
}

// Called when we assure there is a string followed
inline void String_Node(ifstream& inputFile)
{
    StreamPosition = Scanner(inputFile,&temp_token);
    if (TOKEN_STRING == temp_token.Token_type)
    {
        ReadToken(inputFile,StreamPosition);
        BuildNode(v,STRNode,temp_token.value);
    }
    else TokenMissed("<String>");
    return;
}

// Called when we assure there is an Id followed
inline void Identifer_Node(ifstream& inputFile)
{
    StreamPosition = Scanner(inputFile,&temp_token);
    if (TOKEN_IDENTIFIER == temp_token.Token_type)
    {
        ReadToken(inputFile,StreamPosition);
        BuildNode(v,IdentNode,temp_token.value);
    }
    else TokenMissed("<Identifier>");
}





/*------------------Grammars--------------------*/
/* Each NT call would go through the text and read a part that could be reduced to itself,
 * then return with pointer changed, and a part of AST built in the stack, which would be
 * stored in vector<AST> v, temp_token & StreamPosition are temp vars for convenience*/

/*# Variables ##############################################
    Vb  -> ’<IDENTIFIER>’
        -> ’(’ Vl ’)’
        -> ’(’ ’)’                 => ’()’;
    Vl  -> ’<IDENTIFIER>’ list ’,’ => ’,’?;              */
void Vl(ifstream& inputFile)
{
    int N = 1;
    Identifer_Node(inputFile);
    StreamPosition = Scanner(inputFile,&temp_token);
    while(TOKEN_COMMA == temp_token.Token_type)
    {
        ReadToken(inputFile,StreamPosition);
        Identifer_Node(inputFile);
        N++;
        StreamPosition = Scanner(inputFile,&temp_token);
    }
    if(1 != N)
    BuildTree(v, ListNode,",",N);
}

void Vb(ifstream& inputFile)
{
    StreamPosition = Scanner(inputFile,&temp_token);
    if (EOF_POSITION == StreamPosition)
    {
        cerr<< " Error: Attempting to get a Vb while reaching EOF "<<endl; // For Db
        exit(1);
    }
    if (TOKEN_IDENTIFIER == temp_token.Token_type)
    {
        ReadToken(inputFile,StreamPosition);
        BuildNode(v,IdentNode,temp_token.value);
        return;
    }
    else if (TOKEN_LPAREN == temp_token.Token_type)
    {
        ReadToken(inputFile,StreamPosition);
        StreamPosition = Scanner(inputFile,&temp_token);
        if (TOKEN_RPAREN == temp_token.Token_type)
        {
            ReadToken(inputFile,StreamPosition);
            BuildNode(v,NPNode,"()");
        }
        else
        {
            Vl(inputFile);
            StreamPosition = Scanner(inputFile,&temp_token);
            if(TOKEN_RPAREN == temp_token.Token_type)
                 ReadToken(inputFile,StreamPosition);
            else TokenMissed(")");
         }
        return;
    }
}


/*# Definitions ############################################
    D  -> Da ’within’ D            => ’within’
       -> Da ;
    Da -> Dr ( ’and’ Dr )+         => ’and’
       -> Dr ;
    Dr -> ’rec’ Db                 => ’rec’
       -> Db ;
    Db -> Vl ’=’ E                 => ’=’
       -> ’<IDENTIFIER>’ Vb+ ’=’ E => ’fcn_form’
       -> ’(’ D ’)’ ;                                      */
void Db(ifstream& inputFile)
{
    long OriginalPosition = inputFile.tellg();
    StreamPosition = Scanner(inputFile,&temp_token);
    if (TOKEN_LPAREN == temp_token.Token_type)           // ’(’ D ’)’
    {
        ReadToken(inputFile,StreamPosition);
        D(inputFile);
        StreamPosition = Scanner(inputFile,&temp_token);
        if(TOKEN_RPAREN == temp_token.Token_type)
        {
            ReadToken(inputFile,StreamPosition);
            return;
        }
        else TokenMissed(")");
    }

    Identifer_Node(inputFile);                           // The first <id> has been pushed in
    StreamPosition = Scanner(inputFile,&temp_token);     // Read the next token after 1st <id>

    if((TOKEN_COMMA == temp_token.Token_type)||("=" == temp_token.value))        //  Vl ’=’ E
    {

        v.pop_back();
        inputFile.seekg(OriginalPosition);
        Vl(inputFile);
        StreamPosition = Scanner(inputFile,&temp_token);
        if("=" == temp_token.value)
        {
            ReadToken(inputFile,StreamPosition);
            E(inputFile);
            BuildTree(v,AssignNode,"=",2);
            return;
        }
        else TokenMissed("=");
    }
    else                                                //<IDENTIFIER>’ Vb+ ’=’ E => ’fcn_form’
    {
        int N = 1;
        do
        {
            Vb(inputFile);
            N++;
            StreamPosition = Scanner(inputFile,&temp_token);
        }
        while("=" != temp_token.value);
        ReadToken(inputFile,StreamPosition);
        E(inputFile);
        N++;
        BuildTree(v,FCNNode,"function_form",N);
        return;
    }
}

void Dr(ifstream& inputFile)
{
    StreamPosition = Scanner(inputFile,&temp_token);
    if (TOKEN_REC == temp_token.Token_type)
    {
        ReadToken(inputFile,StreamPosition);
        Db(inputFile);
        BuildTree(v,RecNode,"rec",1);
    }
    else
    {
        Db(inputFile);
    }
    return;
}

void Da(ifstream& inputFile)
{
    int N = 0;
    Dr(inputFile);
    N++;
    StreamPosition = Scanner(inputFile,&temp_token);
    while(TOKEN_AND == temp_token.Token_type)
    {
        ReadToken(inputFile,StreamPosition);
        Dr(inputFile);
        N++;
        StreamPosition = Scanner(inputFile,&temp_token);
    }
    if(N != 1)
    {
        BuildTree(v,ParelNode,"and",N);
    }
    return;
}

void D (ifstream& inputFile)
{
    Da(inputFile);
    StreamPosition = Scanner(inputFile,&temp_token);
    if(TOKEN_WITHIN == temp_token.Token_type)
    {
        ReadToken(inputFile,StreamPosition);
        D(inputFile);
        BuildTree(v,WithinNode,"within",2);
    }
    return;
}


/* # Rators And Rands #######################################
    R  -> R Rn                  => ’gamma’
       -> Rn ;
    Rn -> ’<IDENTIFIER>’
       -> ’<INTEGER>’
       -> ’<STRING>’
       -> ’true’                => ’true’
       -> ’false’               => ’false’
       -> ’nil’                 => ’nil’
       -> ’(’ E ’)’
       -> ’dummy’               => ’dummy’ ;              */
bool Rn(ifstream& inputFile)
{
    StreamPosition = Scanner(inputFile,&temp_token);
    if(EOF_POSITION == StreamPosition)                                   return false;
    switch(temp_token.Token_type)
    {
        case TOKEN_INTEGER:     Integer_Node(inputFile);                 return true;

        case TOKEN_STRING:      String_Node(inputFile);                  return true;

        case TOKEN_IDENTIFIER:  Identifer_Node(inputFile);               return true;

        case TOKEN_TRUE:        BuildNode(v, TrueNode ,temp_token.value);
                                ReadToken(inputFile,StreamPosition);     return true;

        case TOKEN_FALSE:       BuildNode(v, FalseNode,temp_token.value);
                                ReadToken(inputFile,StreamPosition);     return true;

        case TOKEN_NIL:         BuildNode(v, NilNode  ,temp_token.value);
                                ReadToken(inputFile,StreamPosition);     return true;
        case TOKEN_DUMMY:       BuildNode(v, DummyNode,temp_token.value);
                                ReadToken(inputFile,StreamPosition);     return true;
        case TOKEN_LPAREN:
            ReadToken(inputFile,StreamPosition);
            E(inputFile);
            StreamPosition = Scanner(inputFile,&temp_token);
            if(TOKEN_RPAREN != temp_token.Token_type)
            TokenMissed(")");
            else ReadToken(inputFile,StreamPosition);
            return true;
        default: return false;
    }
}

void R(ifstream& inputFile)
{
    Rn(inputFile);
    while(Rn(inputFile))
    {
        BuildTree(v,GammaNode,"gamma",2);
    }
    return;
}



/*# Arithmetic Expressions #################################
    A  -> A’+’ At       => ’+’
       -> A ’-’ At      => ’-’
       -> ’+’ At
       -> ’-’ At        => ’neg’  //HOLE:Unary+- got lowest priority
       -> At ;
    At -> At ’*’ Af     => ’*’
       -> At ’/’ Af     => ’/’
       -> Af ;
    Af -> Ap ’**’ Af    => ’**’
       -> Ap ;
    Ap -> Ap ’@’ ’<IDENTIFIER>’ R => ’@’
       -> R ;                                              */

void Ap(ifstream& inputFile)
{
    R(inputFile);
    StreamPosition = Scanner(inputFile,&temp_token);
    while("@" == temp_token.value)
    {
        ReadToken(inputFile,StreamPosition);
        Identifer_Node(inputFile);
        R(inputFile);
        BuildTree(v,ATNode,"@",3);
        StreamPosition = Scanner(inputFile,&temp_token);
    }
    return;
}

void Af(ifstream& inputFile)
{
    Ap(inputFile);
    StreamPosition = Scanner(inputFile,&temp_token);
    if("**" == temp_token.value)
    {
        ReadToken(inputFile,StreamPosition);
        Af(inputFile);
        BuildTree(v,EXPNode,"**",2);
    }
    return;

}

void At(ifstream& inputFile)
{
    string temp = "";
    Af(inputFile);
    StreamPosition = Scanner(inputFile,&temp_token);
    while(("*" == temp_token.value)||("/" == temp_token.value))
    {
        temp = temp_token.value;
        ReadToken(inputFile,StreamPosition);
        Af(inputFile);
        if("*" == temp)  BuildTree(v,MultipNode,temp,2);
        else             BuildTree(v,DivideNode,temp,2);
        StreamPosition = Scanner(inputFile,&temp_token);
    }
    return;

}

void A(ifstream& inputFile)
{
    string temp = "";
    StreamPosition = Scanner(inputFile,&temp_token);
    if(("+" == temp_token.value)||("-" == temp_token.value))
    {
         temp = temp_token.value;
         ReadToken(inputFile,StreamPosition);
         At(inputFile);
         if("-" == temp)
         {
             BuildTree(v,NegNode,"neg",1);
         }
         return;
    }
    else
    {
        At(inputFile);
        StreamPosition = Scanner(inputFile,&temp_token);
        while(("+" == temp_token.value)||("-" == temp_token.value))
        {
            temp = temp_token.value;
            ReadToken(inputFile,StreamPosition);
            At(inputFile);
            if("+" == temp)  BuildTree(v,PlusNode,temp,2);
            else             BuildTree(v,MinusNode,temp,2);
            StreamPosition = Scanner(inputFile,&temp_token);
        }
        return;
    }
}



/*# Boolean Expressions ####################################
    B  -> B’or’ Bt              => ’or’
       -> Bt ;
    Bt -> Bt ’&’ Bs             => ’&’
       -> Bs ;
    Bs -> ’not’ Bp              => ’not’
       -> Bp ;
    Bp -> A (’gr’ | ’>’ ) A     => ’gr’
       -> A (’ge’ | ’>=’) A     => ’ge’
       -> A (’ls’ | ’<’ ) A     => ’ls’
       -> A (’le’ | ’<=’) A     => ’le’
       -> A ’eq’ A              => ’eq’
       -> A ’ne’ A              => ’ne’
       -> A ;                                              */
void Bp(ifstream& inputFile)
{
    A(inputFile);
    StreamPosition = Scanner(inputFile,&temp_token);
    if     ((TOKEN_GR == temp_token.Token_type)||(">" == temp_token.value))
    {
        ReadToken(inputFile,StreamPosition);
        A(inputFile);
        BuildTree(v,GRNode,"gr",2);
    }
    else if((TOKEN_GE == temp_token.Token_type)||(">=" == temp_token.value))
    {
        ReadToken(inputFile,StreamPosition);
        A(inputFile);
        BuildTree(v,GENode,"ge",2);
    }
    else if((TOKEN_LS == temp_token.Token_type)||("<" == temp_token.value))
    {
        ReadToken(inputFile,StreamPosition);
        A(inputFile);
        BuildTree(v,LSNode,"ls",2);
    }
    else if((TOKEN_LE == temp_token.Token_type)||("<=" == temp_token.value))
    {
        ReadToken(inputFile,StreamPosition);
        A(inputFile);
        BuildTree(v,LENode,"le",2);
    }
    else if(TOKEN_EQ == temp_token.Token_type)
    {
        ReadToken(inputFile,StreamPosition);
        A(inputFile);
        BuildTree(v,EQNode,"eq",2);
    }
    else if(TOKEN_NE == temp_token.Token_type)
    {
        ReadToken(inputFile,StreamPosition);
        A(inputFile);
        BuildTree(v,NENode,"ne",2);
    }
    return;
}

void Bs(ifstream& inputFile)
{
    StreamPosition = Scanner(inputFile,&temp_token);
    if (TOKEN_NOT == temp_token.Token_type)
    {
        ReadToken(inputFile,StreamPosition);
        Bp(inputFile);
        BuildTree(v,NotNode,"not",1);
    }
    else
    {
        Bp(inputFile);
    }
    return;
}

void Bt(ifstream& inputFile)
{
    Bs(inputFile);
    StreamPosition = Scanner(inputFile,&temp_token);
    while("&" == temp_token.value)
    {
        ReadToken(inputFile,StreamPosition);
        Bs(inputFile);
        BuildTree(v,AndNode,"&",2);
        StreamPosition = Scanner(inputFile,&temp_token);
    }
    return;
}

void B(ifstream& inputFile)
{
    Bt(inputFile);
    StreamPosition = Scanner(inputFile,&temp_token);
    while(TOKEN_OR == temp_token.Token_type)
    {
        ReadToken(inputFile,StreamPosition);
        Bt(inputFile);
        BuildTree(v,OrNode,"or",2);
        StreamPosition = Scanner(inputFile,&temp_token);
    }
    return;
}



/*# Tuple Expressions ########s##############################
    T  -> Ta ( ’,’ Ta )+                    => ’tau’
       -> Ta ;
    Ta -> Ta ’aug’ Tc                       => ’aug’
       -> Tc ;
    Tc -> B ’->’ Tc ’|’ Tc                  => ’->’
       -> B ;                                              */
void Tc(ifstream& inputFile)
{
    B(inputFile);
    StreamPosition = Scanner(inputFile,&temp_token);
    if ("->" == temp_token.value)
    {
        ReadToken(inputFile,StreamPosition);
        Tc(inputFile);
        StreamPosition = Scanner(inputFile,&temp_token);
        if ("|" == temp_token.value)
        {
            ReadToken(inputFile,StreamPosition);
            Tc(inputFile);
            BuildTree(v,IfNode,"->",3);
        }
        else TokenMissed("|");
    }
    return;
}

void Ta(ifstream& inputFile)
{
    Tc(inputFile);
    StreamPosition = Scanner(inputFile,&temp_token);
    while(TOKEN_AUG == temp_token.Token_type)
    {
        ReadToken(inputFile,StreamPosition);
        Tc(inputFile);
        BuildTree(v,AugNode,"aug",2);
        StreamPosition = Scanner(inputFile,&temp_token);
    }
    return;
}

void T(ifstream& inputFile)
{
    int N = 0;
    Ta(inputFile);
    N++;
    StreamPosition = Scanner(inputFile,&temp_token);
    while(TOKEN_COMMA == temp_token.Token_type)
    {
        ReadToken(inputFile,StreamPosition);
        Ta(inputFile);
        N++;
        StreamPosition = Scanner(inputFile,&temp_token);
    }
    if(N != 1)
    {
        BuildTree(v,TauNode,"tau",N);
    }
    return;
}

/* Expressions ############################################
    E  -> ’let’ D ’in’ E => ’let’
       -> ’fn’ Vb+ ’.’ E => ’lambda’
       -> Ew;
    Ew -> T ’where’ Dr => ’where’
       -> T;                                               */

void Ew(ifstream& inputFile)
{
    T(inputFile);
    StreamPosition = Scanner(inputFile,&temp_token);
    if (TOKEN_WHERE == temp_token.Token_type)
    {
        ReadToken(inputFile,StreamPosition);
        Dr(inputFile);
        BuildTree(v,WhereNode,"where",2);
    }
    return;
}

void E(ifstream& inputFile)
{
    int N = 1;                          //Recording #children. Shoud NOT be defined in case clause
    StreamPosition = Scanner(inputFile,&temp_token);
    switch(temp_token.Token_type)
    {
        case TOKEN_LET:
            ReadToken(inputFile,StreamPosition);
            D(inputFile);
            StreamPosition = Scanner(inputFile,&temp_token);
            if (TOKEN_IN == temp_token.Token_type)
            {
                ReadToken(inputFile,StreamPosition);
                E(inputFile);
                BuildTree(v,LetNode,"let",2);
                return;
            }
            else TokenMissed("in");
         case TOKEN_FN:
            ReadToken(inputFile,StreamPosition);
            do
            {
                Vb(inputFile);
                N++;
                StreamPosition = Scanner(inputFile,&temp_token);
            }
            while("." != temp_token.value );
            ReadToken(inputFile,StreamPosition);
            E(inputFile);
            BuildTree(v,LambdaNode,"lambda",N);
            return;
        default:
            Ew(inputFile);return;
    }
}



/*------------------Parser--------------------*/
/* After the E() call, pointer should be located at the last '\n' or EOF
 * Then in Linux System , there is always a '\n' before the EOF. We check
 * whether we reached the end of the file or not. If parsing successfully
 * it will returnto main with the vector whose 1st element store the whole
 * tree, so there is no need to return any other parameters to main func*/
void Parser(ifstream& inputFile)
{
    E(inputFile);
    char ch = inputFile.get();
    if('\n' == ch) ch = inputFile.get();
    if(EOF != ch) //cout << "Done Parsing..."              <<endl;
               cerr << "EOF Expected but Got:  "<< ch <<endl;
    return;
}
#endif // PARSER_H

