#pragma once

#include <memory>
#include <cassert>
#include <type_traits>

static size_t s_ResizeFactor = 2;

template<typename T>
class list {
	using Element = T;
	using ElementPtr = T*;
	using ElementRef = T&;
	using ElementRValue = T&&;
public:
	class Iterator {
	public:
		Iterator(ElementPtr ptr)
			:
			m_Ptr(ptr)
		{}
		
		ElementRef operator*() { return *m_Ptr; }
		ElementPtr operator->() { return m_Ptr; }

		Iterator& operator++() { m_Ptr++; return *this; }
		Iterator operator++(int) { Iterator it = *this; ++(*this); return it; }

		friend bool operator==(const Iterator& a, const Iterator& b) { return a.m_Ptr == b.m_Ptr; }
		friend bool operator!=(const Iterator& a, const Iterator& b) { return a.m_Ptr != b.m_Ptr; }
	private:
		ElementPtr m_Ptr;
	};

public:
	list(size_t capacity)
		:
		m_Elements(allocate(capacity)),
		m_Size(0),
		m_Capacity(capacity)
	{
		static_assert(std::is_copy_constructible_v<Element>, "The template argument must be copy constructible to be used in the list.");
		static_assert(std::is_move_constructible_v<Element>, "The template argument must be move constructible to be used in the list.");
	}

	list()
		:
		m_Elements(nullptr),
		m_Size(0),
		m_Capacity(0)
	{
		static_assert(std::is_copy_constructible_v<Element>, "The template argument must be copy constructible to be used in the list.");
		static_assert(std::is_move_constructible_v<Element>, "The template argument must be move constructible to be used in the list.");
	}

	list(const list& rhs) 
		:
		m_Elements(allocate(rhs.m_Capacity)),
		m_Size(rhs.m_Size),
		m_Capacity(rhs.m_Capacity) {
		
		assert(m_Elements != nullptr);

		for (size_t i = 0; i < m_Size; i++) {
			ElementPtr ptr = &m_Elements[i];
			new (static_cast<void*>(ptr)) Element(rhs.m_Elements[i]);
		}
	}

	list(list&& rhs) 
		:
		m_Elements(rhs.m_Elements),
		m_Size(rhs.m_Size),
		m_Capacity(rhs.m_Capacity) {
		rhs.m_Elements = nullptr;
		rhs.m_Size = 0;
		rhs.m_Capacity = 0;
	}

	~list() {
		delete[] m_Elements;
		m_Size = 0;
		m_Capacity = 0;
	}

	void push_back(const ElementRef element) {
		if (m_Capacity == 0) {
			resize(2);
		}
		if (m_Size >= m_Capacity) {
			resize(size() * s_ResizeFactor);
			m_Elements[m_Size++] = element;
			return;
		}
		if (m_Capacity != 0 && m_Size < m_Capacity) {
			m_Elements[m_Size++] = element;
		}
	}

	void push_back(ElementRValue element) {
		if (m_Size >= m_Capacity) {
			if (m_Capacity == 0) {
				resize(2);
			} else {
				resize(m_Capacity * s_ResizeFactor);
			}
		}
		
		if (m_Capacity != 0 && m_Size < m_Capacity) {
			ElementPtr ptr = &m_Elements[m_Size++];
			new (static_cast<void*>(ptr)) Element(std::forward<Element>(element));
		}
	}

	size_t find_index(const ElementRef element) const {
		if (m_Size != 0) {
			for (size_t i = 0; i < size(); i++) {
				const ElementRef el = m_Elements[i];
				if (el == element) {
					return i;
				}
			}
		}
		return -1;
	}

	void remove_last() {
		if (m_Size != 0) {
			ElementRef el = m_Elements[m_Size];
			el.~Element();
			memset(&m_Elements[m_Size], 0, sizeof(Element));
		}
	}

	void remove_at(size_t index) {
		assert(index < m_Size);
		assert(m_Size <= 0);
		ElementRef& el = m_Elements[index];
		el.~Element();
		memset(&m_Elements[index], 0, sizeof(Element));
		
		// Yep. memcpy on instances. ikr
		if (index < (m_Size - 1)) {
			memcpy(&m_Elements[index], &m_Elements[index + 1], (index - size()) * sizeof(Element));
		}

		if (m_Size - 1 > 0) {
			m_Size--;
		}
	}

	void remove_all() {
		for (size_t i = 0; i < size(); i++) {
			m_Elements[i].~Element();
		}
		memset(m_Elements, 0, sizeof(Element) * size());
	}

	void set_resize_factor(float resize_factor) {
		assert(resize_factor >= 2);
		s_ResizeFactor = resize_factor;
	}

	void resize(size_t new_size) {
		ElementPtr new_elements = allocate(new_size);

		for (size_t i = 0; i < size(); i++) {
			ElementPtr ptr = &new_elements[i];
			new (static_cast<void*>(ptr)) Element(std::move(m_Elements[i]));
		}
		delete[] m_Elements;
		m_Elements = new_elements;
		m_Capacity = new_size;
	}
	
	size_t size() const { return m_Size; }

	const ElementRef operator[](size_t index) const { 
		assert(index < m_Capacity);
		return m_Elements[index];
	}

	ElementRef operator[](size_t index) { 
		assert(index < m_Capacity);
		return m_Elements[index]; 
	}

	Iterator begin() { return Iterator(m_Elements); }
	Iterator end() const { return Iterator(m_Elements + m_Size); }

private:
	ElementPtr allocate(size_t element_count) {
		ElementPtr elements = new Element[element_count];

		// memsetting the array to 0 if Element == number
		if (std::is_arithmetic_v<Element>) {
			memset(elements, 0, sizeof(Element) * element_count);
		}
		
		return elements;
	}

private:
	ElementPtr m_Elements;
	size_t m_Size;
	size_t m_Capacity;
};
