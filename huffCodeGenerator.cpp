#include <iostream>
#include <queue>
#include <map>
#include <climits> // for CHAR_BIT
#include <iterator>
#include <algorithm>

using namespace std;
const int UniqueSymbols = 1 << CHAR_BIT;
string SampleString = "bbb iiiiiiiiiiiiiiiiii aaaaaaa dd";

typedef map<char, string> HuffCodeMap;
typedef map<string, char> InverseHuffCodeMap;

class INode
{
public:
    const int f;

    virtual ~INode() {}

protected:
    INode(int f) : f(f) {}
};

class InternalNode : public INode
{
public:
    INode *const left;
    INode *const right;

    InternalNode(INode* c0, INode* c1) : INode(c0->f + c1->f), left(c0), right(c1) {}
    ~InternalNode()
    {
        delete left;
        delete right;
    }
};

class LeafNode : public INode
{
public:
    const char c;

    LeafNode(int f, char c) : INode(f), c(c) {}
};

struct NodeCmp
{
    bool operator()(const INode* lhs, const INode* rhs) const { return lhs->f > rhs->f; }
};

INode *huffRoot = NULL;

INode* BuildTree(const int (&frequencies)[UniqueSymbols])
{
    priority_queue<INode*, vector<INode*>, NodeCmp> trees;

    for (int i = 0; i < UniqueSymbols; ++i)
    {
        if(frequencies[i] != 0)
            trees.push(new LeafNode(frequencies[i], (char)i));
    }
    while (trees.size() > 1)
    {
        INode* childR = trees.top();
        trees.pop();

        INode* childL = trees.top();
        trees.pop();

        INode* parent = new InternalNode(childR, childL);
        trees.push(parent);
    }
    return trees.top();
}

void GenerateCodes(const INode* node, const string prefix, HuffCodeMap& outCodes)
{
    if (const LeafNode* lf = dynamic_cast<const LeafNode*>(node))
    {
        outCodes[lf->c] = prefix;
    }
    else if (const InternalNode* in = dynamic_cast<const InternalNode*>(node))
    {
        string leftPrefix = prefix;
        leftPrefix += "0";
        GenerateCodes(in->left, leftPrefix, outCodes);

        string rightPrefix = prefix;
        rightPrefix += "1";
        GenerateCodes(in->right, rightPrefix, outCodes);
    }
}

void decode(const INode* node, string prefix, string encoded, int i, InverseHuffCodeMap& iCodes)
{
    for (int i = 1; i < encoded.length(); i++){
        string code = "";
        i--;
        while(const InternalNode* in = dynamic_cast<const InternalNode*>(node)){
            
            char op = encoded[i];
            if (op == '0'){
                node = in -> left;
                code+="0";
            }else{
                node = in -> right;
                code+="1";
            }
            //code += encoded.substr(i, 1);
            i++;
            
        }
        //code += encoded.substr(i, 1);
        cout << iCodes[code];
        node = huffRoot;
    }
}

int main()
{
    // Build frequency table
    int frequencies[UniqueSymbols] = {0};
    for (int i = 0; i < SampleString.length(); i++ ){
        frequencies[SampleString[i]] += 1;
    }

    INode* root = BuildTree(frequencies);
    huffRoot = root;
    HuffCodeMap codes;
    InverseHuffCodeMap iCodes;
    GenerateCodes(root, "", codes);

    for (HuffCodeMap::const_iterator it = codes.begin(); it != codes.end(); ++it)
    {
        cout << it->first << " ";
        cout << it->second;
        cout << endl;
        iCodes[it->second] = it->first;
    }
    string encoded = "";
    for (int i = 0; i < SampleString.length(); i++){
        encoded += codes[SampleString[i]];
    }
    cout << "Original: "<<endl;
    cout << SampleString << endl;
    cout << "Encoded: " <<endl;
    cout << encoded <<endl;

    cout << "Decoded: " <<endl;
    decode ( huffRoot, "" ,  encoded, 0, iCodes);
    cout << endl;

    delete root;

    return 0;
}
