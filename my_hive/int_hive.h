#pragma once

// ================================
// My notstd::int_hive
// for implement testing
// 
//               Created 2025.06.27
// ================================


#include <initializer_list>
#include <compare>
#include <memory>
#include <type_traits>

#ifdef _DEBUG
#include <cassert>
#endif

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
		: min{ minimum }, max{ maximum } {
	}
};


class int_hive {
public:
	using value_type = int;
	using allocator_type = std::allocator<int>;
	using pointer = typename std::allocator_traits<allocator_type>::pointer;
	using const_pointer = typename std::allocator_traits<allocator_type>::const_pointer;
	using reference = value_type&;
	using const_reference = const value_type&;
	using size_type = size_t;
	using difference_type = ptrdiff_t;
	using iterator = _hive_iterator;
	using const_iterator = _hive_const_iterator;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
	using skipfield_type = typename std::conditional_t<(sizeof(value_type) > 10), unsigned short, unsigned char>;

	struct _hive_memory_block;

	class _hive_block_iterator {
	public:
		using iterator_category = std::random_access_iterator_tag;
		using difference_type = ptrdiff_t;
		using value_type = char;
		using pointer = char;
		using reference = char&;

	public:
		_hive_block_iterator() = default;			// 2025. 5. 20
		_hive_block_iterator(int* p) : p{ p } {}	// special X

		// iterator essentials
		_hive_block_iterator& operator++() {
			++p;
			return *this;
		};

		int& operator*() const {
			return *p;
		}

		bool operator==(const _hive_block_iterator& rhs) const {
			return p == rhs.p;	// 같은 "위치" 인지 비교
		}

		// forward iterator essentials
		_hive_block_iterator operator++(int) {
			_hive_block_iterator tmp = *this;
			tmp.p++;
			return tmp;
		}

		// bidirectional iterator essentials
		_hive_block_iterator& operator--() {
			--p;
			return *this;
		}

		_hive_block_iterator operator--(int) {
			_hive_block_iterator tmp = *this;
			tmp.p--;
			return tmp;
		}

		// random access iterator essentials
		_hive_block_iterator& operator+=(difference_type n) {
			p += n;
			return *this;
		}

		_hive_block_iterator operator+(difference_type n) {
			return p + n;
		}

		_hive_block_iterator& operator-=(difference_type n) {
			p -= n;
			return *this;
		}

		_hive_block_iterator operator-(difference_type n) {
			return p - n;
		}

		difference_type operator-(const _hive_block_iterator& rhs) {
			return p - rhs.p;
		}

		auto operator<=>(const _hive_block_iterator& rhs) const {
			return p <=> rhs.p;
		}


	private:
		int* p{};

	};

	struct _hive_memory_block {
		int* pBlock;
		skipfield_type* pSkipfields;


		size_t size;
		size_t capacity;

		// functions
		_hive_memory_block(size_t bytes) : capacity{ bytes } {
			size = 0;
		}

		~_hive_memory_block() {
		}

		int& operator[](size_t idx) const {
		}

		_hive_block_iterator begin() const {
			return pBlock;
		}
		_hive_block_iterator end() const {
			return pBlock + capacity;
		}

		template <typename U, typename = std::enable_if_t<std::is_same_v<T, U>>>
		bool insert(U&& elem) {

		}

		void erase(size_t idx) {

		}
	};

public:
	// ctor, copy, destroy
	// based on [hive.cons]
	hive() = default;

	hive(size_type n, hive_limits block_limits);
	hive(size_type n, const int& value);
	hive(size_type n, const int& vlaue, hive_limits block_limits);
	template <class InputIt>
	hive(InputIt first, InputIt last);
	template <class InputIt>
	hive(InputIt first, InputIt last, hive_limits block_limits);
	hive(const hive& x);
	hive(hive&& x);
	hive(std::initializer_list<int> il);
	hive(std::initializer_list<int> il, hive_limits block_limits);

	~hive();
	hive& operator=(const hive& x);
	hive& operator=(hive&& x);
	hive& operator=(std::initializer_list<int>);
	template<class InputIterator>
	void assign(InputIterator first, InputIterator last);
	template<std::_Container_compatible_range<int> R>
	void assign_range(R&& rg);
	void assign(size_type n, const int& t);
	void assign(std::initializer_list<int>);
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
	iterator insert(const_iterator hint, const int& x);
	iterator insert(const_iterator hint, int&& x);
	void insert(std::initializer_list<int> il);
	template<std::_Container_compatible_range<int> R>
	void insert_range(R&& rg);
	template<class InputIt>
	void insert(InputIt first, InputIt last);
	void insert(size_type n, const int& x);
	iterator erase(const_iterator position);
	iterator erase(const_iterator first, const_iterator last);
	void swap(hive&) noexcept;
	void clear() noexcept;


	// operations
	// based on [hive.operations]
	void splice(hive& x);
	void splice(hive&& x);
	template<class BinaryPredicate = std::equal_to<int>>
	size_type unique(BinaryPredicate binary_pred = BinaryPredicate());

	template<class Compare = std::less<int>>
	void sort(Compare comp = Compare());

	iterator get_iterator(const_pointer p) noexcept;
	const_iterator get_iterator(const_pointer p) const noexcept;

private:
	hive_limits current_limits = {10, 100};

};