#include"Cluster.h"
#include"Exceptions.h"
#include <sstream>

namespace Clustering {
	unsigned int Cluster::__idGenerator = 0;
	const char Cluster::POINT_CLUSTER_ID_DELIM = ':';

	LNode::LNode(const Point& p, LNodePtr n) : point(p), next(n) {}

	void Cluster::__del() {
		LNodePtr TheEnd = __points, next;

		for (int i = 0; i < __size; ++i) {
			next = TheEnd->next;
			delete TheEnd;
			TheEnd = next;
		}
	}

	void Cluster::__cpy(LNodePtr pts) {
		if (pts == nullptr) {
			__points = nullptr;
			return;
		}
			
		LNodePtr temp = pts;
		LNode *copy = new LNode(pts->point, nullptr);

		LNode *head = copy;
		temp = temp->next;

		while (temp != nullptr) {
			copy = copy->next = new LNode(temp->point, nullptr);
			temp = temp->next;
		}

		__points = head;
	}

	bool Cluster::___in(const Point &p) const { return true; }

	Cluster::Cluster(unsigned int d) : centroid(d, *this) {
		__dimensionality = d;
		__size = 0;
		__points = nullptr;
		__id = __idGenerator++;
	}

	Cluster::Cluster(const Cluster& c) : Cluster(c.getDimensionality()) {
		__id = c.__id;

		for (int i = 0; i < c.getSize(); ++i)
			add(c[i]);

		centroid.compute();
	}

	Cluster& Cluster::operator=(const Cluster& copy) { 
		if (*this == copy) {
			__id = copy.__id;
			return *this;
		}
		else {
			if (__points != nullptr)
				__del();

			__size = 0;
			__id = copy.__id;

			for (int i = 0; i < copy.getSize(); ++i)
				add(copy[i]);

			centroid.compute();
		}
		return *this;
	}

	Cluster::~Cluster() { __del(); }

	//getters
	unsigned int Cluster::getSize() const { return __size; }
	unsigned int Cluster::getDimensionality() const { return __dimensionality; }
	unsigned int Cluster::getId() const { return __id; }

	void Cluster::add(const Point& p) {
		if (__dimensionality != p.getDims())
			throw DimensionalityMismatchEx(__dimensionality, p.getDims());

		LNode* add = new LNode(p, nullptr);
		LNodePtr temp = __points, prev = nullptr;

		if (__size == 0) {//empty
			__points = add;
			++__size;
			centroid.setValid(false);
			return;
		}
		else {//has points

			while (temp != nullptr) {
				if (p < temp->point) {
					if (prev == nullptr) {//smaller than first point
						add->next = temp;
						__points = add;
					}
					else {//middle of list
						prev->next = add;
						add->next = temp;
					}
					++__size;
					centroid.setValid(false);
					return;
				}
				prev = temp;
				temp = temp->next;
			}

			prev->next = add;
			++__size;
			centroid.setValid(false);
			return;
		}
	}

	const Point& Cluster::remove(const Point& p) {
		if (__dimensionality != p.getDims())
			throw DimensionalityMismatchEx(__dimensionality, p.getDims());

		int i = 0;
		bool found = false;
		LNodePtr temp = __points, prev = nullptr;

		if (temp == nullptr) {//no points in cluster
			return p;
		}

		if (temp->next == nullptr) {
			if (temp->point == p) {
				__size = 0;
				delete temp;
				__points = nullptr;
			}
		}
		else {
			prev = __points;
			temp = __points->next;

			if (__points->point == p) {
				__points = temp;
				--__size;
				delete prev, temp;
				centroid.setValid(false);
				return p;
			}

			for (; !found && temp != nullptr; temp = temp->next) {
				if (temp->point == p) {
					prev->next = temp->next;
					--__size;
					found = true;
					delete temp;
					centroid.setValid(false);
					return p;
				}

				prev = temp;
			}
		}

		centroid.setValid(false);
		return p;
	}

	bool Cluster::contains(const Point &p) const {
		bool there = false;
		LNodePtr temp = __points;

		while (temp != nullptr) {
			if (temp->point == p)
				there = true;

			temp = temp->next;
		}

		return there;
	}

	void Cluster::pickCentroids(unsigned int k, Point **pointArray) {
		if (k >= __size) {//k is bigger than size
			for (int i = 0; i < k; ++i) {
				if (i < __size)
					*pointArray[i] = (*this)[i];
				else
					for (unsigned int d = 0; d < pointArray[i]->getDims(); ++d)
						pointArray[i]->setValue(d, std::numeric_limits<double>::max());
			}
		}
		else {//size is bigger than k
			float place = static_cast<float>(__size) / k;

			for (int i = 0; i < k; ++i) {
				*pointArray[i] = (*this)[static_cast<int>(i * place)];
			}
		}

	}

	const Point& Cluster::operator[](unsigned int u) const {
		if (__size == 0)
			throw EmptyClusterEx();

		if (u >= __size)
			throw OutOfBoundsEx(__size, u);

		LNodePtr temp = __points;
		if (__points != nullptr) {
			for (int i = 0; i < u; ++i)
				temp = temp->next;
		}
		return temp->point;
	}

	///friend functions///
	std::ostream& operator<<(std::ostream& os, const Cluster& c) {
		int size = c.getSize();

		for (int i = 0; i < size; ++i)
			os << c[i] << Cluster::POINT_CLUSTER_ID_DELIM << c.getId() << std::endl;

		return os;
	}

