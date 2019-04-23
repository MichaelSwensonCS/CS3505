/*
	Author: Michael Swenson
	1/26/2019
	v0.0
	Created for CS3505 Assignment #2
	
	This class can create big_integer objects that represent extremely large integers.
	These objects behave like integers mathematically and use +-/*%.  
	
	Note: This class expects non-negative numbers and non-null strings, there is 
	no input sanitization.
	
	Basic testing and big_integer usage examples can be found in tester.cpp
*/


#include "big_integer.h"
#include <string>
#include <stdlib.h>

using namespace std;

//Big_integer representation
string number = "";


/************** Constructors *********/
//Default ctor sets value of big_int to 0
big_integer::big_integer()
{
    number = "0";
}

//Gets value of created big_int as a string
big_integer::big_integer(string number)
{
    this->number = number;
}

//copy ctor
big_integer::big_integer(const big_integer& other)
{
    number = other.number;
}

//Returns the value of the big_integer
const string big_integer::get_value() const
{
    return this->number;
}



/******** Overloaded Operators ***********/

//Standard overloaded assignment operator
big_integer& big_integer::operator=(const big_integer& rhs)
{
    number = rhs.number;
}

//Add two big_integers, majority of logic in add_two_strings
const big_integer big_integer::operator+(const big_integer& rhs) const
{
    string left_value = this->number;
    string right_value = rhs.number;

    string result = "";

    result = add_two_strings(left_value, right_value);

    big_integer sum(result);

    return sum;
}

//Subtract two big integers, majority of logic in subtract_two_strings
const big_integer big_integer::operator-(const big_integer& rhs) const
{

    string left_value = this->number;
    string right_value = rhs.number;
    string result = "";
	
    result = big_integer::subtract_two_strings(left_value, right_value);
    big_integer difference(result);
    return difference;
}

//Multiply two big integers, majority of logic in multiply_two_strings
const big_integer big_integer::operator*(const big_integer& rhs) const
{
    string left_value = this->number;
    string right_value = rhs.number;

    string result = ""; 
    result = big_integer::multiply_two_strings(left_value, right_value);
    big_integer product(result);

    return product;
}

//Divide two big integers, majority of logic in divide_two_strings
const big_integer big_integer::operator/(const big_integer& rhs) const
{
    string left_value = this->number;
	string right_value = rhs.number;
	
	string result = "";
	result = big_integer::divide_two_strings(left_value, right_value);
	
	big_integer quotient(result);
    return quotient;
}
//Modulus two big integers, majority of logic in mod_two_strings
const big_integer big_integer::operator%(const big_integer& rhs) const
{
    string left_value = this->number;
	string right_value = rhs.number;
	
	string result = "";
	result = big_integer::mod_two_strings(left_value, right_value);
	
	big_integer remainder(result);
    return remainder;
}


/********** HELPER FUNCTIONS **************/


//Provided by Dr.Jensen for student use
string big_integer::add_two_strings(string left, string right){
	// Start with an empty string, then build up a string to contain the
	string result = "";

    // We'll go through digits right-to-left, so start at the end.
    int left_pos  = left.size()  - 1;
    int right_pos = right.size() - 1;

    // Loop, adding columns until no more digits or carry remain.
    int carry = 0;
    while (left_pos >= 0 || right_pos >= 0 || carry > 0)
    {
        // Calculate the sum for one column of digits.  (Digits remain
        //    so long as positions are not less than zero.)
        int sum = 0;
        if (left_pos >= 0)
        sum += left[left_pos] - '0';  // Remove the ASCII bias as we add
        if (right_pos >= 0)
        sum += right[right_pos] - '0';  // Remove the ASCII bias as we add
        sum += carry;
        // Determine if there is now a carry, confine the sum to a single digit.
        carry = sum / 10;
        sum   = sum % 10;
        // Put the sum into the new string (at the left side)
        result.insert (0, 1, (char)('0'+sum) );  // Add in an ASCII bias before storing the char
        // Move the positions to the prior column of each number.  (Negative positions indicate we've
        //   taken care of all the columns in some number.)
        left_pos--;
        right_pos--;
    }
    // Strip out any leading 0's from our result (but leave at least one digit).
    //   (Only useful for subtraction, but I'm giving it to you here.)
    while (result.size() > 1 && result[0] == '0')
        result.erase(0, 1);

    return result;
}

/*
  Iterate through the multiplier to see how many times we are going to add
  this number to itself
  
  contract: not null, valid_input: non-negative integers
*/
 
//Provided by Dr.Jensen for student use
string big_integer::multiply_two_strings (string number, string multiplier)
{
	// Perform long multiplication.  The result is the working sum and
	//   will be the product.  Do a small example on paper to see this
	//   algorithm work.
	string product = "0";
	// Loop through the digits of the multiplier in MSD to LSD order.
	for (int multiplier_pos = 0; multiplier_pos < multiplier.size(); multiplier_pos++)
	{
	// Multiply the product by 10.
	product.append("0");
	// Add in the correct number of copies of the number.  (Slow algorithm.)
	//   Notice that I count in ASCII.  ;)
	for (int i = '0'; i < multiplier[multiplier_pos]; i++)
	  product = big_integer::add_two_strings(product, number);
	}
	return product;
}

//Check to see if left is bigger than right numerically
//contract: valid input: not null, not negative integers
bool big_integer::is_bigger(string left, string right)
{
  if(left.size() > right.size())
    return true;
  for(int i = 0; i < left.size(); i++)
  {
	  if(left[i] == right[i])
		continue;
	  else
		return left[i] > right[i];
  }
}

