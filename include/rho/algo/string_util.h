#ifndef __rho_algo_string_util_h__
#define __rho_algo_string_util_h__


#include <rho/ppcheck.h>

#include <string>
#include <vector>


namespace rho
{
namespace algo
{


/**
 * Finds the last occurrence of '/' or '\' within 'path' and
 * returns the content previous to that character (and including
 * that character).
 */
std::string findDirName(std::string path);

/**
 * Removes the first occurrence of the 'leader' and removes
 * the part of 'str' after that occurrence.
 */
std::string stripComment(std::string str, std::string leader ="//");

/**
 * Trims the whitespace at the beginning and end of 'str'.
 */
std::string trim(std::string str);

/**
 * Returns true if 'str' contains any whitspace characters.
 */
bool containsWhitespace(std::string str);

/**
 * Replaces all occurrences of 'from' with 'to' within 'str'.
 */
std::string replace(std::string str, std::string from, std::string to);

/**
 * Splits 'str' into a vector of strings at each occurrence of 'delim'.
 * No string in the vector will contain 'delim' (unless there are more
 * occurrences of 'delim' than 'maxparts'-1).
 *
 * If 'maxparts' is less than 1, it is considered to be infinity.
 * If 'maxparts' is not infinity, then the vector returned will have
 * at most 'maxparts' strings in it.
 * Therefore, the vector returned always contains one or more parts.
 */
std::vector<std::string> split(std::string str, std::string delim, int maxparts = -1);

/**
 * Removes all strings within 'parts' that have length zero.
 */
std::vector<std::string> removeEmptyParts(std::vector<std::string> parts);

/**
 * Converts a string representation of a numerical value to a double.
 * Sets '*errorFlag' to true if an error occurs.
 */
double toDouble(std::string str, bool* errorFlag);

/**
 * Converts a string representation of a numerical value to an integer.
 * Sets '*errorFlag' to true if an error occurs.
 */
int toInt(std::string str, bool* errorFlag);


}   // namespace algo
}   // namespace rho


#endif   // __rho_algo_string_util_h__
