#ifndef UTILITIES_H
#define UTILITIES_H
///////////////////////////////////////////////////////////////////////
// Utilities.h - small, generally useful, helper classes             //
// ver 1.4                                                           //
// Language:    C++, Visual Studio 2015                              //
// Platform:    Dell XPS 8900, Windows 10                            //
// Application: Most Projects, CSE687 - Object Oriented Design       //
// Author:      Manish Gupta, Syracuse University,                   //
//              magupta@syr.edu                                      //
// Source:     Jim Fawcett, CST 4-187, Syracuse University           //
//                 (315) 443-3948, jfawcett@twcny.rr.com            //
///////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides classes StringHelper and Converter and a global
* function putline().  
* This package defines a single class, Convert<T> that supports convering
* instances of the T type to and from strings.  Also included are a few
* global functions intended to simplify code (slightly). This class will be 
* extended continuously for 
* awhile to provide convenience functions for general C++ applications.
*
* Build Process:
* --------------
* Required Files: Utilities.h, Utilities.cpp
*
* Build Command: devenv Utilities.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 1.4 : 28 Feb 2017
* - added convert class and some global functions 
* ver 1.3 : 12 Aug 2016
* - added overloads of title functions for std::ostringstreams
* ver 1.2 : 10 Aug 2016
* - added new function StringHelper::sTitle(...)
* - changed the way string arguments are passed to each of the
*   title functions
* ver 1.1 : 06 Feb 2015
* - fixed bug in split which turns a comma separated string into
*   a vector of tokens.
* - added comments
* ver 1.0 : 05 Feb 2016
* - first release
*
* Planned Additions and Changes:
* ------------------------------
* - none yet
*/
#include <string>
#include <vector>
#include <sstream>
#include <functional>
#include <iostream>
#include <algorithm>  
#include <cctype>
#include <locale>

namespace Utilities
{
  class StringHelper
  {
  public:
    static std::vector<std::string> split(const std::string& src);
    static void Title(std::string src, std::ostream&  out = std::cout);
    static void Title(std::string src, std::ostringstream& out);
    static void title(std::string src, std::ostream& out = std::cout);
    static void title(std::string src, std::ostringstream& out);
    static void sTitle(
      std::string src, size_t offset, size_t width, std::ostream& out = std::cout, char underline = '-'
    );
    static void sTitle(
      std::string src, size_t offset, size_t width, std::ostringstream& out, char underline = '-'
    );
  };

  void putline();

  template <typename T>
  class Converter
  {
  public:
    static std::string toString(const T& t);
    static T toValue(const std::string& src);
  };

  template <typename T>
  std::string Converter<T>::toString(const T& t)
  {
    std::ostringstream out;
    out << t;
    return out.str();
  }

  template<typename T>
  T Converter<T>::toValue(const std::string& src)
  {
    std::istringstream in(src);
    T t;
    in >> t;
    return t;
  }


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
	  out.precision(8);
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
	  in.precision(8);
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
  /*----< specialization for std::strings >------------------------*/

  template<>
  inline std::string Convert<std::string>::toString(const std::string& str)
  {
	  return str;
  }

  template<>
  inline std::string Convert<std::string>::fromString(const std::string& str)
  {
	  return str;
  }

  /////////////////////////////////////////////////////////////////////
  // Global functions that make code a little bit simpler

  inline void putLine() { std::cout << "\n"; };

  inline void putLines(size_t n) {
	  for (size_t i = 0; i < n; ++i)
		  putLine();
  };

  inline void titleCore(const std::string& msg, char underline = '-') {
	  std::cout << "\n  " << msg.c_str();
	  std::wcout << "\n " << std::string(msg.size() + 2, underline).c_str();
  };

  inline void Title(const std::string& msg) { titleCore(msg, '='); };

  inline void subTitle(const std::string& msg) { titleCore(msg, '-'); };

  /* string trim function:
  * -----------------------
  * Source from consultation with Dr. Google:
  * http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
  *
  * Note:
  * - Was originally written using std::isspace(char) function.
  * - isspace(char) fails with exception on certain non-ascii
  *   characters. For example, some of the Chinese characters will do that.
  * - To fix need to use isspace(char, std::locale). You can't directly
  *   do that with the original code, due to the way it's using std algorithms,
  *   but it was easy to fix with a helper function.
  * - You will get a compiler warning about returning a reference.  However,
  *   that is not an error, since the functions just modify a string passed in
  *   by reference.
  * - Since std::string now has a move constructor and move assignment, it
  *   may be almost as fast to return by value.  I plan to test that sometime.
  */
  static inline int isspaceHelper(int ch)
  {
	  std::locale loc;
	  return std::isspace(ch, loc);
  }
  // trim from start
  static inline std::string &ltrim(std::string &s) {
	  s.erase(s.begin(), std::find_if(s.begin(), s.end(),
		  std::not1(std::ptr_fun<int, int>(isspaceHelper))));
	  return s;
  }

  // trim from end
  static inline std::string &rtrim(std::string &s) {
	  s.erase(std::find_if(s.rbegin(), s.rend(),
		  std::not1(std::ptr_fun<int, int>(isspaceHelper))).base(), s.end());
	  return s;
  }

  // trim from both ends
  inline std::string &trim(std::string &s) {
	  return ltrim(rtrim(s));
  }

}

#endif