	std::istream& operator>>(std::istream& in, Cluster& c) {
		int size;
		std::string fline, line;


		while (!in.eof()) {

			std::getline(in, line);
			if (line == "")
				break;

			size = std::count(line.begin(), line.end(), Point::POINT_VALUE_DELIM) + 1;
			std::stringstream ss(line);

			Point p(size);
			int temp = p.id();
			try {
				ss >> p;
				c.add(p);
			}
			catch (DimensionalityMismatchEx &e) {
				std::cerr << "Caught exception: " << e << std::endl;
				int temp = p.id();
				p.rewindIdGen();
			}
		}
		return in;
	}

	bool operator==(const Cluster& lhs, const Cluster& rhs) {
		if (lhs.getDimensionality() != rhs.getDimensionality())
			throw DimensionalityMismatchEx(lhs.getDimensionality(), rhs.getDimensionality());

		if (lhs.getSize() != rhs.getSize())
			return false;

		for (int i = 0; i < lhs.getSize(); ++i)
			if (lhs[i] != rhs[i])
				return false;

		return true;
	}

	bool operator!=(const Cluster& lhs, const Cluster& rhs) { return !(lhs == rhs); }

	////members again///
	//operations with points
	Cluster& Cluster::operator+=(const Point& rhs) {
		if (__dimensionality != rhs.getDims())
			throw DimensionalityMismatchEx(__dimensionality, rhs.getDims());

		if (this->contains(rhs))
			return *this;

		add(rhs);
		return *this;
	}

	Cluster& Cluster::operator-=(const Point& rhs) {
		if (__dimensionality != rhs.getDims())
			throw DimensionalityMismatchEx(__dimensionality, rhs.getDims());

		if (!this->contains(rhs))
			return *this;

		remove(rhs);
		return *this;
	}

	//operations with clusters
	Cluster& Cluster::operator+=(const Cluster& rhs) {
		if (__dimensionality != rhs.getDimensionality())
			throw DimensionalityMismatchEx(__dimensionality, rhs.getDimensionality());

		for (int i = 0; i < rhs.getSize(); ++i) {
			if (!this->contains(rhs[i]))
				this->add(rhs[i]);
		}

		return *this;
	}

	Cluster& Cluster::operator-=(const Cluster& rhs) {
		if (__dimensionality != rhs.getDimensionality())
			throw DimensionalityMismatchEx(__dimensionality, rhs.getDimensionality());

		for (int i = 0; i < rhs.getSize(); ++i) {
			if (this->contains(rhs[i]))
				this->remove(rhs[i]);
		}

		return *this;
	}

	//with clusters and clusters
	const Cluster operator+(const Cluster& lhs, const Cluster& rhs) { return Cluster(lhs) += rhs; }
	const Cluster operator-(const Cluster& lhs, const Cluster& rhs) { return Cluster(lhs) -= rhs; }

	//with clusters and points
	const Cluster operator+(const Cluster& lhs, const Point& rhs) { return Cluster(lhs) += rhs; }
	const Cluster operator-(const Cluster& lhs, const Point& rhs) { return Cluster(lhs) -= rhs; }

	//////////////////////////////////////////////////////////////////////////////////////
	/*        Centroid class Implementation                                             */
	//////////////////////////////////////////////////////////////////////////////////////
	Cluster::Centroid::Centroid(unsigned int d, const Cluster &c) :
		__dimensions(d), __c(c), __p(d) {
		__valid = false; //change before turning in delete  one '_'
		if (__c.getSize() == 0)
			toInfinity();
	}

	const Point Cluster::Centroid::get() const { return __p; }

	void Cluster::Centroid::set(const Point& p) {
		__p = p;
		__valid = true; //change before turning in delete  one '_'
	}

	bool Cluster::Centroid::isValid() const { return __valid; } // change valid
	void Cluster::Centroid::setValid(bool valid) { __valid = valid; } //change valid

	void Cluster::Centroid::compute() {
		int size = __c.getSize(), dim = __dimensions;
		double average = 0, total = 0;

		if (size == 1) {
			__p = __c[0];
		}
		else if (dim > 0 && size > 0) {

			for (int i = 0; i < dim; ++i) {
				for (int k = 0; k < size; ++k) {
					Point temp(__c[k]);
					total += temp[i];
				}
				average = total / size;
				__p.setValue(i, average);
				total = 0;
			}
		}
		else
			toInfinity();

		setValid(true);
	}

	bool Cluster::Centroid::equal(const Point& p) const {
		for (int i = 0; i < __dimensions; ++i)
			if (__p[i] != p[i])
				return false;

		return true;
	}

	void Cluster::Centroid::toInfinity() {
		for (int i = 0; i < __dimensions; ++i)
			__p[i] = std::numeric_limits<double>::max();
	}


	//////////////////////////////////////////////////////////////////////////////////////
	/*        move class Implementation                                                 */
	//////////////////////////////////////////////////////////////////////////////////////

	Cluster::Move::Move(const Point& p, Cluster& from, Cluster& to) :
	__p(p), __from(from), __to(to) {}

	void Cluster::Move::perform() {
		if (__from.contains(__p))
			__to.add(__from.remove(__p));

		if (__from.getSize() == 0)
			__from.centroid.toInfinity();

		if (__to.getSize() == 0)
			__to.centroid.toInfinity();
	}

}