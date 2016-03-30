#include"Point.h"
#include"Exceptions.h"
#include <sstream>

namespace Clustering {

	unsigned int Point::__idGen = 0;
	const char Point::POINT_VALUE_DELIM = ',';

	void Point::rewindIdGen() { --__idGen; }

	Point::Point(unsigned int dim) {
		if (dim == 0)
			throw ZeroDimensionsEx();

		__id = __idGen++;
		__dim = dim;
		__values = new double[dim];

		for (int i = 0; i < __dim; ++i)
			setValue(i, 0.0);
	}

	Point::Point(unsigned int dim, double* val) {
		if (dim == 0)
			throw ZeroDimensionsEx();

		__dim = dim;
		__values = val;
		__id = __idGen++;
	}

	Point::Point(const Point& p) {
		__id = p.__id;
		__dim = p.getDims();
		__values = new double[__dim];

		for (int i = 0; i < __dim; ++i)
			__values[i] = p.__values[i];
	}

	Point& Point::operator=(const Point& p) {
		if (__dim != p.getDims())
			throw DimensionalityMismatchEx(__dim, p.getDims());

		__id = p.__id;
		__dim = p.getDims();
		for (int i = 0; i < __dim; ++i)
			__values[i] = p.getValue(i);

		return *this;
	}

	Point::~Point() { delete[] __values; }

	int Point::getId() const { return __id; }
	unsigned int Point::getDims() const { return __dim; }
	void Point::setValue(unsigned int i, double val) { 
		if (i >= __dim)
			throw OutOfBoundsEx(__dim, i);

		__values[i] = val; 
	}
	double Point::getValue(unsigned int i) const { 
		if (i >= __dim)
			throw OutOfBoundsEx(__dim, i);

		return __values[i]; 
	}

	double Point::distanceTo(const Point& p) const {
		double answer = 0;

		if (__dim != p.getDims())
			throw DimensionalityMismatchEx(__dim, p.getDims());

		for (int i = 0; i < __dim; ++i) {
			answer += pow((p.getValue(i) - __values[i]), 2);
		}

		return sqrt(answer);
	}

	Point& Point::operator*=(double num) {
		for (int i = 0; i < __dim; ++i)
			__values[i] *= num;

		return *this;
	}

	Point& Point::operator/=(double num) {
		for (int i = 0; i < __dim; ++i)
			__values[i] /= num;

		return *this;
	}

	const Point Point::operator*(double num) const { return Point(*this) *= num; }

	const Point Point::operator/(double num) const{ return Point(*this) /= num; }

	double& Point::operator[](unsigned int index) { 
		if (index >= __dim)
			throw OutOfBoundsEx(__dim, index);

		return __values[index]; 
	}

	const double& Point::operator[](unsigned int index) const {
		if(index >= __dim)
			throw OutOfBoundsEx(__dim, index);

		return __values[index];
	}

	Point &operator+=(Point& one, const Point& two) {
		if (one.getDims() != two.getDims())
			throw DimensionalityMismatchEx(one.getDims(), two.getDims());

		for (int i = 0; i < one.getDims(); ++i)
			one[i] += two.getValue(i);

		return one;
	}

	Point &operator-=(Point& one, const Point& two) {
		if (one.getDims() != two.getDims())
			throw DimensionalityMismatchEx(one.getDims(), two.getDims());

		for (int i = 0; i < one.getDims(); ++i)
			one[i] -= two.getValue(i);

		return one;
	}

	const Point operator+(const Point& one, const Point& two) {	return Point(one) += two;	}

	const Point operator-(const Point& one, const Point& two) { return Point(one) -= two; }

	bool operator==(const Point& one, const Point& two) {
		if (one.getDims() != two.getDims())
			throw DimensionalityMismatchEx(one.getDims(), two.getDims());

		bool same = true;
		if (one.getId() != two.getId()) {
			return false;
		}
		else if (one.__dim == two.__dim) {
			for (int i = 0; i < one.getDims(); ++i)
				if (one.getValue(i) != two.getValue(i))
					same = false;
		}
		else
			return false;

		return same;
	}

	bool operator!=(const Point& one, const Point& two) {	return !(one == two);	}

	bool operator<(const Point& one, const Point& two) {
		if (one.getDims() != two.getDims())
			throw DimensionalityMismatchEx(one.getDims(), two.getDims());

		for (int i = 0; i < one.getDims(); ++i)
			if (one.getValue(i) < two.getValue(i))
				return true;

		return false;
	}

	bool operator>(const Point& one, const Point& two) { return two < one; }
	bool operator<=(const Point& one, const Point& two) { return !(one > two); }
	bool operator>=(const Point& one, const Point& two) { return !(one < two); }

	std::ostream &operator<<(std::ostream& os, const Point& p) {
		int dis = p.getDims() - 1;

		for (int i = 0; i < dis; ++i)
			os << p.__values[i] << ", ";
		os << p.getValue(dis);

		return os;
	}

	std::istream &operator>>(std::istream& is, Point& p) {
		double point;
		int index = 0, size;
		std::string line, spoint;

		while (std::getline(is, line)) {

			size = std::count(line.begin(), line.end(), Point::POINT_VALUE_DELIM) + 1;

			if (p.getDims() != size)
				throw DimensionalityMismatchEx(p.getDims(), size);

			std::stringstream ss(line);
			while (std::getline(ss, spoint, Point::POINT_VALUE_DELIM)) {

				point = stod(spoint);
				p.setValue(index, point);
				++index;
			}
		}
		return is;
	}
}