#pragma once

// ================================
// My notstd::hive
// 
//               Created 2025.06.27
// ================================

#include <initializer_list>
#include <compare>
#include <memory>
#include <tyoe_traits>

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
public:
	using value_type = T;
	using allocator_type = Allocator;
	using pointer = typename std::allocator_traits<Allocator>::pointer;
	using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;
	using reference = value_type&;
	using const_reference = const value_type&;
	using size_type = size_t;
	using difference_type = ptrdiff_t;
	using iterator = _hive_iterator;
	using const_iterator = _hive_const_iterator;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
	using skipfield_type = typename std::conditional_t<sizeof(value_type) > 10, unsigned short, unsigned char>;
	
	struct _hive_memory_block;

	struct _hive_memory_block {
		std::unique_ptr<T> pBlock;
		std::unique_ptr<skipfield_type> pSkipfileds;
		std::unique_ptr<_hive_memory_block> pPrev = nullptr;
		std::unique_ptr<_hive_memory_block> pNext = nullptr;
	};

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
	template <class InputIt>
	hive(InputIt first, InputIt last, const Allocator& = Allocator());
	template <class InputIt>
	hive(InputIt first, InputIt last, hive_limits block_limits, const Allocator& = Allocator());
	template <std::_Container_compatible_range<T> R>
	hive(std::from_range_t, R&& rg, const Allocator& = Allocator());
	template <std::_Container_compatible_range<T> R>
	hive(std::from_range_t, R&& rg, hive_limits block_limits, const Allocator & = Allocator());
	hive(const hive& x);
	hive(const hive& x, const std::type_identity_t<Allocator>& alloc);
	hive(hive&&, const std::type_identity_t<Allocator>& alloc);
	hive(std::initializer_list<T> il, const Allocator& = Allocator());
	hive(std::initializer_list<T> il, hive_limits block_limits, const Allocator& = Allocator());

	~hive();
	hive& operator=(const hive& x);
	hive& operator=(hive&& x) noexcept(std::allocator_traits<Allocator>::propagate_on_container_move_assignment::value || std::allocator_traits<Allocator>::is_always_equal::value);
	hive& operator=(std::initializer_list<T>);
	template<class InputIterator>
	void assign(InputIterator first, InputIterator last);
	template<std::_Container_compatible_range<T> R>
	void assign_range(R&& rg);
	void assign(size_type n, const T& t);
	void assign(std::initializer_list<T>);
	allocator_type get_allocator() const noexcept;

	// iterators
	iterator				begin() noexcept;
	const_iterator			begin() const noexcept;
	iterator				end() noexcept;
	const_iterator			end() const noexcept;
	reverse_iterator		rbegin() noexcept;
	const_reverse_iterator	rbegin() const noexcept;
	reverse_iterator		rend() noexcept;
	const_reverse_iterator	rend() const noexcept;
	
	const_iterator			cbegin() const noexcept;
	const_iterator			cend() const noexcept;
	const_reverse_iterator	crbegin() const noexcept;
	const_reverse_iterator	crend() const noexcept;

	// capacity
	// based on [hive.capacity]
	bool empty() const noexcept;
	size_type size() const noexcept;
	size_type max_size() const noexcept;
	size_type capacity() const noexcept;
	void reserve(size_type n);
	void shrink_to_fit();
	void trim_capacity() noexcept;
	void trim_capacity(size_type n) noexcept;
	constexpr hive_limits block_capacity_limits() const noexcept;
	static constexpr hive_limits block_capacity_default_limits() noexcept;
	static constexpr hive_limits block_capacity_hard_limits() noexcept;
	void reshape(hive_limits block_limits);

	// modifiers
	// based on [hive.modifiers]
	template<class... Args> 
	iterator emplace(Args&&... args);
	template<class... Args> 
	iterator emplace_hint(const_iterator hint, Args&&... args);
	iterator insert(const T& x);
	iterator insert(T&& x);
	iterator insert(const_iterator hint, const T& x);
	iterator insert(const_iterator hint, T&& x);
	void insert(std::initializer_list<T> il);
	template<std::_Container_compatible_range<T> R>
	void insert_range(R&& rg);
	template<class InputIt>
	void insert(InputIt first, InputIt last);
	void insert(size_type n, const T& x);
	iterator erase(const_iterator position);
	iterator erase(const_iterator first, const_iterator last);
	void swap(hive&) noexcept(see below);
	void clear() noexcept;


	// operations
	// based on [hive.operations]
	void splice(hive& x);
	void splice(hive&& x);
	template<class BinaryPredicate = equal_to<T>>
	size_type unique(BinaryPredicate binary_pred = BinaryPredicate());

	template<class Compare = less<T>>
	void sort(Compare comp = Compare());

	iterator get_iterator(const_pointer p) noexcept;
	const_iterator get_iterator(const_pointer p) const noexcept;

private:
	hive_limits current_limits = {};

};

template<class InputIterator, class Allocator = std::allocator<std::_Iter_value_t<InputIterator>>>
hive(InputIterator, InputIterator, Allocator = Allocator())
-> hive<std::_Iter_value_t<InputIterator>, Allocator>;

template<class InputIterator, class Allocator = std::allocator<std::_Iter_value_t<InputIterator>>>
hive(InputIterator, InputIterator, hive_limits, Allocator = Allocator())
-> hive<std::_Iter_value_t<InputIterator>, Allocator>;

template<std::ranges::input_range R, class Allocator = std::allocator<std::ranges::range_value_t<R>>>
hive(std::from_range_t, R&&, Allocator = Allocator())
-> hive<std::ranges::range_value_t<R>, Allocator>;

template<std::ranges::input_range R, class Allocator = std::allocator<std::ranges::range_value_t<R>>>
hive(std::from_range_t, R&&, hive_limits, Allocator = Allocator())
-> hive<std::ranges::range_value_t<R>, Allocator>;