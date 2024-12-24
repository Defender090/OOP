#include <algorithm>
#include <stdexcept>

template <std::size_t N>
class Mask {
    std::array<int, N> mask;

public:
    Mask(std::same_as<int> auto... init)
        requires(sizeof...(init) == N)
        : mask{init...} {}

    std::size_t size() const {
        return N;
    }

    int at(std::size_t index) const {
        if (index >= N) {
            throw std::out_of_range("Index is out of range");
        }
        return mask[index];
    }

    auto begin() {
        return mask.begin();
    }
    auto end() {
        return mask.end();
    }

    template <typename Container>
    void slice(Container& container) const {
        std::size_t mask_index = 0;
        container.erase(std::remove_if(container.begin(), container.end(),
                                       [this, &mask_index](const auto& value) {
                                           return !mask[mask_index++ % N];
                                       }),
                        container.end());
    }

    template <typename Container, typename Func>
    Container transform(const Container& container, Func func) const {
        Container result = container;
        for (std::size_t i = 0; i < container.size(); i++)
            if (mask[i % N])
                result[i] = func(container[i]);

        return result;
    }

    template <typename Container, typename Func>
    Container slice_and_transform(const Container& container, Func func) const {
        auto result = transform(container, func);
        this->slice(result);
        return result;
    }

private:
};


#include <iostream>
#include <vector>

auto mul = [](int x) {
    return x * 10;
};

void print_arr(auto v) {
    std::cout << "[ ";
    for (auto i : v) {
        std::cout << i << " ";
    }
    std::cout << "]\n";
}

int main() {
    Mask<3> mask = {1, 0, 0};

    std::cout << "Mask size: " << mask.size() << std::endl;
    std::cout << "Mask: ";
    print_arr(mask);

    // Пример использования метода slice
    std::vector<int> data = {1, 2, 3, 4, 5, 6, 7};

    auto data1 = data;
    mask.slice(data1);
    std::cout << "slice:\n";
    print_arr(data1);
    // Пример использования метода transform
    auto data2 = data;
    auto transformed = mask.transform(data2, mul);
    std::cout << "transform:\n";
    print_arr(transformed);

    // Пример использования метода slice_and_transform
    auto data3 = data;
    auto sliced_transformed = mask.slice_and_transform(data3, mul);
    std::cout << "slice_and_transform:\n";
    print_arr(sliced_transformed);

    return 0;
}
