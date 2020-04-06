
#include "stats.h"
#include <cmath>


/**
 * Initialize the private vectors so that for each color, entry 
 * (x,y) is the cumulative sum of the the color values from (0,0)
 * to (x,y).
 * Similarly, the sumSq vectors are the cumulative sum of squares
 * from (0,0) to (x,y).
 */
stats::stats(PNG & im){
	//initialize vectors
	sumRed.resize(im.height(), vector<long> (im.width(),0));
	sumGreen.resize(im.height(), vector<long> (im.width(),0));
	sumBlue.resize(im.height(), vector<long> (im.width(),0));
	sumsqRed.resize(im.height(), vector<long> (im.width(),0));
	sumsqGreen.resize(im.height(), vector<long> (im.width(),0));
	sumsqBlue.resize(im.height(), vector<long> (im.width(),0));

	// iterate through all pixels, set sums and sumsqrs
	for (unsigned int x = 0; x < im.width(); x++) {
		for (unsigned int y = 0; y < im.height(); y++) {
			RGBAPixel * pixel = im.getPixel(x,y);

			if ((y == 0) && (x == 0)) {
				sumRed[y][x] = pixel->r;
				sumGreen[y][x] = pixel->g;
				sumBlue[y][x] = pixel->b;
				sumsqRed[y][x] = pow(pixel->r, 2);
				sumsqGreen[y][x] = pow(pixel->g, 2);
				sumsqBlue[y][x] = pow(pixel->b, 2);
			} else if (x == 0) {
				sumRed[y][x] = sumRed[y-1][x] + pixel->r;
				sumGreen[y][x] = sumGreen[y-1][x] + pixel->g;
				sumBlue[y][x] = sumBlue[y-1][x] + pixel->b;
				sumsqRed[y][x] = sumsqRed[y-1][x] + pow(pixel->r, 2);
				sumsqGreen[y][x] = sumsqGreen[y-1][x] + pow(pixel->g, 2);
				sumsqBlue[y][x] = sumsqBlue[y-1][x] + pow(pixel->b, 2);
			} else if (y==0) {
				sumRed[y][x] = sumRed[y][x-1] + pixel->r;
				sumGreen[y][x] = sumGreen[y][x-1] + pixel->g;
				sumBlue[y][x] = sumBlue[y][x-1] + pixel->b;
				sumsqRed[y][x] = sumsqRed[y][x-1] + pow(pixel->r, 2);
				sumsqGreen[y][x] = sumsqGreen[y][x-1] + pow(pixel->g, 2);
				sumsqBlue[y][x] = sumsqBlue[y][x-1] + pow(pixel->b, 2);
			} else {
				sumRed[y][x] = sumRed[y][x-1] + sumRed[y-1][x] - sumRed[y-1][x-1] + pixel->r;
				sumGreen[y][x] = sumGreen[y][x-1] + sumGreen[y-1][x] - sumGreen[y-1][x-1] + pixel->g;
				sumBlue[y][x] = sumBlue[y][x-1] + sumBlue[y-1][x] - sumBlue[y-1][x-1] + pixel->b;
				sumsqRed[y][x] = sumsqRed[y][x-1] + sumsqRed[y-1][x] - sumsqRed[y-1][x-1] + pow(pixel->r, 2);
				sumsqGreen[y][x] = sumsqGreen[y][x-1] + sumsqGreen[y-1][x] - sumsqGreen[y-1][x-1] + pow(pixel->g, 2);
				sumsqBlue[y][x] = sumsqBlue[y][x-1] + sumsqBlue[y-1][x] - sumsqBlue[y-1][x-1] + pow(pixel->b, 2);
			}
		}
	}
}

/** 
 * Returns the sums of all pixel values across all color channels.
 * useful in computing the score of a rectangle
 * PA3 function
 * @param channel is one of r, g, or b
 * @param ul is (x,y) of the upper left corner of the rectangle 
 * @param lr is (x,y) of the lower right corner of the rectangle
 */
long stats::getSum(char channel, pair<int,int> ul, pair<int,int> lr)
{
	return getSumHelper(false, channel, ul, lr);
}

/** 
 * Returns the sums of squares of all pixel values across all color channels.
 * useful in computing the score of a rectangle
 * PA3 function
 * @param channel is one of r, g, or b
 * @param ul is (x,y) of the upper left corner of the rectangle 
 * @param lr is (x,y) of the lower right corner of the rectangle
 */
long stats::getSumSq(char channel, pair<int,int> ul, pair<int,int> lr)
{
	return getSumHelper(true, channel, ul, lr);
}

/**
 * Helper function for getSum and getSumSq
 * @param square if true, returns sumSq, if false returns Sum
 * @param channel is one of r, g, or b
 * @param ul is (x,y) of the upper left corner of the rectangle 
 * @param lr is (x,y) of the lower right corner of the rectangle
 */
long stats::getSumHelper(bool square, char channel, pair<int,int> ul, pair<int,int> lr)
{
	vector< vector< long >> s;

	if (square) {
		if (channel == 'r') {
        	s = sumsqRed;
    	} else if (channel == 'g') {
        	s = sumsqGreen;
    	} else if (channel == 'b') {
        	s = sumsqBlue;
    	}
		
	} else {
		if (channel == 'r') {
			s = sumRed;
		} else if (channel == 'g') {
			s = sumGreen;
		} else if (channel == 'b') {
			s = sumBlue;
		}
	}
    
	// getting sums
	if (ul.first == 0 && ul.second == 0) {
		return s[lr.second][lr.first];
	} else if (ul.second == 0) {
		return s[lr.second][lr.first] - s[lr.second][ul.first - 1];
	} else if (ul.first == 0) {
		return s[lr.second][lr.first] - s[ul.second - 1][lr.first];
	} else {
		return s[lr.second][lr.first] - s[ul.second - 1][lr.first] - s[lr.second][ul.first - 1] + s[ul.second - 1][ul.first - 1];
	}
}

long stats::rectArea(pair<int,int> ul, pair<int,int> lr){
// YOUR CODE HERE!!
	long area = (lr.first - ul.first + 1) * (lr.second - ul.second + 1);
	return area;
}

// given a rectangle, compute its sum of squared deviations from mean, over all color channels.
long stats::getScore(pair<int,int> ul, pair<int,int> lr){
// YOUR CODE HERE!!
	long rsq = getSumSq('r',ul,lr);
	long gsq = getSumSq('g',ul,lr);
	long bsq = getSumSq('b',ul,lr);

	long rsum = getSum('r',ul,lr);
	long gsum = getSum('g',ul,lr);
	long bsum = getSum('b',ul,lr);
	
	long area = rectArea(ul,lr);

    return rsq - (pow(rsum, 2) / area) + gsq - (pow(gsum, 2) / area) + bsq - (pow(bsum, 2) / area);

}
		
RGBAPixel stats::getAvg(pair<int,int> ul, pair<int,int> lr){
// YOUR CODE HERE!!
    long area = rectArea(ul, lr);

	long ravg = getSum('r',ul,lr)/area;
	long gavg = getSum('g',ul,lr)/area;
	long bavg = getSum('b',ul,lr)/area;

	return RGBAPixel(ravg,gavg,bavg);
}
