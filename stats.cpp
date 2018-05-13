#include "stats.h"
#include <utility>
#include <vector>
#include <cmath>
#include <iostream>

using namespace std;
using namespace cs221util;

/* returns the sums of all pixel values across the given color channel.
* useful in computing the score of a rectangle
* PA3 function
* @param channel is one of r, g, or b
* @param ul is (x,y) of the upper left corner of the rectangle 
* @param lr is (x,y) of the lower right corner of the rectangle */
long stats::getSum(char channel, pair<int,int> ul, pair<int,int> lr)
{
	int ul_x = ul.first;
	int ul_y = ul.second;
	int lr_x = lr.first;
	int lr_y = lr.second;
	vector<vector<long>>* vect;
	
	switch (channel)
	{
		case 'r':
			vect = &sumRed;
			break;
		case 'g':
			vect = &sumGreen;
			break;
		case 'b':
			vect = &sumBlue;
			break;
	}
	
	if (ul_x > 0 && ul_y > 0) 			// General case
		return (*vect)[lr_x][lr_y] - (*vect)[ul_x-1][lr_y] - (*vect)[lr_x][ul_y-1] + (*vect)[ul_x-1][ul_y-1];
	
	else if (ul_x == 0 && ul_y > 0)		// Case where ul is on the left column
		return (*vect)[lr_x][lr_y] - (*vect)[lr_x][ul_y -1];
	
	else if (ul_x > 0 && ul_y == 0) 	// Case where ul is on the top row
		return (*vect)[lr_x][lr_y] - (*vect)[ul_x-1][lr_y];
	
	else return (*vect)[lr_x][lr_y];	// Case where ul is (0,0)
}

/* returns the sums of squares of all pixel values across the given color channel.
* useful in computing the score of a rectangle
* PA3 function
* @param channel is one of r, g, or b
* @param ul is (x,y) of the upper left corner of the rectangle 
* @param lr is (x,y) of the lower right corner of the rectangle */
long stats::getSumSq(char channel, pair<int,int> ul, pair<int,int> lr)
{
	int ul_x = ul.first;
	int ul_y = ul.second;
	int lr_x = lr.first;
	int lr_y = lr.second;
	vector<vector<long>>* vect;
	
	switch (channel)
	{
		case 'r':
			vect = &sumsqRed;
			break;
		case 'g':
			vect = &sumsqGreen;
			break;
		case 'b':
			vect = &sumsqBlue;
			break;
	}
	
	if (ul_x > 0 && ul_y > 0)			// General case
		return (*vect)[lr_x][lr_y] - (*vect)[ul_x-1][lr_y] - (*vect)[lr_x][ul_y-1] + (*vect)[ul_x-1][ul_y-1];
	
	else if (ul_x == 0 && ul_y > 0) 	// Case where ul is on the left column
		return (*vect)[lr_x][lr_y] - (*vect)[lr_x][ul_y -1];
	
	else if (ul_x > 0 && ul_y == 0)  	// Case where ul is on the top row
		return (*vect)[lr_x][lr_y] - (*vect)[ul_x-1][lr_y];
	
	else return (*vect)[lr_x][lr_y];	// Case where ul is (0,0)
}

