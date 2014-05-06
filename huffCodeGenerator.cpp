#include <iostream>
#include <sstream>
#include <queue>
#include <map>
#include <climits> // for CHAR_BIT
#include <iterator>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>

using namespace std;
const int UniqueSymbols = 1 << CHAR_BIT;
string SampleString = "bbb iiiiiiiiiiiiiiiiii aaaaaaa dd";

typedef map<char, string> HuffCodeMap;
typedef map<string, char> InverseHuffCodeMap;

//PARA LZW
// Compress a string to a list of output symbols. // The result will be written to the output iterator
// starting at "result"; the final iterator is returned.
template <typename Iterator>
Iterator compress(const string &uncompressed, Iterator result) {
  // Build the dictionary.
  int dictSize = 256;
  map<string,int> dictionary;
  for (int i = 0; i < 256; i++)
    dictionary[string(1, i)] = i;

  string w;
  for (string::const_iterator it = uncompressed.begin();
       it != uncompressed.end(); ++it) {
    char c = *it;
    string wc = w + c;
    //if wc exists in dictionary
    if (dictionary.count(wc))
      w = wc;
    else {
    //assign the value on dictionary[w] to *result and then advance the
    //iterator
      *result++ = dictionary[w];
      // Add wc to the dictionary.
      // And then increment the dictionary.
      dictionary[wc] = dictSize++;
      //fill w with 1 repetition of c
      w = string(1, c);
    }
  }

  // Output the code for w.
  // if w is still not empty
  if (!w.empty())
  //asign value of w to result and then increment iterator
    *result++ = dictionary[w];
    //result is now the iterator at the end of the result...
  return result;
}

// Decompress a list of output ks to a string.
// "begin" and "end" must form a valid range of ints
template <typename Iterator>
string decompress(Iterator begin, Iterator end) {
  // Build the dictionary.
  int dictSize = 256;
  map<int,string> dictionary;
  // builds a dictionary of strings
  // index 0 = "0" index 1 = "1" ...
  for (int i = 0; i < 256; i++)
    dictionary[i] = string(1, i);

  //initializes a string w with 1 repetition of the value in *begin
  // in other words it parses the int value pointed by begin to a string
  string w(1, *begin++);
  string result = w;
  string entry;
  //keep cycling the compressed vector
  for ( ; begin != end; begin++) {
    // k is the value pointed at by the begin iterator
    int k = *begin;
    // if k exists in dictionary
    if (dictionary.count(k))
        //entry is equal to the value of dictionary in k
      entry = dictionary[k];
    else if (k == dictSize)
      entry = w + w[0];
    else
      throw "Bad compressed k";

    result += entry;

    // Add w+entry[0] to the dictionary.
    dictionary[dictSize++] = w + entry[0];

    w = entry;
  }
  return result;
}
// FIN DE FUNCIONES PARA LZW

// PARA HUFFMAN
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

string decode(const INode* node, string prefix, string encoded, int i, InverseHuffCodeMap& iCodes)
{
	string result = "";
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
        result += iCodes[code];
        cout <<result<<endl;
        node = huffRoot;
    }

    return result;
}
// FIN DE HUFFMAN

void printMenu(){
    cout<<endl;
    cout<<"------------ MENU ---------------"<<endl;
    cout<<" 1) Code "<<endl;
    cout<<" 2) Decode " <<endl;
    cout<<" 9) Salir " <<endl;
}

