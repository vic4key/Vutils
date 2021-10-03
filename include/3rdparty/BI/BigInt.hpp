/*
    BigInt
    ------
    Arbitrary-sized integer class for C++.
    
    Version: 0.5.0-dev
    Released on: 05 October 2020 23:15 IST
    Author: Syed Faheel Ahmad (faheel@live.in)
    Project on GitHub: https://github.com/faheel/BigInt
    License: MIT
*/

/*
    ===========================================================================
    BigInt
    ===========================================================================
    Definition for the BigInt class.
*/

#ifndef BIG_INT_HPP
#define BIG_INT_HPP

#include <iostream>
#include <string>

class BigInt {
    std::string value;
    char sign;

    public:
        // Constructors:
        BigInt();
        BigInt(const BigInt&);
        BigInt(const long long&);
        BigInt(const std::string&);

        // Assignment operators:
        BigInt& operator=(const BigInt&);
        BigInt& operator=(const long long&);
        BigInt& operator=(const std::string&);

        // Unary arithmetic operators:
        BigInt operator+() const;   // unary +
        BigInt operator-() const;   // unary -

        // Binary arithmetic operators:
        BigInt operator+(const BigInt&) const;
        BigInt operator-(const BigInt&) const;
        BigInt operator*(const BigInt&) const;
        BigInt operator/(const BigInt&) const;
        BigInt operator%(const BigInt&) const;
        BigInt operator+(const long long&) const;
        BigInt operator-(const long long&) const;
        BigInt operator*(const long long&) const;
        BigInt operator/(const long long&) const;
        BigInt operator%(const long long&) const;
        BigInt operator+(const std::string&) const;
        BigInt operator-(const std::string&) const;
        BigInt operator*(const std::string&) const;
        BigInt operator/(const std::string&) const;
        BigInt operator%(const std::string&) const;

        // Arithmetic-assignment operators:
        BigInt& operator+=(const BigInt&);
        BigInt& operator-=(const BigInt&);
        BigInt& operator*=(const BigInt&);
        BigInt& operator/=(const BigInt&);
        BigInt& operator%=(const BigInt&);
        BigInt& operator+=(const long long&);
        BigInt& operator-=(const long long&);
        BigInt& operator*=(const long long&);
        BigInt& operator/=(const long long&);
        BigInt& operator%=(const long long&);
        BigInt& operator+=(const std::string&);
        BigInt& operator-=(const std::string&);
        BigInt& operator*=(const std::string&);
        BigInt& operator/=(const std::string&);
        BigInt& operator%=(const std::string&);

        // Increment and decrement operators:
        BigInt& operator++();       // pre-increment
        BigInt& operator--();       // pre-decrement
        BigInt operator++(int);     // post-increment
        BigInt operator--(int);     // post-decrement

        // Relational operators:
        bool operator<(const BigInt&) const;
        bool operator>(const BigInt&) const;
        bool operator<=(const BigInt&) const;
        bool operator>=(const BigInt&) const;
        bool operator==(const BigInt&) const;
        bool operator!=(const BigInt&) const;
        bool operator<(const long long&) const;
        bool operator>(const long long&) const;
        bool operator<=(const long long&) const;
        bool operator>=(const long long&) const;
        bool operator==(const long long&) const;
        bool operator!=(const long long&) const;
        bool operator<(const std::string&) const;
        bool operator>(const std::string&) const;
        bool operator<=(const std::string&) const;
        bool operator>=(const std::string&) const;
        bool operator==(const std::string&) const;
        bool operator!=(const std::string&) const;

        // I/O stream operators:
        friend std::istream& operator>>(std::istream&, BigInt&);
        friend std::ostream& operator<<(std::ostream&, const BigInt&);

        // Conversion functions:
        std::string to_string() const;
        int to_int() const;
        long to_long() const;
        long long to_long_long() const;

        // Random number generating functions:
        friend BigInt big_random(size_t);
};

BigInt abs(const BigInt& num);
BigInt big_pow10(size_t exp);
BigInt pow(const BigInt& base, int exp);
BigInt pow(const long long& base, int exp);
BigInt pow(const std::string& base, int exp);
BigInt sqrt(const BigInt& num);
BigInt gcd(const BigInt& num1, const BigInt& num2);
BigInt gcd(const BigInt& num1, const long long& num2);
BigInt gcd(const BigInt& num1, const std::string& num2);
BigInt gcd(const long long& num1, const BigInt& num2);
BigInt gcd(const std::string& num1, const BigInt& num2);
BigInt lcm(const BigInt& num1, const BigInt& num2);
BigInt lcm(const BigInt& num1, const long long& num2);
BigInt lcm(const BigInt& num1, const std::string& num2);
BigInt lcm(const long long& num1, const BigInt& num2);
BigInt lcm(const std::string& num1, const BigInt& num2);

#ifdef _MSC_VER

#ifndef and
#define and &&
#endif // and

#ifndef or
#define or ||
#endif // or

#endif // _MSC_VER

#endif  // BIG_INT_HPP

