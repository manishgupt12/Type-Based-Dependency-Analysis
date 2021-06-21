#pragma once
/////////////////////////////////////////////////////////////////////
// Convert.h - Implement serialization to and from strings         //
// ver 1.1                                                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017       //
/////////////////////////////////////////////////////////////////////
/*
 * Package Operations:
 * -------------------
 * This package defines a single class, Convert<T> with methods:
 * - std::string toString(const T&t t)      converts t to a string
 * - T fromString(const std::string& str)   converts str to an instance of T
 * - std::string id(const T& t)             returns the typeid().Name of t
 *
 * Required Files:
 * ---------------
 *   Convert.h, Convert.cpp, Utilities.h, Utilities.cpp
 *
 * Maintanence History:
 * --------------------
 * ver 1.1 : 05 Feb 2017
 * - added comments
 * ver 1.0 : 28 Jan 2017
 * - first release
 */
#include <sstream>
#include <string>

/////////////////////////////////////////////////////////////////////
// Convert<T> class
// - (de)serializes objects of type T (from)to strings
// - generates a type identifier using Run-Time Type Identification
//
template<typename T>
class Convert
{
public:
  virtual ~Convert<T>() {}
  static std::string toString(const T& t);
  static T fromString(const std::string& str);
  static std::string id(const T& t);
};
/*----< serialize t to a std::string >-----------------------------*/
/*
 * Assumes that T defines an insertion operator.
 */
template<typename T>
std::string Convert<T>::toString(const T& t)
{
  std::ostringstream out;
  out << *const_cast<T*>(&t);  
  return out.str();
}
/*----< create an instance of T from a std::string >---------------*/
/*
 * Assumes that T defines an extraction operator.
 * To succeed str must be created from Convert<T>::toString(const T& t). 
 */
template<typename T>
T Convert<T>::fromString(const std::string& str)
{
  std::istringstream in(str);
  T value;
  in >> value;    // istringstream extraction provides the conversion
  return value;
}
/*----< create an id string from an instance of T using RTTI >----*/

template<typename T>
std::string Convert<T>::id(const T& t)
{
  return typeid(t).name();
}
