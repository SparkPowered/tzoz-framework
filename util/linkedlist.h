#pragma once

#include "../stdafx.h"

namespace mu
{
	template <typename T>
	class linkednode {
	public:
		linkednode() : next(nullptr) {}
		linkednode (T &info) : m_next(nullptr), m_info(info) {}

		T m_info;
		linkednode<T> *m_next;
	};

	// generic linked list
	template <typename T>
	class linkedlist {
	public:
		linkedlist()
		{
			reset();
		}

		~linkedlist()
		{
			free();
		}

		void reset()
		{
			m_head = m_tail = nullptr;
		}

		linkednode<T> *begin()
		{
			return m_head;
		}

		const linkednode<T> *begin() const
		{
			return m_head;
		}

		linkednode<T> *end()
		{
			return m_tail;
		}

		const linkednode<T> *end() const
		{
			return m_tail;
		}

		linkednode<T> *search (T &info) const;
		linkednode<T> *insertHead (T &info);
		linkednode<T> *insertTail (T &info);

		void remove (T &info);
		void free();

	protected:
		// chain of pointers for a single list
		linkednode<T> *m_head, *m_tail;
	};

	template <typename T>
	linkednode<T> *linkedlist<T>::search (T &info) const
	{
		// check the list for a match
		for (linkednode<T> *current = m_head; current != nullptr; current = current->m_next)
		{
			if (current->m_info == info)
				return current;
		}

		return nullptr;
	}

	template <typename T>
	linkednode<T> *linkedlist<T>::insertHead (T &info)
	{
		linkednode<T> *link = new linkednode<T>(info);

		// insert before the m_head node
		link->m_next = m_head;
		m_head = link;

		// if the list was empty then this node will also be the tail
		if (m_tail == nullptr)
			m_tail = link;

		return link;
	}

	template <typename T>
	linkednode<T> *linkedlist<T>::insertTail (T &info)
	{
		linkednode<T> *link = new linkednode<T>(info);

		if (m_head != nullptr)
		{
			// insert after the m_tail node
			m_tail->m_next = link;
			m_tail = link;
		}
		else
		{
			// list is empty; this will be the m_head and m_tail node
			m_head = m_tail = link;
		}

		return link;
	}

	template <typename T>
	void linkedlist<T>::remove (T &info)
	{
		linkednode<T> *current, *trailCurrent;

		if (m_head != nullptr)
		{
			// check the start of the list
			if (m_head->m_info == info)
			{
				current = m_head;

				// only item in the list?
				if ((m_head = m_head->m_next) == nullptr)
					m_tail = nullptr;

				delete current;
			}
			else
			{
				// search the list					
				for (trailCurrent = m_head, current = m_head->m_next; current != nullptr;)
				{
					if (current->m_info == info)
					{
						// skip one node
						trailCurrent->m_next = current->m_next;
						
						// fix list
						if (m_tail == current)
							m_tail = trailCurrent;

						// remove and break the loop
						delete current;
						current = nullptr;
					}
					else
					{
						// continue
						trailCurrent = current;
						current = current->m_next;
					}
				}
			}
		}
	}

	template <typename T>
	void linkedlist<T>::free()
	{
		linkednode<T> *current, *trailCurrent;

		for (current = m_head; current != nullptr;)
		{
			// save the current node
			trailCurrent = current;
			// get the next one
			current = current->m_next;
			// delete this one
			delete trailCurrent;
		}

		reset();
	}
};