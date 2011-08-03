/* */

#include "util.h"
#include <unistd.h>
#include <cerrno>
#include <openssl/md5.h> // from crypto lib

using namespace std;

/** Convert bytes to its hexidecimal string form */
string hexString (unsigned char* bytes, unsigned bytesLen) {
	char hex[2 * bytesLen];
	char* hexp = hex;
	for (int i=0; i < bytesLen; i++) {
		sprintf (hexp, "%02x", bytes[i]);
		hexp += 2;
	}
	return string (hex, 2 * bytesLen);
}

/** Return md5 hash of string as hex string */
string md5 (std::string s) {
	unsigned char hash[MD5_DIGEST_LENGTH];
	const unsigned char* c = (const unsigned char*)s.data();
	MD5 (c, s.size(), hash);
	return hexString (hash, MD5_DIGEST_LENGTH);
}

/** Tokenize string using given char as delimeter */
vector<string> split_string (char delimeter, string str) {
	vector<string> tokens;
	string::size_type i = 0;
	string::size_type j = 0;
	while ((j = str.find (delimeter, j)) != string::npos) {
		tokens.push_back (str.substr (i, j-i));
		i = ++j;
	}
	tokens.push_back (str.substr (i, str.length() - i));
	return tokens;
}
