#ifndef H_TVECTOR
#define H_TVECTOR

#define explspec template<>

#include <string>
using std::string;
using std::to_string;
#include <stdexcept>
#include <cmath>
using std::pow;

enum searchingMethod
{
    linear, binary
};

enum sortingMethod
{
    quick, insertion, merge, shell
};

template<typename type>
class TVector
{
private:

    type* i_vector = nullptr;

    size_t i_capacity = 0;
    size_t i_size = 0;

    float i_growth_multiplier;

    bool sorted_guarantee = true;

    static const size_t MAX_CAPACITY = 65535;

    void copy(const TVector& p_other)
    {
        if (p_other.i_capacity > 0)
        {
            this->reserve(p_other.i_capacity);
            for (size_t _i = 0; _i < p_other.i_size; _i++)
                this->i_vector[_i] = p_other[_i];
        }
        
        this->i_size = p_other.i_size;
        this->i_growth_multiplier = p_other.i_growth_multiplier;
        this->sorted_guarantee = p_other.sorted_guarantee;
    }

    inline short compare(const type& p_obj1, const type& p_obj2) const
    {
        (void)p_obj1;
        (void)p_obj2;
        throw std::runtime_error("There is no comparison function provided for this type.");
    }
    inline size_t min(const size_t& p_index1, const size_t& p_index2) const
    {
        if (compare(i_vector[p_index2], i_vector[p_index1]) < 0)
            return p_index2;
        else
            return p_index1;
    }
    inline size_t max(const size_t& p_index1, const size_t& p_index2) const
    {
        if (compare(i_vector[p_index1], i_vector[p_index2]) < 0)
            return p_index2;
        else
            return p_index1;
    }

    inline void swap(const size_t& p_index1, const size_t& p_index2)
    {
        type obj = i_vector[p_index1];
        i_vector[p_index1] = i_vector[p_index2];
        i_vector[p_index2] = obj;
    }

    long linear_search(const type& p_searched) const
    {
        long num_of_lesser = -1;
        for (size_t _i = 0; _i < i_size; _i++)
        {
            int comparison = compare(p_searched, i_vector[_i]);
            if ((comparison) == 0)
                return _i;
            if (comparison > 0)
                num_of_lesser--;
        }
        return num_of_lesser;
    }
    long binary_search(const type& p_searched, const long long& p_low, const long long& p_high) const
    {
        if (p_high >= p_low)
        {
            long mid = (p_high - p_low) / 2 + p_low;
            short comparison = compare(i_vector[mid], p_searched);
            if (comparison != 0)
                if (comparison < 0)
                    return binary_search(p_searched, mid + 1, p_high);
                else
                    return binary_search(p_searched, p_low, mid - 1);
            else
                return mid;
        }
        long long placement = p_low;
        if (placement > (long long)i_size)
            placement = i_size;
        return (placement + 1) * -1;
    }

    void quick_sort(const int& p_low, const int& p_high)
    {
        auto median3 = [&](const int& p_i1, const int& p_i2, const int& p_i3)
        {
            // algorithm provided by Gyorgy Szekely - https://stackoverflow.com/a/19027761
            return max(min(p_i1, p_i2), min(max(p_i1, p_i2), p_i3));
        };
        auto partition = [&](const int& p_low, const int& p_high)
        { 
            int pivot_index = median3(p_low, p_high / 2, p_high - 1);
            type pivot = i_vector[pivot_index];
            int _i = p_low - 1;
            int _j = p_high;
            while (_i < _j)
            {
                do
                {
                    _i++;
                } while (compare(i_vector[_i], pivot) <= 0 && _i < p_high);
                do
                {
                    _j--;
                } while (compare(i_vector[_j], pivot) > 0);
                if (_i < _j)
                {
                    if (_j == pivot_index)
                        pivot_index = _i;
                    swap(_i, _j);
                }
            }
            swap(pivot_index, _j);
            return _j;
        };

        if (p_low < p_high - 1)
        {
            int pivot = partition(p_low, p_high);
            quick_sort(p_low, pivot);
            quick_sort(pivot + 1, p_high);
        }
    }

public:

