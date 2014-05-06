#include <string>
#include <map>

using namespace std;

// Compress a string to a list of output symbols.
// The result will be written to the output iterator
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

#include <iostream>
#include <iterator>
#include <vector>

int main() {
  vector<int> compressed;
  //call compress with an iterator pointing at the back of compressed
  compress("TOBEORNOTTOBE \nORTOBEORNOT", back_inserter(compressed));
  //print everythin contained in the compressed iterator using an ostream_iterator
  copy(compressed.begin(), compressed.end(), ostream_iterator<int>(cout, ", "));
  cout << endl;
  //decompress returns a string
  string decompressed = decompress(compressed.begin(), compressed.end());
  cout << decompressed << endl;

  return 0;
}
