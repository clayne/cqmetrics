/*-
 * Copyright 2014 Diomidis Spinellis
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */

#ifndef DESCRIPTIVE_H
#define DESCRIPTIVE_H

#include <algorithm>
#include <cmath>
#include <limits>
#include <ostream>
#include <vector>

#include "math.h"

/** Maintain simple descriptive statistics values */
template <typename T>
class Descriptive {
private:
	T sum;
	T max;			// Required; we get min by partial_sort
	mutable std::vector <T> values;	// Required for median
	/*
	 * These are used for calculating the standard deviation from the
	 * running values; see:
	 * https://en.wikipedia.org/wiki/Standard_deviation#Rapid_calculation_methods
	 */
	double a, q;
	mutable bool is_sorted;

	/** Sort the first half of the values to obtain median and min */
	void sort() const {
		if (is_sorted)
			return;
		partial_sort(values.begin(), values.begin() +
				values.size() / 2 + 1, values.end());
		is_sorted = true;
	}
public:
	Descriptive() :
		sum(0),
		max(std::numeric_limits<T>::min()),
		a(0), q(0), is_sorted(true)
	{}

	T get_sum() const {
		return (sum);
	}

	int get_count() const {
		return (values.size());
	}

	T get_min() const {
		sort();
		return (values.front());
	}

	T get_max() const {
		return (max);
	}

	/** Return the artihmetic mean of the measured value */
	double get_mean() const {
		return ((double)sum / values.size());
	}

	/** Return the median of the measured value */
	double get_median() const {
		if (values.size() == 0)
			return nan("");
		sort();
		if (values.size() % 2 == 0)
			return (values[values.size() / 2] + values[values.size() / 2 + 1]) / 2.0;
		else
			return values[values.size() / 2];
	}

	void add(T v) {
		values.push_back(v);
		is_sorted = false;
		sum += v;
		if (v > max)
			max = v;

		// Running variance
		double a_prev = a;
		a = a + (v - a) / (double)values.size();
		q = q + (v - a_prev) * (v - a);
	}

	/** Return the population's standard deviation. */
	double get_standard_deviation(void) const {
		/*
		 * The standard deviation of an empty population is not defined,
		 * but for measuring quality, 0, is a reasonable value.
		 */
		return values.size() ? sqrt(q / values.size()) : nan("");
	}
};

template <typename T>
std::ostream&
operator <<(std::ostream& o, const Descriptive<T> &d) {
	if (d.get_count() != 0)
		o << d.get_count() << '\t' << d.get_min() << '\t' <<
			d.get_mean() << '\t' << d.get_max() << '\t' <<
			d.get_standard_deviation();
	else
		o << "0\t\t\t\t";
	return o;
}
#endif /* DESCRIPTIVE_H */