    TVector(const size_t& p_initial_capacity = 0, const float& p_growth_multiplier = 2.0f)
    : i_growth_multiplier(p_growth_multiplier)
    {
        if (p_initial_capacity < MAX_CAPACITY)
        {
            if (p_initial_capacity > 0)
                reserve(p_initial_capacity);
            if (p_growth_multiplier < 0.0f)
                i_growth_multiplier *= -1.0f;
        }
        else
            throw std::invalid_argument("parameter initial_capacity out of allowed TVector range: 0 - " + to_string(MAX_CAPACITY));
    }
    TVector(const TVector& p_other)
    {
        copy(p_other);
    }
    ~TVector()
    {
        if (i_capacity > 0)
        {
            delete[] i_vector;
            i_vector = nullptr;
        }
    }

    TVector& operator=(const TVector& p_other)
    {
        if (this != &p_other)
        {
            if (i_capacity > 0)
            {
                delete[] this->i_vector;
                this->i_vector = nullptr;
                this->i_capacity = 0;
                this->i_size = 0;
            }
            copy(p_other);
        }
        return *this;
    }

    type& at(const size_t& p_index) const
    {
        if (p_index > i_size)
            throw std::invalid_argument("Parameter index out of range. 0 - " + to_string(i_size));
        return i_vector[p_index];
    }
    type& operator[](const size_t& p_index) const
    {
        return at(p_index);
    }

    size_t size() const
    {
        return i_size;
    }

    void set_growth_multiplier(const float& p_multiplier)
    {
        i_growth_multiplier = p_multiplier;
        if (p_multiplier < 0.0f)
            i_growth_multiplier *= -1.0f;
    }
    float growth_multiplier() const
    {
        return i_growth_multiplier;
    }

    size_t reserve(const size_t& p_amount)
    {
        size_t old_memory_size = i_capacity;
        if ((p_amount == MAX_CAPACITY && i_capacity == MAX_CAPACITY) || p_amount <= i_capacity)
            return 0;
        else if (p_amount > MAX_CAPACITY)
            return reserve(MAX_CAPACITY);
        else
        {
            type* new_array = new type[p_amount];
            for (size_t _i = 0; _i < i_size; _i++)
                new_array[_i] = i_vector[_i];

            if (i_capacity > 0)
                delete[] i_vector;

            i_vector = new_array;
            i_capacity = p_amount;
            return p_amount - old_memory_size;
        }
    }

    size_t shrink()
    {
        int old_memory_size = i_capacity;
        if (i_capacity > i_size)
        {
            type* new_array;
            if (i_size > 0)
            {
                new_array = new type[i_size];
                for (size_t _i = 0; _i < i_size; _i++)
                    new_array[_i] = i_vector[_i];
                delete[] i_vector;
                i_vector = new_array;
            }
            else
            {
                delete[] i_vector;
                i_vector = nullptr;
            }
            i_capacity = i_size;
            return old_memory_size - i_size;
        }
        else
            return 0;
    }

    void push(const type& p_obj)
    {
        if (i_size == i_capacity)
        {
            if (i_size == 0)
                reserve(1);
            else if (reserve(i_capacity * i_growth_multiplier) == 0)
                throw std::runtime_error("Exceeded maximum capacity");
        }
        i_vector[i_size++] = p_obj;
        sorted_guarantee = false;
    }
    void push(const type* p_objs, const size_t& p_size)
    {
        for (size_t _i = 0; _i < p_size; _i++)
            push(p_objs[_i]);
    }

    void insert(const type& p_obj, const size_t& p_index)
    {
        if (p_index < i_size)
        {
            type obj_swap1;
            type obj_swap2;
            if (i_size == i_capacity)
            {
                if (i_size == 0)
                    reserve(1);
                else if (reserve(i_capacity * i_growth_multiplier) == 0)
                    throw std::runtime_error("Exceeded maximum capacity");
            }
            obj_swap1 = i_vector[p_index];
            i_vector[p_index] = p_obj;
            i_size++;
            sorted_guarantee = false;
            for (size_t _i = p_index + 1; _i < i_size; _i++)
            {
                obj_swap2 = i_vector[_i];
                i_vector[_i] = obj_swap1;
                obj_swap1 = obj_swap2;
            }
        }
        else if (p_index == i_size)
            push(p_obj);
        else
            throw std::invalid_argument("Parameter index out of range of current TVector size");
    }

