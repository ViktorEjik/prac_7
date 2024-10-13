#pragma once

#include <iostream>

template <typename T>
class SharedPtr 
{
public:
    // Constructor
    SharedPtr(T* ptr = nullptr)
    {
        m_ptr = ptr;
        m_counter = new uint64_t{};
        *m_counter = 1;
    }

    // Copy constructor
    SharedPtr(SharedPtr<T>& sp)
    {
        m_ptr = sp.m_ptr;
        m_counter = sp.m_counter;
        (*m_counter)++;
    }

    uint64_t use_count() 
    { 
      return *m_counter; 
    }

    T& operator*() 
    { 
      return *m_ptr; 
    }

    T* get(){
        return m_ptr;
    }

    T* operator->() 
    { 
      return m_ptr;
    }
    
    void swap(SharedPtr& sp){
        std::swap(m_ptr, sp.m_ptr);
    }

    SharedPtr& operator= (SharedPtr sp) {
        // if assigned pointer points to some other location
        if (m_ptr != sp.m_ptr) {
            // if shared pointer already points to some location
            if(m_ptr && m_counter) {
                // decrease the reference counter for the previously pointed location
                (*m_counter)--;
                // if reference count reaches 0, deallocate memory
                if((*m_counter) == 0) {
                    delete m_counter;
                    delete m_ptr;
                }
            }
            // reference new memory location
            m_ptr = sp.m_ptr;
            // increase new memory location reference counter.
            if(m_ptr) {
                m_counter = sp.m_counter;
                (*m_counter)++;
            }
        }
        return *this;
    }
  
    // Destructor
    ~SharedPtr()
    {
        (*m_counter)--;
        if (*m_counter == 0) 
        {
            delete m_counter;
            delete m_ptr;
        }
    }

    friend std::ostream& operator<<(std::ostream& os,
                               SharedPtr<T>& sp)
    {
        os << "Address pointed : " << sp.m_ptr << std::endl;
        os << *(sp.m_counter) << std::endl;
        return os;
    }

private:
    uint64_t* m_counter;
    T* m_ptr;
};
