#include <iostream>
#include <cstring>
#include <fstream>
#include <chrono>

#define VECTOR_SIZE 10'000'000
#define GROW_CAPACITY(capacity) ((capacity < 8) ? 8 : (capacity * 2))

using i64 = int64_t;

class Vector
{
public:
    Vector(): Vector{ 0 }
    {
    }

    Vector(i64 capacity): capacity_{ capacity }, size_{ 0 }, data_{ new int[capacity_] } 
    {
    }

    // Accessors
    i64 capacity() const
    {
        return capacity_;
    }

    i64 size() const
    {
        return size_;
    }

    int at(i64 index) const
    {
        check_out_of_bounds_error(index);
        return *(data_ + index);
    }

    // Mutators
    void push_back(int value)
    {
        if (size_ >= capacity_)
        {
            grow_capacity();
        }
        *(data_ + size_) = value;
        size_++;
    }

    void set(i64 index, int value)
    {
        check_out_of_bounds_error(index);
        *(data_ + index) = value;
    }

private:
    // Private helper methods
    void check_out_of_bounds_error(i64 index) const
    {
        if (index < 0 || index >= size_)
        {
            std::cerr << "OutOfBoundsError: Index is out of limits.\n";
            std::exit(1);
        }
    }

    void grow_capacity()
    {
        int *tmp{ new int[size_] };

        memcpy(tmp, data_, size_ * sizeof(int));
        
        delete data_;
        
        capacity_ = GROW_CAPACITY(capacity_);
        data_ = new int[capacity_];

        memcpy(data_, tmp, size_ * sizeof(int));

        delete tmp;
    }

private:
    i64 capacity_;
    i64 size_;
    int *data_;
};

std::ostream &operator << (std::ostream &out, const Vector &vector)
{
    out << "[ ";
    
    for (i64 i{ 0 }; i < vector.size(); ++i)
    {
        out << vector.at(i);
        if (i < vector.size() - 1)
        {
            out << ", ";
        }
        else
        {
            out << " ";
        }
    }

    out << "]";

    return out;
}

void generate_random_values(Vector &vector);

i64 num_duplicates(const Vector &vector, bool **arr);

void remove_duplicates(Vector &vector, bool *arr);

void save_to_file(const Vector &vector);

int main()
{
    Vector vector{ };
    generate_random_values(vector);

    bool *arr{ nullptr };

    std::cout << "Number of duplicates = " << num_duplicates(vector, &arr) << "\n";
    
    auto start{ std::chrono::high_resolution_clock::now() };
    
    remove_duplicates(vector, arr);
    
    auto end{ std::chrono::high_resolution_clock::now() };

    auto elapsed_time{ std::chrono::duration_cast<std::chrono::microseconds>(end - start) };

    std::cout << "Time taken by duplicate handling procedure = " << elapsed_time.count() << " microseconds.\n";
    
    std::cout << "Number of duplicates = " << num_duplicates(vector, &arr) << "\n";

    save_to_file(vector);
    
    delete[] arr;
    return 0;
}

void generate_random_values(Vector &vector)
{
    std::ofstream file{ "./input.txt" };

    if (!file.is_open())
    {
        std::cerr << "FileNotOpenedError: Somehow file couldn't be opened.\n";
        std::exit(1);
    }

    for (i64 i{ 0 }; i < VECTOR_SIZE; ++i)
    {
        int value{ static_cast<int>(drand48() * VECTOR_SIZE) };
        vector.push_back(value);
        file << value << " ";
    }

    file.close();
}

i64 num_duplicates(const Vector &vector, bool **arr)
{
    delete[] *arr;
    *arr = new bool[VECTOR_SIZE];
    
    for (i64 i{ 0 }; i < VECTOR_SIZE; ++i)
    {
        (*arr)[i] = false;
    }
    
    i64 result = 0;

    for (i64 i{ 0 }; i < vector.size(); ++i)
    {
        if ((*arr)[vector.at(i)] == true)
        {
            ++result;
        }
        (*arr)[vector.at(i)] = true;
    }

    return result;
}

void remove_duplicates(Vector &vector, bool *arr)
{
    bool *used{ new bool[VECTOR_SIZE] };
    for (i64 i{ 0 }; i < VECTOR_SIZE; ++i)
    {
        used[i] = false;
    }

    for (i64 i { 0 }; i < vector.size(); ++i)
    {
        if (used[vector.at(i)] == false)
        {
            used[vector.at(i)] = true;
        }
        else
        {
            int left{ vector.at(i)-1 };
            int right{ vector.at(i)+1 };
            
            bool is_found{ false };
            
            while (left >= 0 && right < VECTOR_SIZE)
            {
                if ((arr[left] == false && arr[right] == false) || (arr[left] == false))
                {
                    arr[left] = true;
                    vector.set(i, left);
                    is_found = true;
                    break;
                }
                else if (arr[right] == false)
                {
                    arr[right] = true;
                    vector.set(i, right);
                    is_found = true;
                    break;
                }
                --left;
                ++right;
            }
            if (!is_found)
            {
                while (left >= 0)
                {
                    if (arr[left] == false)
                    {
                        arr[left] = true;
                        vector.set(i, left);
                        is_found = true;
                        break;
                    }
                    --left;
                }
            }
            if (!is_found)
            {
                while (right < VECTOR_SIZE)
                {
                    if (arr[right] == false)
                    {
                        arr[right] = true;
                        vector.set(i, right);
                        is_found = true;
                        break;
                    }
                    ++right;
                }
            }
        }
    }

    delete[] used;
}

void save_to_file(const Vector &vector)
{
    std::ofstream file{ "result.txt" };

    if (!file.is_open())
    {
        std::cerr << "FileNotOpenedError: Somehow file couldn't be opened.\n";
        std::exit(1);
    }

    for (i64 i{ 0 }; i < vector.size(); ++i)
    {
        file << vector.at(i) << " ";
    }

    file.close();
}