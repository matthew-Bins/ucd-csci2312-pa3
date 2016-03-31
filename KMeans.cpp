#include"KMeans.h"
#include"Exceptions.h"
#include <assert.h>
#include <limits>

namespace Clustering {

	KMeans::KMeans(unsigned int dim, unsigned int k, std::string filename, unsigned int maxIter) :
	__dimensionality(dim), __k(k), __iFileName(filename), __maxIter(maxIter), __numIter(0),
	__numNonempty(1){
		
		if (k == 0) //checks for there to be at least one cluster
			throw ZeroClustersEx();
		
		std::ifstream file(__iFileName);//inialize the file

		if (!file.is_open()) 
			throw DataFileOpenEx(__iFileName);
		
		//make an array of clusters
		__clusters = new Cluster *[__k];
		//make an array of centroids
		__initCentroids = new Point *[__k];

		//fill the arrays
		for (int i = 0; i < __k; ++i) {
			__clusters[i] = new Cluster(dim);
			__initCentroids[i] = new Point(dim);
		}

		file >> *__clusters[0];
		file.close();

		__clusters[0]->pickCentroids(__k, __initCentroids);

	}

	KMeans::~KMeans() {
		//delete the array of clusters
		for (int i = 0; i < __k; ++i)
			delete __clusters[i];
		delete[] __clusters;
		//delete the array of centriods
		for (int i = 0; i < __k; ++i)
			delete __initCentroids[i];
		delete[] __initCentroids;

	}

	unsigned int KMeans::getMaxIter() { return __maxIter; }
	unsigned int KMeans::getNumIters() { return __numIter; }
	unsigned int KMeans::getNumNonemptyClusters() { return __numNonempty; }
	unsigned int KMeans::getNumMovesLastIter() { return __numMovesLastIter; }

	Cluster& KMeans::operator[](unsigned int u) { return *__clusters[u]; }
	const Cluster& KMeans::operator[](unsigned int u) const{ return *__clusters[u]; }

	std::ostream& operator<<(std::ostream& os, const KMeans& kmeans) {
		for (int i = 0; i < kmeans.__k; ++i)
			os << kmeans[i];

		return os;
	}

	void KMeans::run() {
		/*for (int i = 0; i < __k; ++i)
			assert(__initCentroids[i]->getValue(i) == NULL);*/

		int moves = 100, iter = 0, closest = NULL;
		double d1, d2, Final;

		for (int i = 1; i < __k; ++i) {
			Cluster::Move move(*(__initCentroids[i]), *(__clusters[0]), *(__clusters[i]));
			move.perform();
		}

		while (moves > 0 && iter < __maxIter) {
			moves = 0;
			for (int i = 0; i < __k; ++i) {//for whole system
				for (int c = 0; c < __clusters[i]->getSize(); ++c) {
					d1 = (*__clusters[i])[c].distanceTo(__clusters[i]->centroid.get());

					for (int c2 = 1; c2 < __k; ++c2) {
						d2 = (*__clusters[i])[c].distanceTo(__clusters[c2]->centroid.get());
						if (d1 > d2) {
							closest = i;
							Final = d2;
						}
					}
					if (closest != NULL) {
						Cluster::Move move((*(__clusters[i]))[c], *(__clusters[i]), *(__clusters[closest]));
						move.perform();
						++moves;
					}

				}
			}

			for (int i = 0; i < __k; ++i)
					__clusters[i]->centroid.compute();
			++iter;
		}

		__numNonempty = 0;
		for (int i = 0; i < __k; ++i)
			if (__clusters[i]->getSize() > 0)
				++__numNonempty;

		__numMovesLastIter = moves;
		__numIter = iter;

	}//end of run

}