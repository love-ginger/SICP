#include <iostream>
#include <functional>
#include <cstdlib>
#include <sys/time.h>

static uint32_t findDivisor(uint32_t n, uint32_t testDivisor) {
    auto square = [](uint32_t a) {return a * a;};
    auto devides = [](uint32_t a, uint32_t b) {
        return b % a == 0;
    };
    if(square(testDivisor) > n) return n;
    else if(devides(testDivisor, n)) return testDivisor;
    else return findDivisor(n, testDivisor+1);
}

// 原始方法求素数
bool prime(uint32_t n) {
    auto smallestDivisor = [=]() {
        return findDivisor(n, 2);
    };
    return smallestDivisor() == n;
}

/* *
 * 费马小定理
 * 如果n是一个素数，a是小于n的任意正整数，那么a的n次方与a模n同余
 * 如果n不是素数，那么一般而言大部分的a<n都满足上面关系
 * */

// 使用uint64_t作为返回值，由于uint32_t的平方会使得uint32_t溢出
uint64_t expmod(uint32_t base, uint32_t exp, uint32_t m) {
    auto expIsEven = [=]{return (exp & 1) == 0;};
    auto square = [](uint64_t a){return a * a;};
    if(exp == 0) return 1;
    else if(expIsEven()) return square(expmod(base, exp / 2, m)) % m;
    else return (base * expmod(base, exp-1, m)) % m;
}


uint64_t expmod2(uint32_t base, uint32_t exp, uint32_t m) {
    auto expIsEven = [](uint32_t e){return (e & 1) == 0;};
    auto square = [](uint64_t a){return a * a;};
    uint64_t r = 1;
    uint64_t s = 0;
    while(exp != 0) {
        if(expIsEven(exp)) {
            s = square(base);
            base = (s % m);
            exp /= 2;
        } else {
            r = (r * base) % m;
            exp--;
        }
    }
    return r % m;
}

uint64_t expmodIter(uint32_t base, uint32_t exp, uint32_t m, uint64_t r) {
    auto expIsEven = [=]{return (exp & 1) == 0;};
    auto square = [](uint64_t a){return a * a;};
    if(exp == 0) return r;
    else if(expIsEven()) {
        uint64_t s = square(base) % m;
        return expmodIter(s, exp/2, m, r);
    }
    else return expmodIter(base, exp-1, m, (r*base)%m);
}

uint64_t expmod3(uint32_t base, uint32_t exp, uint32_t m) {
    return expmodIter(base, exp, m, 1);
}

bool primeTest(uint32_t n, uint32_t times) {
    auto tryIt = [=](uint32_t t) {
        return expmod3(t, n, n) == t; // can change to expmod
    };
    auto random = [=]{return (std::rand() % (n-1)) + 1;}();
    if(times == 0) return true;
    else if(tryIt(random)) return primeTest(n, times-1);
    return false;
}

bool prime2(uint32_t n) {
    return primeTest(n, 10);
}



const static uint32_t N = 10000000;
void test0(std::function<bool(uint32_t)> f) {
    uint32_t count = 0;
    struct timeval t1, t2;
    ::gettimeofday(&t1, nullptr);
    for(uint32_t i = 2; i < N; i++) {
        if(f(i)) {
            count++;
        }
    }
    ::gettimeofday(&t2, nullptr);
    std::cout << "\nprime count: " << count << '\n';
    std::cout << N << " times cost: " << t2.tv_sec - t1.tv_sec << "s "
        << t2.tv_usec - t1.tv_usec << "us\n";
}

void test1(std::function<uint32_t(uint32_t, uint32_t, uint32_t)> f) {
    struct timeval t1, t2;
    ::gettimeofday(&t1, nullptr);
    std::cout <<"2^0 % 100 = " <<  f(2, 0, 100) << '\n';
    std::cout <<"2^1 % 100 = " <<  f(2, 1, 100) << '\n';
    std::cout <<"2^5 % 100 = " <<  f(2, 5, 100) << '\n';
    std::cout <<"2^10 % 100 = " <<  f(2, 10, 100) << '\n';
    std::cout <<"3^0 % 100 = " <<  f(3, 0, 100) << '\n';
    std::cout <<"3^1 % 100 = " <<  f(3, 1, 100) << '\n';
    std::cout <<"3^5 % 100 = " <<  f(3, 5, 100) << '\n';
    std::cout <<"3^10 % 100 = " <<  f(3, 10, 100) << '\n';
    std::cout <<"large: " <<  f(833123121, 3212323123, 3212323123) << '\n';
    ::gettimeofday(&t2, nullptr);
    std::cout << "expmod cost: " << t2.tv_sec - t1.tv_sec << "s "
        << t2.tv_usec - t1.tv_usec << "us\n";
}

void test2(std::function<bool(uint32_t)> f1,
        std::function<bool(uint32_t)> f2) {
    for(uint32_t i = 2; i < N; i++) {
        bool b1 = f1(i);
        bool b2 = f2(i);
        if(b1 && !b2) {
            std::cout << i << ' ';
        }
        if(!b1 && b2) {
            std::cout << " $" << i << "$ ";
        }
    }
    std::cout << '\n';
}

int main() {
    std::srand(std::time(nullptr));
    test0(prime);
    test0(prime2);
    test1(expmod);
    test1(expmod2);
    test1(expmod3);
    test2(prime, prime2);
}