    type pull()
    {
        type obj = 0;
        if (i_size > 0)
            obj = i_vector[--i_size];
        else
            throw std::runtime_error("TVector is empty");
        return obj;
    }

    type erase(const size_t& p_index)
    {
        type obj;
        if (i_size > 0)
            obj = at(p_index);
        else
            throw std::runtime_error("TVector is empty");
        for (size_t _i = p_index; _i < --i_size; _i++)
            i_vector[_i] = i_vector[_i + 1];
        return obj;
    }

    long find(const type& p_searched, bool p_sorted = false, const searchingMethod& p_searching_method = searchingMethod::binary) const
    {
        if (p_sorted == true || sorted_guarantee == true)
        {
            switch(p_searching_method)
            {
            case searchingMethod::linear:
                return linear_search(p_searched);
            case searchingMethod::binary:
                return binary_search(p_searched, 0, i_size);
            default:
                throw std::invalid_argument("Parameter searched is not a valid searching method.");
            }
        }
        else
            return linear_search(p_searched);
    }
    long find(const type& p_searched, const sortingMethod& p_sorting_method, const searchingMethod& p_searching_method) const
    {
        sort(p_sorting_method);
        return find(p_searched, true, p_searching_method);
    }

    bool contains(const type& p_searched, bool p_sorted = false, const searchingMethod& p_searching_method = searchingMethod::binary) const
    {
        if (find(p_searched, p_sorted, p_searching_method) >= 0)
            return true;
        else
            return false;
    }
    bool contains(const type& p_searched, const sortingMethod& p_sorting_method, const searchingMethod& p_searching_method) const
    {
        if (find(p_searched, p_sorting_method, p_searching_method) >= 0)
            return true;
        else
            return false;
    }
    
    void emplace(const type& p_obj, bool p_sorted = false, const searchingMethod& p_searching_method = searchingMethod::binary)
    {
        long sorted_index = find(p_obj, p_sorted, p_searching_method);
        if (sorted_index < 0)
            sorted_index = (sorted_index + 1) * -1;
        insert(p_obj, sorted_index);
        sorted_guarantee = true;
    }
    void emplace(const type& p_obj, const sortingMethod& p_sorting_method, const searchingMethod& p_searching_method)
    {
        long sorted_index = find(p_obj, p_sorting_method, p_searching_method);
        if (sorted_index < 0)
            sorted_index = (sorted_index + 1) * -1;
        insert(p_obj, sorted_index);
        sorted_guarantee = true;
    }

    type extract(const type& p_searched, bool p_sorted = false, const searchingMethod& p_searching_method = searchingMethod::binary)
    {
        long index = find(p_searched, p_sorted, p_searching_method);
        if (index < 0)
            throw std::invalid_argument("Parameter searched not present in TVector.");
        else
            return erase(index);
    }
    type extract(const type& p_searched, const sortingMethod& p_sorting_method, const searchingMethod& p_searching_method)
    {
        long index = find(p_searched, p_sorting_method, p_searching_method);
        if (index < 0)
            throw std::invalid_argument("Parameter searched not present in TVector.");
        else
            return erase(index);
    }

    void sort(const sortingMethod& p_method = sortingMethod::quick)
    {
        switch(p_method)
        {
        case sortingMethod::quick:
            quick_sort(0, i_size);
            break;
        case sortingMethod::insertion:
            //insertion_sort();
        case sortingMethod::merge:
            // merge_sort();
        case sortingMethod::shell:
            // shell_sort();
        default:
            throw std::runtime_error("Invalid sorting method.");
        }
        sorted_guarantee = true;
    }

};

template<typename type>
const size_t TVector<type>::MAX_CAPACITY;

