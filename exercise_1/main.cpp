#include <iostream>
#include <limits>
#include <unordered_set>
#include <fstream>

#define INITIAL_CAPACITY 8

class Vector
{
public:
    Vector(): Vector{ INITIAL_CAPACITY }
    { }

    Vector(int capacity): _capacity{ capacity }, _size{ 0 }, _data{ new int32_t[_capacity] }
    { }

    ~Vector()
    {
        _capacity = 0;
        _size = 0;
        delete _data;
    }

    void insert(int value)
    {
        if (_size >= _capacity)
        {
            std::cerr << "vector overflow\n";
            std::exit(1);
        }
        *(_data + _size) = value;
        ++_size;
    }

    int at(int index) const
    {
        if (index < 0)
        {
            std::cerr << "index to vector can not be negative\n";
            std::exit(1);
        }
        return *(_data + index);
    }
    
    int set(int index, int value)
    {
        if (index < 0)
        {
            std::cerr << "index to vector can not be negative\n";
            std::exit(1);
        }
        return (*(_data + index) = value);
    }

    int capacity() const
    {
        return _capacity;
    }

    int size() const
    {
        return _size;
    }

private:
    int _capacity;
    int _size;
    int32_t *_data;
};

std::ostream &operator << (std::ostream &out, const Vector &vector)
{
    out << "[ ";
    for (int i = 0; i < vector.size(); ++i)
    {
        out << vector.at(i);
        if (i < vector.size() - 1)
        {
            out << ",";
        }
        out << " ";
    }
    out << "]";
    return out;
}

void vector_insert_random_values(Vector &vector);

int get_duplicates_num(const Vector &vector);

void round_duplicates(Vector &vector);

void print_final_values_to_file(const Vector &vector);

int main()
{
    Vector vector{ 1'000'000 };

    vector_insert_random_values(vector);

    std::cout << "number of duplicates: " << get_duplicates_num(vector) << "\n";
    
    auto start_time = time(nullptr);
    
    round_duplicates(vector);
    
    auto end_time = time(nullptr);
    
    std::cout << "time taken by duplicates handling routine = " << end_time - start_time << " seconds\n";
    
    std::cout << "number of duplicates: " << get_duplicates_num(vector) << "\n";

    print_final_values_to_file(vector);

    return 0;
}

void vector_insert_random_values(Vector &vector)
{
    std::ofstream file { "./input.txt" };

    if (!file.is_open())
    {
        std::cerr << "dataset.txt couldn't be opened\n";
        std::exit(1);
    }

    for (int i = 0; i < vector.capacity(); ++i)
    {
        int value = static_cast<int>(1'000'000 * drand48());
        vector.insert(value);
        file << value << " ";
    }

    file.close();
}

int get_duplicates_num(const Vector &vector)
{
    std::unordered_set<int> used;
    int result = 0;

    for (int i = 0; i < vector.size(); ++i)
    {
        if (used.count(vector.at(i)) == 0)
        {
            used.insert(vector.at(i));
        }
        else
        {
            ++result;
        }
    }
    return result;
}

void round_duplicates(Vector &vector)
{
    std::unordered_set<int> free;

    for (int i = 0; i < 1'000'000; ++i)
    {
        free.insert(i);
    }

    int max = std::numeric_limits<int>::min();
    
    for (int i = 0; i < vector.size(); ++i)
    {
        if (free.count(vector.at(i)) == 1) free.erase(vector.at(i));
        if (max < vector.at(i))
        {
            max = vector.at(i);
        }
    }

    std::unordered_set<int> used;
    
    for (int i = 0; i < vector.size(); ++i)
    {
        if (used.count(vector.at(i)) == 0)
        {
            used.insert(vector.at(i));
        }
        else
        {
            bool found = false;
            int left = vector.at(i)-1;
            int right = vector.at(i)+1;
            while (left >= 0 && right <= max)
            {
                if ((free.count(left) == 1 && free.count(right) == 1) || (free.count(left) == 1))
                {
                    found = true;
                    free.erase(left);
                    vector.set(i, left);
                }
                else if (free.count(right) == 1)
                {
                    found = true;
                    free.erase(right);
                    vector.set(i, right);
                }
                if (found) break;
                --left;
                ++right;
            }
            if (!found)
            {
                while (left >= 0)
                {
                    if (free.count(left) == 1)
                    {
                        found = true;
                        free.erase(left);
                        vector.set(i, left);
                        break;
                    }
                    --left;
                }
            }
            if (!found)
            {
                while (right <= 999'999)
                {
                    if (free.count(right) == 1)
                    {
                        found = true;
                        free.erase(right);
                        vector.set(i, right);
                        break;
                    }
                    ++right;
                }
            }
        }
    }
}

void print_final_values_to_file(const Vector &vector)
{
    std::ofstream file { "./result.txt" };

    if (!file.is_open())
    {
        std::cerr << "result.txt couldn't be opened\n";
        std::exit(1);
    }

    for (int i = 0; i < vector.size(); ++i)
    {
        file << vector.at(i) << " ";
    }

    file.close();
}