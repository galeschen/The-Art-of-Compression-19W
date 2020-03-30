
/**
 *
 * twoDtree (pa3)
 * slight modification of a Kd tree of dimension 2.
 * twoDtree.cpp
 * This file will be used for grading.
 *
 */

#include "twoDtree.h"
#include <climits>
#include <cmath>
#include <map>

using std::map;

// Node constructor, given.
twoDtree::Node::Node(pair<int,int> ul, pair<int,int> lr, RGBAPixel a)
	:upLeft(ul),lowRight(lr),avg(a),left(NULL),right(NULL)
	{}

// twoDtree destructor, given.
twoDtree::~twoDtree(){
	clear();
}

// twoDtree copy constructor, given.
twoDtree::twoDtree(const twoDtree & other) {
	copy(other);
}


// twoDtree assignment operator, given.
twoDtree & twoDtree::operator=(const twoDtree & rhs){
	if (this != &rhs) {
		clear();
		copy(rhs);
	}
	return *this;
}

twoDtree::twoDtree(PNG & imIn){ 
// YOUR CODE HERE!!
	cout << "building tree" << endl;
	stats treestats = stats(imIn);
	width = imIn.width();
	height = imIn.height();
	pair<int, int> ul = make_pair(0, 0);
	pair<int, int> lr = make_pair(width - 1, height - 1);

	root = buildTree(treestats, ul, lr, true);
	cout << "built tree" << endl;
}

twoDtree::Node * twoDtree::buildTree(stats & s, pair<int,int> ul, pair<int,int> lr, bool vert)
{
// YOUR CODE HERE!!
	// creating new child node to be returned to caller function
	cout << "new node (" << ul.first << ", " << ul.second <<"), (" << lr.first << ", " << lr.second << ")" << endl;
	RGBAPixel avgpixel = s.getAvg(ul, lr);
	Node* currentnode = new Node(ul, lr, avgpixel);
	
	// case where we have one pixel rectangle
	if (ul.first == lr.first && ul.second == lr.second) {
		return currentnode;
	}

	// variances that we will be comparing to decide where to partition
	long var1;
	long var2;

	// minimum variance starts as max so any calc var is lower than it
	long minvar = LONG_MAX;

	// We cut vert if we are supposed to and can, or can't cut horizontally
	if ((vert && (ul.first != lr.first)) || (lr.second == ul.second)) {
		int vcut = ul.first;
		// x-coord of the left side of the partition
		for (int x = ul.first; x < lr.first; x++) {
			// for each partition, calculate variances
			var1 = s.getScore(ul, pair<int,int>(x, lr.second));
			var2 = s.getScore(pair<int,int>(x + 1, ul.second), lr);

			// if variance sums <= minvar, set as minvar, update partition
			if (var1 + var2 <= minvar) {
				minvar = var1 + var2;
				vcut = x;
			}
		}
		// Flip vert, build subtrees
		currentnode -> left = buildTree(s, ul, pair<int, int>(vcut, lr.second), !vert);
		currentnode -> right = buildTree(s, pair<int, int>(vcut + 1, ul.second), lr, !vert);

	// We cut horiz if we are supposed to and can, or can't cut vert
	} else if ((!vert && (ul.second != lr.second)) || (ul.first == lr.first)) {
		int hcut;
		// y-coord of the upper side of the partition
		for (int y = ul.second; y < lr.second; y++) {
			// for each partition, calculate variances
			var1 = s.getScore(ul, pair<int, int>(lr.first, y));
			var2 = s.getScore(pair<int, int>(ul.first, y + 1), lr);

			// if variance sums < minvar, set as minvar, update partition, and flip vert, build subtrees
			if (var1 + var2 < minvar) {
				minvar = var1 + var2;
				hcut = y;
			}
		}
		currentnode -> left = buildTree(s, ul, pair<int, int>(lr.first, hcut), !vert);
		currentnode -> right = buildTree(s, pair<int, int>(ul.first, hcut + 1), lr, !vert);
	}
	return currentnode;
}

/**
 * Render returns a PNG image consisting of the pixels
 * stored in the tree. It may be used on pruned trees. Draws
 * every leaf node's rectangle onto a PNG canvas using the 
 * average color stored in the node.
 */
PNG twoDtree::render(){
// YOUR CODE HERE!!
	PNG image = PNG(width,height);
	render(root, image);
	return image;
}