explspec short TVector<bool>::                  compare(const bool& p_obj1,                 const bool& p_obj2)                 const
{
    if (p_obj1 != p_obj2)
        if (p_obj1 > p_obj2)
            return 1;
        else
            return -1;
    else
        return 0;
}
explspec short TVector<char>::                  compare(const char& p_obj1,                 const char& p_obj2)                 const
{
    if (p_obj1 != p_obj2)
        if (p_obj1 > p_obj2)
            return 1;
        else
            return -1;
    else
        return 0;
}
explspec short TVector<signed char>::           compare(const signed char& p_obj1,          const signed char& p_obj2)          const
{
    if (p_obj1 != p_obj2)
        if (p_obj1 > p_obj2)
            return 1;
        else
            return -1;
    else
        return 0;
}
explspec short TVector<short>::                 compare(const short& p_obj1,                const short& p_obj2)                const
{
    if (p_obj1 != p_obj2)
        if (p_obj1 > p_obj2)
            return 1;
        else
            return -1;
    else
        return 0;
}
explspec short TVector<int>::                   compare(const int& p_obj1,                  const int& p_obj2)                  const
{
    if (p_obj1 != p_obj2)
        if (p_obj1 > p_obj2)
            return 1;
        else
            return -1;
    else
        return 0;
}
explspec short TVector<long>::                  compare(const long& p_obj1,                 const long& p_obj2)                 const
{
    if (p_obj1 != p_obj2)
        if (p_obj1 > p_obj2)
            return 1;
        else
            return -1;
    else
        return 0;
}
explspec short TVector<long long>::             compare(const long long& p_obj1,            const long long& p_obj2)            const
{
    if (p_obj1 != p_obj2)
        if (p_obj1 > p_obj2)
            return 1;
        else
            return -1;
    else
        return 0;
}
explspec short TVector<unsigned char>::         compare(const unsigned char& p_obj1,        const unsigned char& p_obj2)        const
{
    if (p_obj1 != p_obj2)
        if (p_obj1 > p_obj2)
            return 1;
        else
            return -1;
    else
        return 0;
}
explspec short TVector<unsigned short>::        compare(const unsigned short& p_obj1,       const unsigned short& p_obj2)       const
{
    if (p_obj1 != p_obj2)
        if (p_obj1 > p_obj2)
            return 1;
        else
            return -1;
    else
        return 0;
}
explspec short TVector<unsigned>::              compare(const unsigned& p_obj1,             const unsigned& p_obj2)             const
{
    if (p_obj1 != p_obj2)
        if (p_obj1 > p_obj2)
            return 1;
        else
            return -1;
    else
        return 0;
}
explspec short TVector<unsigned long>::         compare(const unsigned long& p_obj1,        const unsigned long& p_obj2)        const
{
    if (p_obj1 != p_obj2)
        if (p_obj1 > p_obj2)
            return 1;
        else
            return -1;
    else
        return 0; 
}
explspec short TVector<unsigned long long>::    compare(const unsigned long long& p_obj1,   const unsigned long long& p_obj2)   const
{
    if (p_obj1 != p_obj2)
        if (p_obj1 > p_obj2)
            return 1;
        else
            return -1;
    else
        return 0;
}
explspec short TVector<float>::                 compare(const float& p_obj1,                const float& p_obj2)                const
{
    if (p_obj1 != p_obj2)
        if (p_obj1 > p_obj2)
            return 1;
        else
            return -1;
    else
        return 0;
}
explspec short TVector<double>::                compare(const double& p_obj1,               const double& p_obj2)               const
{
    if (p_obj1 != p_obj2)
        if (p_obj1 > p_obj2)
            return 1;
        else
            return -1;
    else
        return 0;
}
explspec short TVector<long double>::           compare(const long double& p_obj1,          const long double& p_obj2)          const
{
    if (p_obj1 != p_obj2)
        if (p_obj1 > p_obj2)
            return 1;
        else
            return -1;
    else
        return 0;
}
explspec short TVector<string>::                compare(const string& p_obj1,               const string& p_obj2)               const
{
    int comparison = p_obj1.compare(p_obj2);
    if (comparison != 0)
        if (comparison > 0)
            return 1;
        else
            return -1;
    else
        return 0;
}

#endif

#ifndef H_TUDLLIST
#define H_TUDLLIST

#include <exception>

template <typename type>
struct TUDLListNode
{
    type value;
    TUDLListNode<type>* prev = nullptr;
    TUDLListNode<type>* next = nullptr;