int main()
{
	int opcionElegida = 0;
	while(opcionElegida != 9) {

        printMenu();
        cin>>opcionElegida;
        cout<<endl;
        cout<<endl;

		if(opcionElegida != 9) {

			string nomArchEnt, nomArchSal;
			cout<<"Introduzca nombre de archivo de entrada (sin extensión) "<<endl;
			cin>>nomArchEnt;
			cout<<"Introduzca nombre de archivo de salida (sin extensión) "<<endl;
			cin>>nomArchSal;

            string nomArchSalLzw = nomArchSal;

			if(opcionElegida == 1) {
				nomArchEnt += ".txt";
				nomArchSal += ".huf";
                nomArchSalLzw += ".lzw";

				ifstream archEnt (nomArchEnt.c_str());
				ofstream archSal (nomArchSal.c_str());
				string linea;

				// Build frequency table
				int frequencies[UniqueSymbols] = {0};
				getline (archEnt, linea);
				string contenido = linea;
				while ( getline (archEnt, linea) ){

					contenido += "\n"+linea;

				}

				cout << "CONTENIDO" << contenido << endl;
				//linea+="\n";
				for (int i = 0; i < contenido.length(); i++ ){
					frequencies[contenido[i]] += 1;
				}

                //create huffCode tree and get the huffcode map

				INode* root = BuildTree(frequencies);
				huffRoot = root;
				HuffCodeMap codes;
				InverseHuffCodeMap iCodes;
				GenerateCodes(root, "", codes);

				/*
				archSal << codes.size() << endl;
				for (HuffCodeMap::const_iterator it = codes.begin(); it != codes.end(); ++it)
				{
					if (string(1,it->first) == "\n"){
						archSal << "salto " << it -> second << endl;
					}else {
						archSal << it->first << " ";
						archSal << it->second << endl;
						iCodes[it->second] = it->first;
					}
				}
				*/

				for(int i=0; i<256; i++) {
					archSal<<frequencies[i]<<" ";
				}

				string encoded = "";
				for (int i = 0; i < contenido.length(); i++){
					encoded += codes[contenido[i]];
				}

				cout <<"Linea encoded:  " << encoded <<endl;
				archSal<<encoded<<endl;


				archSal.close();
				archEnt.close();

                //sigue lzw
                //usare el string contenido y los metodos de lzw
                //para encode, y tengo que abrir el archSal nuevamente
				ofstream archSalLzw (nomArchSalLzw.c_str());

                vector<int> compressed;
                //call compress with an iterator pointing at the back of compressed
                compress(contenido, back_inserter(compressed));
                //compressed tiene mi resultado
                //print everythin contained in the compressed iterator using an ostream_iterator
                copy(compressed.begin(), compressed.end(), ostream_iterator<int>(cout, ", "));
                ostringstream oss;
                copy(compressed.begin(), compressed.end(), ostream_iterator<int>(oss, ", "));
                string compS = oss.str();
                cout << endl;
                cout << "String comprimido..." << endl;
                cout << compS << endl;

                //decompress returns a string
//                string decompressed = decompress(compressed.begin(), compressed.end());
//                cout << decompressed << endl;

                archSalLzw.close();

			} else if (opcionElegida == 2) {
				// Build frequency table
				int frequencies[UniqueSymbols] = {0};

				nomArchEnt += ".huf";
				nomArchSal += ".txt";

				ifstream archEnt (nomArchEnt.c_str());
				ofstream archSal (nomArchSal.c_str());

				int cantCodes = 0;
				//archEnt>>cantCodes;

				for(int i=0; i<256; i++) {
					archEnt >> frequencies[i];
					cout << frequencies[i];
				}
				cout<<endl;

				INode* root = BuildTree(frequencies);
				huffRoot = root;
				HuffCodeMap codes;
				InverseHuffCodeMap iCodes;
				GenerateCodes(root, "", codes);
				for (HuffCodeMap::const_iterator it = codes.begin(); it != codes.end(); ++it)
				{
						iCodes[it->second] = it->first;
						cout<<"codes: "<<it->second<<"   "<<it->first<<endl;
				}
				string linea;
				getline(archEnt, linea);
				while(linea[0] ==  ' ') {
					linea = linea.substr(1);
				}
				/*
				while(getline(archEnt, linea) != NULL) {
					//cout << "linea:  " << linea << endl;
				}
				cout << "linea:  " << linea << endl;
				*/
				cout << "linea:" << linea << endl;
				archSal << decode ( huffRoot, "" ,  linea, 0, iCodes);
			}



		}
	}

    /*
    cout << "Original: "<<endl;
    cout << SampleString << endl;
    cout << "Encoded: " <<endl;
    cout << encoded <<endl;


    cout << "Decoded: " <<endl;
    decode ( huffRoot, "" ,  encoded, 0, iCodes);
    cout << endl;

    delete root;
    */

    return 0;
}
