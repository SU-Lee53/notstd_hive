#pragma once

// ================================
// My std::hive
// 
//               Created 2025.06.27
// ================================


#include <memory>
#include <iterator>

class _hive_iterator_base {

};

class _hive_iterator : public _hive_iterator_base {

};

class _hive_const_iterator : public _hive_iterator_base {

};

struct hive_limits {
	size_t min;
	size_t max;
	constexpr hive_limits(size_t minimum, size_t maximum) noexcept
		: min{ minimum }, max{ maximum } { }
};

template<class T, class Allocator = std::allocator<T>>
class hive {
private:
	using _allocator_traits = Allocator

public:
	using value_type = T;
	using allocator_type = Allocator;
	using pointer = typename allocator_traits<Allocator>::pointer;
	using const_pointer = typename allocator_traits<Allocator>::const_pointer;
	using reference = value_type&;
	using const_reference = const value_type&;
	using size_type = size_t;
	using difference_type = ptrdiff_t;
	using iterator = _hive_iterator;
	using const_iterator = _hive_const_iterator;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public:
	// ctor, copy, destroy
	// based on [hive.cons]

	constexpr hive() noexcept(noexcept(Allocator())) : hive(Allocator()) {}
	constexpr explicit hive(const Allocator&) noexcept;
	constexpr explicit hive(hive_limits block_limits) : hive(block_limits, Allocator()) {}
	constexpr hive(hive_limits block_limits, const Allocator());
	explicit hive(size_type n, const Allocator& = Allocator());

	hive(size_type n, hive_limits block_limits, const Allocator & = Allocator());
	hive(size_type n, const T& value, const Allocator& = Allocator());
	hive(size_type n, const T& vlaue, hive_limits block_limits, const Allocator& = Allocator());

	template <std::input_iterator InputIt>
	hive(InputIt first, InputIt last, const Allocator& = Allocator());

	template <std::input_iterator InputIt>
	hive(InputIt first, InputIt last, hive_limits block_limits, const Allocator& = Allocator());






};

