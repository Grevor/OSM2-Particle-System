#ifndef CURVE_H_
#define CURVE_H_

namespace Curves {

/**
 * A Curve should be seen as a graph which takes x-values and outputs y-values.
 */
template<typename Domain, typename Range>
class Curve {
public:
	/**
	 * Gets the value of this curve at the specified point.
	 * @param value The passed value.
	 * @return
	 * The value in the range of this curve which corresponds to the input.
	 */
	virtual Range getValue(Domain value) = 0;

	virtual ~Curve() {}
};

}

#endif /* CURVE_H_ */
