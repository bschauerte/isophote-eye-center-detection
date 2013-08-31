#pragma once

#include <string>

/** Template to convert a type into a string (e.g. for debugging output of templates). */
template <typename T> std::string TypeToString(void);

///
// Template instantiations of TypeToString
///
// floating point types
template <> inline std::string TypeToString<float>(void) { return "float"; }
template <> inline std::string TypeToString<double>(void) { return "double"; }
template <> inline std::string TypeToString<long double>(void) { return "long double"; }
// integer types
template <> inline std::string TypeToString<long int>(void) { return "long int"; }
template <> inline std::string TypeToString<int>(void) { return "int"; }
template <> inline std::string TypeToString<short>(void) { return "short"; }
template <> inline std::string TypeToString<char>(void) { return "char"; }
template <> inline std::string TypeToString<unsigned long int>(void) { return "unsigned long int"; }
template <> inline std::string TypeToString<unsigned int>(void) { return "unsigned int"; }
template <> inline std::string TypeToString<unsigned short>(void) { return "unsigned short"; }
template <> inline std::string TypeToString<unsigned char>(void) { return "unsigned char"; }
// stuff types
template <> inline std::string TypeToString<std::string>(void) { return "std::string"; }