void twoDtree::render(Node* currentnode, PNG & image){
// added helper fn
	if (currentnode -> left == NULL && currentnode -> right == NULL) {
		for (int x = currentnode->upLeft.first; x < currentnode->lowRight.first + 1; x++) {
			for (int y = currentnode->upLeft.second; y < currentnode->lowRight.second + 1; y++) {
				*(image.getPixel(x,y)) = currentnode->avg;
				}
		}
	} else {
		render(currentnode->left, image);
		render(currentnode->right, image);
	}
}


/**
 * The idealPrune function can be considered to be the inverse
 * of the pruneSize function. It takes as input a number of leaves
 * and returns the minimum tolerance that would produce that resolution
 * upon a prune. It does not change the structure of the tree.
 */
int twoDtree::idealPrune(int leaves)
{
// YOUR CODE HERE!!
	int lowerBound = 1;
	int upperBound = 2;
	int mid;
	int midPrune;
	if (leaves == pruneSize(0)) {
		return 0;
	}
	if (leaves == pruneSize(1)) {
		return 1;
	}
	while (pruneSize(upperBound) > leaves) {
		lowerBound = upperBound;
		upperBound = upperBound*2;
	}
	while (upperBound > lowerBound) {
		mid = (lowerBound + upperBound) / 2;
		midPrune = pruneSize(mid);
		if (leaves >= midPrune) {
			upperBound = mid;
			cout << "case1 (upper: " << upperBound  << " , mid: " << mid << " , lower: " << lowerBound << ")" << endl;
		} else if (leaves < midPrune) {
			lowerBound = (mid + 1);
			cout << "case2 (upper: " << upperBound  << " , mid: " << mid << " , lower: " << lowerBound << ")" << endl;
		}
	}
	return lowerBound;
}

int twoDtree::pruneSize(int tol){
// YOUR CODE HERE!!
	return pruneSizeHelper(root, tol);
}

int twoDtree::pruneSizeHelper(Node* currentnode, int tol) {
// added helper fn
	if (currentnode == NULL) {
		return 0;
	} else if (prunehelper(currentnode, tol, currentnode->avg)) {
		return 1;
	} else {
		return (pruneSizeHelper(currentnode->left, tol)
				+ pruneSizeHelper(currentnode->right, tol));
	}
}

/**
 *  Prune function trims subtrees as high as possible in the tree.
 *  A subtree is pruned (cleared) if all of its leaves are within 
 *  tol of the average color stored in the root of the subtree. 
 *  Pruning criteria should be evaluated on the original tree, not 
 *  on a pruned subtree. (we only expect that trees would be pruned once.)
 *  
 * You may want a recursive helper function for this one.
 */
void twoDtree::prune(int tol){
// YOUR CODE HERE!!
	if (root !=NULL) {
		prune(root->left, tol);
		prune(root->right, tol);
	}
}

void twoDtree::prune(Node* currentnode, int tol) {
// added helper fn
	if (currentnode != NULL) {
		if (prunehelper(currentnode, tol, currentnode->avg)) {
			Node* ogleft = currentnode->left;
			Node* ogright = currentnode->right;
			currentnode->left = NULL;
			currentnode->right = NULL;
			clear(ogleft);
			clear(ogright);
		} else {
			prune(currentnode->left, tol);
			prune(currentnode->right, tol);
		}
	}
}

bool twoDtree::prunehelper(Node* currentnode, int tol, RGBAPixel & avg) {
// added helper fn
	if (currentnode->left == NULL && currentnode->right == NULL) {
		return (dist(currentnode->avg, avg) <= tol);
	} else {
		bool left = prunehelper(currentnode->left, tol, avg);
		bool right = prunehelper(currentnode->right, tol, avg);
		return (left && right);
	}
}

double twoDtree::dist(RGBAPixel p1, RGBAPixel p2) {
// added helper fn
	return pow(p1.r - p2.r, 2) + pow(p1.b - p2.b, 2) + pow(p1.g - p2.g, 2);
}

void twoDtree::clear() {
// YOUR CODE HERE!!!
	clear(root);
	root = NULL;
}

void twoDtree::clear(Node *current){
// added helper fn
	if (current != NULL) {
		clear(current->left);
		clear(current->right);
		delete current;
		current=NULL;
	}
}

void twoDtree::copy(const twoDtree & orig){
// YOUR CODE HERE!!
	width = orig.width;
	height = orig.height;
	this->root = copy(orig.root);
}

twoDtree::Node * twoDtree::copy(Node* orig){
// added helper fn
	if (orig == NULL){
		return NULL;
	}
	
	Node* newNode = new Node(orig->upLeft, orig->lowRight, orig->avg);
	newNode->left = copy(orig->left);
	newNode->right = copy(orig->right);
	
	return newNode;
}
