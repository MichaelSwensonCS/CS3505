/*
 * Author: Michael Swenson
 * Date: 1/11/19
 * Class: CS3505
 * 
 * Assignment 1b: This class will read in a valid integer from the command line 
 * and check if it is a prime number. If it is the program will print "prime" otherwise
 * it will print "not prime".  
*/


#include <iostream>

//For atoi 
#include <cstdlib>

int main (int argc, char **argv)
{
  //Evaluated Number
  int prime;
  bool isPrime = true;

  prime = std::atoi(argv[1]);

  
  if(prime == 0 || prime == 1)
  {
	  std::cout << "not prime" << std::endl;
      isPrime = false;
	  return 0;
  }
  
  //Check all numbers up to  the number and check if it divides evenly 
  //(For optimization  the max check could be sqrt(prime) but this program is meant to be simple
  for(int i = 2; i < prime; i++)
  {
    if(prime % i == 0)
    {
      //If it divides evenly, it is not prime
      std::cout << "not prime" << std::endl;
      isPrime = false;
      break;
    }
  }
  //If none of the numbers divide evenly
  if(isPrime)
  {
    std::cout << "prime" << std::endl;
  }
  return 0;
}