/*
	Operates the same as subtraction by hand keep track of difference
	and if it is less than 0 add ten to it and subtract one from the next 
	digit
	
	contract: non-negative integers and not null strings
*/
//Modified from Dr.Jensen's add_two_strings
string big_integer::subtract_two_strings(string left, string right)
{
  if(big_integer::is_bigger(right,left))
     return "0";

     // Start with an empty string, then build up a string to contain the
    string result = "";

    // We'll go through digits right-to-left, so start at the end.
    int left_pos  = left.size()  - 1;
    int right_pos = right.size() - 1;

    // Loop, adding columns until no more digits or carry remain.
    int carry = 0;
    while (left_pos >= 0 || right_pos >= 0 || carry > 0)
    {
        // Calculate the difference for one column of digits.  (Digits remain
        //    so long as positions are not less than zero.)
        int difference = 0;
        if (left_pos >= 0)
        difference += left[left_pos] - '0';  // Remove the ASCII bias as we add
        if (right_pos >= 0)
        difference -= right[right_pos] - '0';  // Remove the ASCII bias as we add
	if(carry > 0)
	  {
	    difference -= carry;
	    carry -= 1;
	  }

	if(difference < 0)
	  {
	  difference += 10;
	  carry += 1;
	  }
	  
        // Put the difference into the new string (at the left side)
        result.insert (0, 1, (char)('0'+difference) );  // Add in an ASCII bias before storing the char
        // Move the positions to the prior column of each number.  (Negative positions indicate we've
        //   taken care of all the columns in some number.)
        left_pos--;
        right_pos--;
    }
    // Strip out any leading 0's from our result (but leave at least one digit).
    //   (Only useful for subtraction, but I'm giving it to you here.)
    while (result.size() > 1 && result[0] == '0')
        result.erase(0, 1);

   return result;
}

/*
	Check to see if the divisor is bigger then the dividend if so just return 0 else
	multiply divisor by 10 until it is the correct size to subtract from the dividend. 
	After subtraction divide divisor by 10 and check the new divisor against the new dividend.
	Repeat subtraction and /10 until the dividend is less then the original divisor
	
	This method gives you both quotient and remainder.
	
	contract: non-negative integers, non-null strings
*/
string big_integer::divide_two_strings(string dividend, string divisor)
{		
  string quotient = "";
  string temp = "";
		
  if(big_integer::is_bigger(divisor, dividend))
	{
      quotient = "0";
      return quotient;
	}
	
	string zeroesContributed = "";
	int quotientCounter = 0;

	//Set up how big the divisor will need to be
   while(big_integer::is_bigger(dividend, divisor))
   {
	   divisor.push_back('0');
	   zeroesContributed.append("0");
   }
	//Keep subtracting, dividing the divisor by 10, and adding the next 10's place to the quotient
   	while(true)
	{
	   //Divide by 10
   	   divisor.erase(divisor.size() - 1);
	   zeroesContributed.erase(zeroesContributed.size() - 1);
	   
	   quotientCounter = 0;
	   
	    //Keep subtracting quotientCounter should be 0-9
		while(big_integer::is_bigger(dividend, divisor))
		{
			dividend = subtract_two_strings(dividend,divisor);
			quotientCounter++;
		}
		//Create the string to add to the quotient
		temp = (char)('0' + quotientCounter) + zeroesContributed;
		//Numerically add the newly calcuated 10's place to the total quotient
		quotient = add_two_strings(quotient,temp);
		
		//If there are no more appended 0's we are back to the original divisor and done
		if(zeroesContributed == "")
		{
			return quotient;
		}	
	}
}


/*
	Check to see if the divisor is bigger then the dividend if so just return 0 else
	multiply divisor by 10 until it is the correct size to subtract from the dividend. 
	After subtraction divide divisor by 10 and check the new divisor against the new dividend.
	Repeat subtraction and /10 until the dividend is less then the original divisor
	
	This method gives you both quotient and remainder.
	
	FUTURE USE NOTE:  This function is heavily redundant I tried implementing a flag
	that divide_two_strings would use and then the overloaded operators / % would set the
	flag and that would determine if divide_two_strings output quotient or remainder
	
	However, I was having scope issues that I felt wouldn't be worth the time to solve
	due to the deadline closing in.
	
	contract: non-negative integers, non-null strings
*/
string big_integer::mod_two_strings(string dividend, string divisor)
{		
  string quotient = "";
  string temp = "";
	

  //If divisor is bigger just return dividend	
  if(big_integer::is_bigger(divisor, dividend))
  {
      return dividend;
  }
	
	string zeroesContributed = "";
	int quotientCounter = 0;

	//Set up how big the divisor will need to be
   while(big_integer::is_bigger(dividend, divisor))
   {
	   divisor.push_back('0');
	   zeroesContributed.append("0");
   }
   
   	while(true)
	{
	   //Divide by 10
   	   divisor.erase(divisor.size() - 1);
	   zeroesContributed.erase(zeroesContributed.size() - 1);
	 
	   quotientCounter = 0;
	   
	    //Keep subtracting quotientCounter should be 0-9
		while(big_integer::is_bigger(dividend, divisor))
		{
			dividend = subtract_two_strings(dividend,divisor);
			quotientCounter++;
		}
		//Create the string to add to the quotient
		temp = (char)('0' + quotientCounter) + zeroesContributed;
		
		//Numerically add the newly calcuated 10's place to the total quotient
		quotient = add_two_strings(quotient,temp);
		
		//If there are no more appended 0's we are back to the original divisor and done
		if(zeroesContributed == "")
		{
			return dividend;
		}	
	}
}
