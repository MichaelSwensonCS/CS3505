#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <string>

class big_integer
{
    public:
		//ctors
        big_integer();
        big_integer(std::string);
        big_integer(const big_integer& other);
		
        big_integer& operator=(const big_integer& other);
			
		const big_integer operator+(const big_integer& other) const;
		const big_integer operator-(const big_integer& other) const;
		const big_integer operator*(const big_integer& other) const;
		const big_integer operator/(const big_integer& other) const;
		const big_integer operator%(const big_integer& other) const;

		std::string const get_value() const;

    private:
		static bool is_bigger(std::string left, std::string right);
		
		static std::string add_two_strings(std::string left, std::string right);
		static std::string subtract_two_strings(std::string left, std::string right);
		static std::string multiply_two_strings(std::string left, std::string right);
		static std::string divide_two_strings(std::string left, std::string right);
		static std::string mod_two_strings(std::string left, std::string right);

		std::string number;
};

#endif
