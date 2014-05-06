#include <fstream>
#include <iostream>

using namespace std;

int main() {
	ifstream archEnt ("salida.huf");
	ofstream archSal ("result.txt");

	string linea;
	for(int i=0; i<256; i++) {
		int n;
		archEnt>>n;
		cout << n << " ";
	}
	//cout<<endl;
	archEnt.ignore();
	while(getline(archEnt, linea) != NULL ) {
		cout<<linea<<endl;
	}
	cout<<"linea en blanco" << linea<<endl;



}
