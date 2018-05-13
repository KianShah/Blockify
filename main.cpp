
// File:        main.cpp
// Author:      cheeren
// Date:        2018-02-25
// Description: Partial test of PA3 functionality
//              Reads Point data from external files
//              Produces PNG images of the point sets


#include "twoDtree.h"
#include "cs221util/RGBAPixel.h"
#include "cs221util/PNG.h"
#include <iostream>

using namespace cs221util;
using namespace std;

int main()
{

	// read in image
	PNG origIm1;
	origIm1.readFromFile("images/stanley-totem-poles.png");
	PNG origIm2;
	origIm2.readFromFile("images/ubc-totem-poles.png");
	PNG origIm3;
	origIm3.readFromFile("images/remb.png");
	PNG origIm4;
	origIm4.readFromFile("images/rosa.png");
	PNG origIm5;
	origIm5.readFromFile("images/TestImage.png");

	// use it to build a twoDtree
	twoDtree t1(origIm1);
	cout << "t1 made" << endl;
	twoDtree tCopy1(t1);
	cout << "tCopy1 made" << endl;
	twoDtree t2(origIm2);
	cout << "t2 made" << endl;
	twoDtree tCopy2(t2);
	cout << "tCopy2 made" << endl;
	twoDtree t3(origIm3);
	cout << "t3 made" << endl;
	twoDtree tCopy3(t3);
	cout << "tCopy3 made" << endl;
	twoDtree t4(origIm4);
	cout << "t4 made" << endl;
	twoDtree tCopy4(t4);
	cout << "tCopy4 made" << endl;
	twoDtree t5(origIm5);
	cout << "t5 made" << endl;
	twoDtree tCopy5(t5);
	cout << "tCopy5 made" << endl;


	// prune the twoDtree
	//where at least 100*X% of pixels are within Y of mean
	 tCopy1.prune(0.95, 10000); 
	 cout << "tCopy1 pruned" << endl;
	 tCopy2.prune(0.95, 10000); 
	 cout << "tCopy2 pruned" << endl;
	 tCopy3.prune(0.95, 3000); 
	 cout << "tCopy3 pruned" << endl;
	 tCopy4.prune(0.95, 3000); 
	 cout << "tCopy4 pruned" << endl;
	 tCopy5.prune(0.95, 300);
	 cout << "tCopy5 pruned" << endl;
	
	// render the twoDtree
	PNG ppic1 = tCopy1.render();
	PNG ppic2 = tCopy2.render();
	PNG ppic3 = tCopy3.render();
	PNG ppic4 = tCopy4.render();
	PNG test = tCopy5.render();
	cout << "Images rendered" << endl;

	ppic1.writeToFile("images/prunedstanleytotem.png");
	ppic2.writeToFile("images/prunedubctotem.png");
	ppic3.writeToFile("images/prunedremb.png");
	ppic4.writeToFile("images/prunedrosa.png");
	test.writeToFile("images/NewImage.png");
	cout << "Images written to files" << endl;

  return 0;
}

