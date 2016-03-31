#include"Exceptions.h"
#include<string>

namespace Clustering {

	//////////////////////////////////////out of bounds ex/////////////////
	OutOfBoundsEx::OutOfBoundsEx(unsigned int c, int r) : __current(c), __rhs(r) {
		__name = "OutOfBoundsEx";
	}

	unsigned int OutOfBoundsEx::getCurrent() const { return __current; }
	int OutOfBoundsEx::getRhs() const { return __rhs; }
	std::string OutOfBoundsEx::getName() const { return __name; }

	std::ostream &operator<<(std::ostream& os, const OutOfBoundsEx& ex) {
		return os << ex.getName() << ": " << ex.getCurrent() << ", " << ex.getRhs();
	}

	///////////////////////////////dimensionality mismatch ex/////////////////////////
	DimensionalityMismatchEx::DimensionalityMismatchEx(unsigned int c, unsigned int r) :
		__current(c), __rhs(r) {
		__name = "DimensionalityMismatchEx";
	}

	unsigned int DimensionalityMismatchEx::getCurrent() const { return __current; }
	unsigned int DimensionalityMismatchEx::getRhs() const { return __rhs; }
	std::string DimensionalityMismatchEx::getName() const { return __name; }

	std::ostream &operator<<(std::ostream &os, const DimensionalityMismatchEx &ex) {
		return os << ex.getName() << ": " << ex.getCurrent() << ", " << ex.getRhs();
	}

	////////////////////zero cluster ex//////////////////////////
	ZeroClustersEx::ZeroClustersEx() {
		__name = "ZeroClustersEx";
	}

	std::string ZeroClustersEx::getName() const { return __name; }

	std::ostream &operator<<(std::ostream &os, const ZeroClustersEx &ex) {
		return os << ex.getName();
	}

	/////////////////////////////////data file open ex//////////////////////
	DataFileOpenEx::DataFileOpenEx(std::string filename){
		__filename = filename;
		__name = "DataFileOpenEx";
	}

	std::string DataFileOpenEx::getFilename() const { return __filename; }
	std::string DataFileOpenEx::getName() const { return __name; }

	std::ostream &operator<<(std::ostream &os, const DataFileOpenEx &ex) {
		return os << ex.getName() << ": " << ex.getFilename();
	}

	//////////////////////////////Zero Dimension Ex///////////////////////////
	ZeroDimensionsEx::ZeroDimensionsEx() {
		__name = "ZeroDimensionsEx";
	}

	std::string ZeroDimensionsEx::getName() const { return __name; }

	std::ostream &operator<<(std::ostream &os, const ZeroDimensionsEx& ex) {
		return os << ex.getName();
	}

	/////////////////////////////Empty Cluster Ex////////////////////////////////
	EmptyClusterEx::EmptyClusterEx() {
		__name = "EmptyClusterEx";
	}

	std::string EmptyClusterEx::getName() const { return __name; }

	std::ostream &operator<<(std::ostream &os, const EmptyClusterEx &ex) {
		return os << ex.getName();
	}
	///////////////////////////////end//////////////////////////////////
}