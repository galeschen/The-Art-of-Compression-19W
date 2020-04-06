
#include "stats.h"
#include <cmath>


stats::stats(PNG & im){
// YOUR CODE HERE!!
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


long stats::getSum(char channel, pair<int,int> ul, pair<int,int> lr)
{
	// YOUR CODE HERE!!
	return getSumHelper(false, channel, ul, lr);
}

long stats::getSumSq(char channel, pair<int,int> ul, pair<int,int> lr)
{
	// YOUR CODE HERE!!
	return getSumHelper(true, channel, ul, lr);
}

long stats::getSumHelper(bool square, char channel, pair<int,int> ul, pair<int,int> lr)
{
	// added helper fn
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
	return (lr.first - ul.first + 1) * (lr.second - ul.second + 1);
}

// given a rectangle, compute its sum of squared deviations from mean, over all color channels.
long stats::getScore(pair<int,int> ul, pair<int,int> lr){
// YOUR CODE HERE!!
	long redSq = getSumSq('r',ul,lr);
	long greenSq = getSumSq('g',ul,lr);
	long blueSq = getSumSq('b',ul,lr);

	long redSum = getSum('r',ul,lr);
	long greenSum = getSum('g',ul,lr);
	long blueSum = getSum('b',ul,lr);
	
	long area = rectArea(ul,lr);

	long redScore = redSq - (pow(redsum, 2) / area);
	long greenScore = greenSq - (pow(greensum, 2) / area);
	long blueScore = blueSq - (pow(bluesum, 2) / area);

    return redScore + greenScore + blueScore;

}
		
RGBAPixel stats::getAvg(pair<int,int> ul, pair<int,int> lr){
// YOUR CODE HERE!!
    long area = rectArea(ul, lr);

	long redAvg = getSum('r',ul,lr)/area;
	long greenAvg = getSum('g',ul,lr)/area;
	long blueAvg = getSum('b',ul,lr)/area;

	return RGBAPixel(redAvg,greenAvg,blueAvg);
}