    TUDLListNode(type p_value) : value(p_value) {}

};

template <typename type>
class TUDLListIterator;

template <typename type>
class TUDLList
{
friend class TUDLListIterator<type>;
private:

    TUDLListNode<type>* front = nullptr;
    TUDLListNode<type>* back = nullptr;

    TVector<TUDLListIterator<type>*> i_iterators;

    void copy(const TUDLList& p_other)
    {
        if (p_other.front)
        {
            TUDLListNode<type>* other_node = p_other.front;
            this->front = new TUDLListNode<type>(p_other.front->value);
            TUDLListNode<type>* this_node = this->front;
            while (other_node->next)
            {
                other_node = other_node->next;
                this_node->next = new TUDLListNode<type>(other_node->value);
                this_node->next->prev = this_node;
                this_node = this_node->next;
            }
            this->back = this_node;
        }
    }

public:

    TUDLList() {}
    TUDLList(const TUDLList& p_other)
    {
        copy(p_other);
    }
    ~TUDLList()
    {
        TUDLListNode<type>* prev = nullptr;
        while(front)
        {
            prev = front;
            front = front->next;
            delete prev;
        }

        for (unsigned int _i = 0; _i < i_iterators.size(); _i++)
            delete i_iterators[_i];
    }

    TUDLList& operator=(const TUDLList& p_other)
    {
        TUDLListNode<type>* prev = nullptr;
        TUDLListNode<type>* node = this->front;
        while (node)
        {
            prev = node;
            node = node->next;
            delete prev;
        }
        copy(p_other);
    }

    bool is_empty() const
    {
        if (!front)
            return true;
        return false;
    }

    void push_back(const type& p_obj)
    {
        TUDLListNode<type>* node = new TUDLListNode<type>(p_obj);
        if (back)
        {
            back->next = node;
            node->prev = back;
            back = node;
        }
        else
        {
            front = node;
            back = node;
        }
    }
    void push_back(const type* p_objs, const long& p_size)
    {
        for (unsigned int _i = 0; _i < p_size; _i++)
            push_back(p_objs[_i]);
    }

    void push_front(const type& p_obj)
    {
        TUDLListNode<type>* node = new TUDLListNode<type>(p_obj);
        if (front)
        {
            front->prev = node;
            node->next = front;
            front = node;
        }
        else
        {
            front = node;
            back = node;
        }
    }
    void push_front(const type* p_objs, const long& p_size)
    {
        for (unsigned int _i = 0; _i < p_size; _i++)
            push_front(p_objs[_i]);
    }

    type pull_back()
    {
        type value = back->value;
        if (back)
        {
            back = back->prev;
            back->next = nullptr;
        }
        else
        {
            throw std::runtime_error("List is empty.");
        }
        return value;
    }

    type pull_front()
    {
        type value = front->value;
        if (front)
        {
            front = front->next;
            front->prev = nullptr;
        }
        else
        {
            throw std::runtime_error("List is empty.");
        }
        return value;
    }

    TUDLListIterator<type>* iterator(const short& p_start = 0)
    {
        if (front)
        {
            TUDLListIterator<type>* iter = new TUDLListIterator<type>(this, p_start);
            i_iterators.push(iter);
            return iter;
        }
        else
            return nullptr;
    }

};

template <typename type>
class TUDLListIterator
{
friend class TUDLList<type>;
private:

    TUDLList<type>* master;

    TUDLListNode<type>* node = nullptr;

    TUDLListIterator(TUDLList<type>* p_tudllist, const short& p_start = 0) : master(p_tudllist)
    {
        if (p_start >= 0)
        {
            node = master->front;
        }
        else
        {
            node = master->back;
        }
    }
    ~TUDLListIterator() {}

public:

    bool has_next() const
    {
        if (node->next)
        {
            return true;
        }
        return false;
    }
    type& next()
    {
        node = node->next;
        return node->value;
    }

    bool has_prev() const
    {
        if (node->prev)
        {
            return true;
        }
        return false;
    }
    type& prev()
    {
        node = node->prev;
        return node->value;
    }

    type value() const
    {
        return node->value;
    }

    void clear()
    {
        master->i_iterators.extract(this);
        delete this;
    }

};

#endif