// initialize the private vectors so that, for each color,  entry 
// (x,y) is the cumulative sum of the the color values from (0,0)
// to (x,y). Similarly, the sumSq vectors are the cumulative
// sum of squares from (0,0) to (x,y).
stats::stats(PNG & im)
{
	for (int x = 0; x < (int) im.width(); ++x)
	{
		int height = im.height();
		vector<long> sumRCol (height);
		vector<long> sumBCol (height);
		vector<long> sumGCol (height);
		vector<long> sumsqRCol (height);
		vector<long> sumsqGCol (height);
		vector<long> sumsqBCol (height);
												// Initialize column vectors and push_back into respective private vectors
		sumRed.push_back(sumRCol);				// Note that column vectors are initialized with size height
		sumGreen.push_back(sumGCol);
		sumBlue.push_back(sumBCol);
		sumsqRed.push_back(sumsqRCol);
		sumsqGreen.push_back(sumsqGCol);
		sumsqBlue.push_back(sumsqBCol);
		
		for (int y = 0; y < (int) im.height(); ++y)
		{			
			RGBAPixel* pixel = im.getPixel(x,y);
			
			if (x > 0 && y > 0)				// General case
			{
				sumRed[x][y] = sumRed[x-1][y] + sumRed[x][y-1] - sumRed[x-1][y-1] + (long) pixel->r;
				sumGreen[x][y] = sumGreen[x-1][y] + sumGreen[x][y-1] - sumGreen[x-1][y-1] + (long) pixel->g;
				sumBlue[x][y] = sumBlue[x-1][y] + sumBlue[x][y-1] - sumBlue[x-1][y-1] + (long) pixel->b;
				sumsqRed[x][y] = sumsqRed[x-1][y] + sumsqRed[x][y-1] - sumsqRed[x-1][y-1] + (long) pow((double) pixel->r, 2);
				sumsqGreen[x][y] = sumsqGreen[x-1][y] + sumsqGreen[x][y-1] - sumsqGreen[x-1][y-1] + (long) pow((double) pixel->g, 2);
				sumsqBlue[x][y] = sumsqBlue[x-1][y] + sumsqBlue[x][y-1] - sumsqBlue[x-1][y-1] + (long) pow((double) pixel->b, 2);
			}
			
			else if (x > 0 && y == 0)		// Row rectangle case
			{
				sumRed[x][y] = sumRed[x-1][y] + (long) pixel->r;
				sumGreen[x][y] = sumGreen[x-1][y] + (long) pixel->g;
				sumBlue[x][y] = sumBlue[x-1][y] + (long) pixel->b;
				sumsqRed[x][y] = sumsqRed[x-1][y] + (long) pow((double) pixel->r, 2);
				sumsqGreen[x][y] = sumsqGreen[x-1][y] + (long) pow((double) pixel->g, 2);
				sumsqBlue[x][y] = sumsqBlue[x-1][y] + (long) pow((double) pixel->b, 2);
			}
			
			else if (x == 0 && y > 0)		// Column rectangle case
			{
				sumRed[x][y] = sumRed[x][y-1] + (long) pixel->r;
				sumGreen[x][y] = sumGreen[x][y-1] + (long) pixel->g;
				sumBlue[x][y] = sumBlue[x][y-1] + (long) pixel->b;
				sumsqRed[x][y] = sumsqRed[x][y-1] + (long) pow((double) pixel->r, 2);
				sumsqGreen[x][y] = sumsqGreen[x][y-1] + (long) pow((double) pixel->g, 2);
				sumsqBlue[x][y] = sumsqBlue[x][y-1] + (long) pow((double) pixel->b, 2);
			}
			
			else							// Single pixel case
			{
				sumRed[x][y] = (long) pixel->r;
				sumGreen[x][y] = (long) pixel->g;
				sumBlue[x][y] = (long) pixel->b;
				sumsqRed[x][y] = (long) pow((double) pixel->r, 2);
				sumsqGreen[x][y] = (long) pow((double) pixel->g, 2);
				sumsqBlue[x][y] = (long) pow((double) pixel->b, 2);
			}
		}
	}
}

// given a rectangle, compute its sum of squared deviations from 
	// mean, over all color channels. Will be used to make split when
// building tree.
/* @param ul is (x,y) of the upper left corner of the rectangle 
* @param lr is (x,y) of the lower right corner of the rectangle */
long stats::getScore(pair<int,int> ul, pair<int,int> lr) {
	// cout << "getScore: " << ul.first << "," << ul.second << " " << lr.first << "," << lr.second << " " << endl;
	return getScoreHelper('r', ul, lr) + getScoreHelper('g', ul, lr) + getScoreHelper('b', ul, lr);
}

// Private helper function for getScore
long stats::getScoreHelper(char channel, pair<int,int> ul, pair<int,int> lr) {
	return getSumSq(channel, ul, lr) - ((getSum(channel, ul, lr) * getSum(channel, ul, lr)) / rectArea(ul, lr));
}

// given a rectangle, return the average color value over the 
// rectangle as a pixel.
/* Each color component of the pixel is the average value of that 
* component over the rectangle.
* @param ul is (x,y) of the upper left corner of the rectangle 
* @param lr is (x,y) of the lower right corner of the rectangle */
RGBAPixel stats::getAvg(pair<int,int> ul, pair<int,int> lr) {
	long area = rectArea(ul, lr);
	long meanRed = getSum('r', ul, lr) / area;
	long meanGreen = getSum('g', ul, lr) / area;
	long meanBlue = getSum('b', ul, lr) / area;
	
	return RGBAPixel(meanRed, meanGreen, meanBlue);
}

// given a rectangle, return the number of pixels in the rectangle
/* @param ul is (x,y) of the upper left corner of the rectangle 
* @param lr is (x,y) of the lower right corner of the rectangle */
long stats::rectArea(pair<int,int> ul, pair<int,int> lr)
{
	long height = (long) (lr.second - ul.second + 1);
	long width = (long) (lr.first - ul.first + 1);
	return height * width;
}