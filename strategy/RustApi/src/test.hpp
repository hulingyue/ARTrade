#include <iostream>
#include <memory>


namespace core::datas {
struct TradePair {
    double price;
    double quantity;
};
} // namespace core::datas


class Test {
public:
    Test() {
        std::cout  << "hi, I'm Test!" << std::endl;
    }

    void print() const {
        std::cout << "Hello World! price is: " << pair.price << " quantity is: " << pair.quantity << std::endl;
    }

private:
    core::datas::TradePair pair;
};

std::unique_ptr<Test> new_test() {
    return std::make_unique<Test>();
}