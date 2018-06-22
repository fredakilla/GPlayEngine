#ifndef _BASE_64_
# define _BASE_64_
# include <string>
# include <fstream>
  std::string base64_encode(unsigned char const *, unsigned int);
  std::string base64_decode(std::string const &);
  bool base64_encode_file(std::string);
  bool base64_decode_file(std::string);
#endif
