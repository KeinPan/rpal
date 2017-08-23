#ifndef AST_H
#define AST_H
#include <vector>


/*----------------Node_Names------------------*/
#define LetNode      1            /*  "let"   */
#define LambdaNode   2            /* "lambda" */
#define WhereNode    3            /* "where"  */
#define TauNode      4            /*  "tau"   */
#define AugNode      5            /*  "aug"   */
#define IfNode       6            /*  "->"    */
#define OrNode       7            /*  "or"    */
#define AndNode      8            /*   "&"    */     //not confused with "and"
#define NotNode      9            /*  "not"   */
#define GRNode      10            /*  "gr"    */
#define GENode      11            /*  "ge"    */
#define LSNode      12            /*  "ls"    */
#define LENode      13            /*  "le"    */
#define EQNode      14            /*  "eq"    */
#define NENode      15            /*  "ne"    */
#define PlusNode    16            /*   "+"    */
#define MinusNode   17            /*   "-"    */
#define NegNode     18            /*  "neg"   */
#define MultipNode  19            /*   "*"    */
#define DivideNode  20            /*   "/"    */
#define EXPNode     21            /*  "**"    */
#define ATNode      22            /*   "@"    */
#define GammaNode   23            /*  "gamma" */
#define TrueNode    24            /*  "true"  */
#define FalseNode   25            /*  "false" */
#define NilNode     26            /*  "nil"   */
#define DummyNode   27            /*  "dummy" */
#define WithinNode  28            /* "within" */
#define ParelNode   29            /*  "and"   */    //not confused with "&"
#define RecNode     30            /*  "rec"   */
#define AssignNode  31            /*   "="    */    //not confused with EQ
#define FCNNode     32            /*"fcn_form"*/
#define NPNode      33            /*   "()"   */    //no parameters for a func
#define ListNode    34            /*   ","    */    //parameters listed by ','
#define IdentNode   35            /*IDENTIFIER*/
#define IntegerNode 36            /*<INTEGER> */
#define STRNode     37            /* <STRING> */

using namespace std;

/*---------------Pre-Declaration---------------*/
struct TreeNode;
typedef struct TreeNode * AST;


/*---------------Node Structure---------------*/
struct TreeNode
{
  int Degree;
  int ElementType;
  string KeyValue;
  AST Child;
  AST Sibling;
};


/*-----------------Functions------------------*/
// Create an empty but initialized treenode
AST Initialize()
{
    AST p  = new TreeNode;
    p->Degree       = 0;
    p->ElementType  = 0;
    p->Child        = NULL;
    p->Sibling      = NULL;
    p->KeyValue     = "";
    return p;
}

// Customize a treenode's properties except pointers
AST NodeCustomize(AST p, int type,string key,int degree = 0)
{
    p->ElementType = type;
    p->KeyValue    = key;
    p->Degree      = degree;
    return p;
}

//Create a node with values and push it into a stack
void BuildNode(vector<AST> & v, int type, string s, int degree = 0)
{
    v.push_back( NodeCustomize( Initialize(), type, s,degree) );
    return;
}

//Build a tree with a new root while updating the stack simultaneously
void BuildTree(vector<AST> & v, int type, string s, int degree)
{
    // Concatenate Siblings
    AST p  = NULL;
    AST p1 = p;
    for(int i = 0; i!= degree; ++i)
    {
       p = v.back();
       p->Sibling = p1;
       p1 = p;
       v.pop_back();
    }
    // Build the root node
    BuildNode(v,type,s,degree);
    v.back()->Child = p;
    return;
}

// Print the tree using its root node
void PrintTree(AST CurRoot, string separator =  "")  // There is a default parameter
{
    if (CurRoot == NULL) return;                     // Print root itself
    //cout << separator<< "<"<<CurRoot->KeyValue<<":"<< CurRoot->ElementType<<">"<<'('<< CurRoot->Degree <<')'<<endl;
    {
        switch(CurRoot->ElementType)
        {
            case IdentNode:    cout<< separator <<"<ID:"   << CurRoot->KeyValue << ">" <<endl; break;
            case IntegerNode:  cout<< separator <<"<INT:"  << CurRoot->KeyValue << ">" <<endl; break;
            case STRNode:      cout<< separator <<"<STR:"  << CurRoot->KeyValue << ">" <<endl; break;
            case TrueNode:     cout<< separator <<"<true>" << endl;                            break;
            case FalseNode:    cout<< separator <<"<false>"<< endl;                            break;
            case NilNode:      cout<< separator <<"<nil>"  << endl;                            break;
            case DummyNode:    cout<< separator <<"<dummy>"<< endl;                            break;
            default:           cout<< separator <<            CurRoot->KeyValue        <<endl;
        }
    }
    if((CurRoot = CurRoot->Child) != NULL)           // Print its children
    {
        do
        {
            PrintTree(CurRoot, (separator+"."));
            CurRoot = CurRoot->Sibling;
        }
        while(CurRoot != NULL);
    }
    return;
}
#endif // AST_H
