#pragma once

#include <memory>
#include <cassert>

static size_t s_ResizeFactor = 2;

template<typename T>
class vector {
	using Element = T;
	using ElementPtr = T*;
public:
	vector(size_t capacity)
		:
		m_Elements(new Element[capacity]),
		m_Size(0),
		m_Capacity(capacity)
	{}

	vector()
		:
		m_Elements(nullptr),
		m_Size(0),
		m_Capacity(0)
	{}

	vector(const vector& rhs) 
		:
		m_Elements(new Element[rhs.m_Capacity]),
		m_Size(rhs.m_Size),
		m_Capacity(rhs.m_Capacity) {
		
		assert(m_Elements != nullptr);
		memset(m_Elements, 0, sizeof(Element) * m_Capacity);

		for (size_t i = 0; i < m_Size; i++) {
			ElementPtr ptr = &m_Elements[i];
			new (static_cast<void*>(ptr)) Element(rhs.m_Elements[i]);
		}
	}

	vector(vector&& rhs) 
		:
		m_Elements(rhs.m_Elements),
		m_Size(rhs.m_Size),
		m_Capacity(rhs.m_Capacity) {
		memset(&rhs, 0, sizeof(rhs));
	}

	~vector() {
		if (m_Elements != nullptr) {
			delete[] m_Elements;
		}
		m_Size = 0;
		m_Capacity = 0;
	}

	void push_back(const Element& element) {
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

	void push_back(Element&& element) {
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

	size_t find_index(const Element& element) const {
		if (m_Size != 0) {
			for (size_t i = 0; i < size(); i++) {
				const Element& el = m_Elements[i];
				if (el == element) {
					return i;
				}
			}
		}
		return -1;
	}

	void remove_last() {
		if (m_Size != 0) {
			Element& el = m_Elements[m_Size];
			el.~Element();
			memset(&m_Elements[m_Size], 0, sizeof(Element));
		}
	}

	void remove_at(size_t index) {
		assert(index < m_Size);
		Element& el = m_Elements[index];
		el.~Element();
		memset(&m_Elements[index], 0, sizeof(Element));
		
		// Yep. memcpy on instances. ikr
		if (index < (m_Size - 1)) {
			memcpy(&m_Elements[index], &m_Elements[index + 1], (index - size()) * sizeof(Element));
		}

		m_Size--;
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
		ElementPtr new_elements = new Element[new_size];
		memset(new_elements, 0, sizeof(Element) * new_size);
		for (size_t i = 0; i < size(); i++) {
			ElementPtr ptr = &new_elements[i];
			new (static_cast<void*>(ptr)) Element(std::move(m_Elements[i]));
		}
		delete[] m_Elements;
		m_Elements = new_elements;
		m_Capacity = new_size;
	}
	
	size_t size() const { return m_Size; }

	const Element& operator[](size_t index) const { 
		assert(index < m_Capacity);
		return m_Elements[index];
	}

	Element& operator[](size_t index) { 
		assert(index < m_Capacity);
		return m_Elements[index]; 
	}

private:
	ElementPtr m_Elements;
	size_t m_Size;
	size_t m_Capacity;
};

