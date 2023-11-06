#include "HttpUtility.h"
namespace HttpUtility {

	unsigned char __base64__alphabet_map[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	unsigned char __base64__reverse_map[] = {
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 62, 255, 255, 255, 63,
		52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 255, 255, 255, 255, 255, 255,
		255,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
		15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 255, 255, 255, 255, 255,
		255, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
		41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 255, 255, 255, 255, 255
	};

	std::string base64_encode(const unsigned char *text, unsigned int text_len)
	{
		unsigned int i, j;
		std::string base64Str;
		for (i = 0, j = 0; i + 3 <= text_len; i += 3)
		{
			base64Str += __base64__alphabet_map[text[i] >> 2];                             //取出第一个字符的前6位并找出对应的结果字符
			base64Str += __base64__alphabet_map[((text[i] << 4) & 0x30) | (text[i + 1] >> 4)];     //将第一个字符的后2位与第二个字符的前4位进行组合并找到对应的结果字符
			base64Str += __base64__alphabet_map[((text[i + 1] << 2) & 0x3c) | (text[i + 2] >> 6)];   //将第二个字符的后4位与第三个字符的前2位组合并找出对应的结果字符
			base64Str += __base64__alphabet_map[text[i + 2] & 0x3f];                         //取出第三个字符的后6位并找出结果字符
		}
		if (i < text_len)
		{
			unsigned int tail = text_len - i;
			if (tail == 1)
			{
				base64Str += __base64__alphabet_map[text[i] >> 2];
				base64Str += __base64__alphabet_map[(text[i] << 4) & 0x30];
				base64Str += '=';
				base64Str += '=';
			}
			else //tail==2
			{
				base64Str += __base64__alphabet_map[text[i] >> 2];
				base64Str += __base64__alphabet_map[((text[i] << 4) & 0x30) | (text[i + 1] >> 4)];
				base64Str += __base64__alphabet_map[(text[i + 1] << 2) & 0x3c];
				base64Str += '=';
			}
		}
		return base64Str;
	}
	std::string  base64_decode(const unsigned char *code, unsigned int code_len)
	{
		//assert();  //如果它的条件返回错误，则终止程序执行。4的倍数。
		if ((code_len & 0x03) == 0) {
			return "";
		}
		std::string str;

		unsigned int i, j = 0;
		unsigned char quad[4];
		for (i = 0; i < code_len; i += 4)
		{
			for (unsigned int k = 0; k < 4; k++)
			{
				quad[k] = __base64__reverse_map[code[i + k]];//分组，每组四个分别依次转换为base64表内的十进制数
			}

			//assert(quad[0] < 64 && quad[1] < 64);
			if (quad[0] < 64 && quad[1] < 64) {
				return "";
			}

			str += (quad[0] << 2) | (quad[1] >> 4); //取出第一个字符对应base64表的十进制数的前6位与第二个字符对应base64表的十进制数的前2位进行组合

			if (quad[2] >= 64)
				break;
			else if (quad[3] >= 64)
			{
				str += (quad[1] << 4) | (quad[2] >> 2); //取出第二个字符对应base64表的十进制数的后4位与第三个字符对应base64表的十进制数的前4位进行组合
				break;
			}
			else
			{
				str += (quad[1] << 4) | (quad[2] >> 2);
				str += (quad[2] << 6) | quad[3];//取出第三个字符对应base64表的十进制数的后2位与第4个字符进行组合
			}
		}
		return str;
	}


	unsigned char ToHex(unsigned char x)
	{
		return  x > 9 ? x + 55 : x + 48;
	}
	unsigned char FromHex(unsigned char x)
	{
		unsigned char y;
		if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
		else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
		else if (x >= '0' && x <= '9') y = x - '0';
		else assert(0);
		return y;
	}
	std::string UrlEncode(const std::string& str)
	{
		std::string strTemp = "";
		size_t length = str.length();
		for (size_t i = 0; i < length; i++)
		{
			if (isalnum((unsigned char)str[i]) ||
				(str[i] == '-') ||
				(str[i] == '_') ||
				(str[i] == '.') ||
				(str[i] == '~'))
				strTemp += str[i];
			else if (str[i] == ' ')
				strTemp += "+";
			else
			{
				strTemp += '%';
				strTemp += ToHex((unsigned char)str[i] >> 4);
				strTemp += ToHex((unsigned char)str[i] % 16);
			}
		}
		return strTemp;
	}
	std::string UrlDecode(const std::string& str)
	{
		std::string strTemp = "";
		size_t length = str.length();
		for (size_t i = 0; i < length; i++)
		{
			if (str[i] == '+') strTemp += ' ';
			else if (str[i] == '%')
			{
				assert(i + 2 < length);
				unsigned char high = FromHex((unsigned char)str[++i]);
				unsigned char low = FromHex((unsigned char)str[++i]);
				strTemp += high * 16 + low;
			}
			else strTemp += str[i];
		}
		return strTemp;
	}


	typedef unsigned char byte;
	typedef unsigned int uint32;
	typedef unsigned int uint4;
	using std::string;
	using std::ifstream;
	namespace MD5 {
		/* MD5 declaration. */
		class MD5 {
		public:
			MD5();
			MD5(const void *input, size_t length);
			MD5(const string &str);
			MD5(ifstream &in);
			void update(const void *input, size_t length);
			void update(const string &str);
			void update(ifstream &in);
			const byte* digest();
			string toString();
			void reset();

			inline uint4 rotate_left(uint4 x, int n);
			inline void FF(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
			inline void GG(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
			inline void HH(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
			inline void II(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);

		private:
			void update(const byte *input, size_t length);
			void final();
			void transform(const byte block[64]);
			void encode(const uint32 *input, byte *output, size_t length);
			void decode(const byte *input, uint32 *output, size_t length);
			string bytesToHexString(const byte *input, size_t length);

			/* class uncopyable */
			MD5(const MD5&);
			MD5& operator=(const MD5&);
		private:
			uint32 _state[4]; /* state (ABCD) */
			uint32 _count[2]; /* number of bits, modulo 2^64 (low-order word first) */
			byte _buffer[64]; /* input buffer */
			byte _digest[16]; /* message digest */
			bool _finished; /* calculate finished ? */

			static const size_t BUFFER_SIZE = 1024;

			const byte PADDING[64] = { 0x80 };
			const char HEX[16] = {
			  '0', '1', '2', '3',
			  '4', '5', '6', '7',
			  '8', '9', 'a', 'b',
			  'c', 'd', 'e', 'f'
			};

		};
		string FileDigest(const string &file);
		static std::string GetMd5FromFile(const std::string & filename) {
			ifstream fs(filename.c_str(), std::ios::binary);
			MD5 md5(fs);
			fs.close();
			return md5.toString();
		}

		static std::string GetMd5FromString(const std::string & string) {
			MD5 md5(string);
			return md5.toString();
		}

		/* Constants for MD5Transform routine. */
#define S11 7 
#define S12 12 
#define S13 17 
#define S14 22 
#define S21 5 
#define S22 9 
#define S23 14 
#define S24 20 
#define S31 4 
#define S32 11 
#define S33 16 
#define S34 23 
#define S41 6 
#define S42 10 
#define S43 15 
#define S44 21 

/* F, G, H and I are basic MD5 functions.
*/
#define F(x, y, z) (((x) & (y)) | ((~x) & (z))) 
#define G(x, y, z) (((x) & (z)) | ((y) & (~z))) 
#define H(x, y, z) ((x) ^ (y) ^ (z)) 
#define I(x, y, z) ((y) ^ ((x) | (~z))) 

/* ROTATE_LEFT rotates x left n bits.
*/
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n)))) 

		inline uint4 MD5::rotate_left(uint4 x, int n) {
			return (x << n) | (x >> (32 - n));
		}

		inline void MD5::FF(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac) {
			a = rotate_left(a + F(b, c, d) + x + ac, s) + b;
		}

		inline void MD5::GG(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac) {
			a = rotate_left(a + G(b, c, d) + x + ac, s) + b;
		}

		inline void MD5::HH(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac) {
			a = rotate_left(a + H(b, c, d) + x + ac, s) + b;
		}

		inline void MD5::II(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac) {
			a = rotate_left(a + I(b, c, d) + x + ac, s) + b;
		}


		/* Default construct. */
		inline MD5::MD5() {
			reset();
		}

		/* Construct a MD5 object with a input buffer. */
		inline MD5::MD5(const void *input, size_t length) {
			reset();
			update(input, length);
		}

		/* Construct a MD5 object with a string. */
		inline MD5::MD5(const string &str) {
			reset();
			update(str);
		}

		/* Construct a MD5 object with a file. */
		inline MD5::MD5(ifstream &in) {
			reset();
			update(in);
		}

		/* Return the message-digest */
		inline const byte* MD5::digest() {
			if (!_finished) {
				_finished = true;
				final();
			}
			return _digest;
		}

		/* Reset the calculate state */
		inline void MD5::reset() {

			_finished = false;
			/* reset number of bits. */
			_count[0] = _count[1] = 0;
			/* Load magic initialization constants. */
			_state[0] = 0x67452301;
			_state[1] = 0xefcdab89;
			_state[2] = 0x98badcfe;
			_state[3] = 0x10325476;
		}

		/* Updating the context with a input buffer. */
		inline void MD5::update(const void *input, size_t length) {
			update((const byte*)input, length);
		}

		/* Updating the context with a string. */
		inline void MD5::update(const string &str) {
			update((const byte*)str.c_str(), str.length());
		}

		/* Updating the context with a file. */
		inline void MD5::update(ifstream &in) {

			if (!in)
				return;

			std::streamsize length;
			char buffer[BUFFER_SIZE];
			while (!in.eof()) {
				in.read(buffer, BUFFER_SIZE);
				length = in.gcount();
				if (length > 0)
					update(buffer, length);
			}
			in.close();
		}

		/* MD5 block update operation. Continues an MD5 message-digest
		operation, processing another message block, and updating the
		context.
		*/
		inline void MD5::update(const byte *input, size_t length) {

			uint32 i, index, partLen;

			_finished = false;

			/* Compute number of bytes mod 64 */
			index = (uint32)((_count[0] >> 3) & 0x3f);

			/* update number of bits */
			if ((_count[0] += ((uint32)length << 3)) < ((uint32)length << 3))
				_count[1]++;
			_count[1] += ((uint32)length >> 29);

			partLen = 64 - index;

			/* transform as many times as possible. */
			if (length >= partLen) {

				memcpy(&_buffer[index], input, partLen);
				transform(_buffer);

				for (i = partLen; i + 63 < length; i += 64)
					transform(&input[i]);
				index = 0;

			}
			else {
				i = 0;
			}

			/* Buffer remaining input */
			memcpy(&_buffer[index], &input[i], length - i);
		}

		/* MD5 finalization. Ends an MD5 message-_digest operation, writing the
		the message _digest and zeroizing the context.
		*/
		inline void MD5::final() {

			byte bits[8];
			uint32 oldState[4];
			uint32 oldCount[2];
			uint32 index, padLen;

			/* Save current state and count. */
			memcpy(oldState, _state, 16);
			memcpy(oldCount, _count, 8);

			/* Save number of bits */
			encode(_count, bits, 8);

			/* Pad out to 56 mod 64. */
			index = (uint32)((_count[0] >> 3) & 0x3f);
			padLen = (index < 56) ? (56 - index) : (120 - index);
			update(PADDING, padLen);

			/* Append length (before padding) */
			update(bits, 8);

			/* Store state in digest */
			encode(_state, _digest, 16);

			/* Restore current state and count. */
			memcpy(_state, oldState, 16);
			memcpy(_count, oldCount, 8);
		}

		/* MD5 basic transformation. Transforms _state based on block. */
		inline void MD5::transform(const byte block[64]) {

			uint32 a = _state[0], b = _state[1], c = _state[2], d = _state[3], x[16];

			decode(block, x, 64);

			/* Round 1 */
			FF(a, b, c, d, x[0], S11, 0xd76aa478); /* 1 */
			FF(d, a, b, c, x[1], S12, 0xe8c7b756); /* 2 */
			FF(c, d, a, b, x[2], S13, 0x242070db); /* 3 */
			FF(b, c, d, a, x[3], S14, 0xc1bdceee); /* 4 */
			FF(a, b, c, d, x[4], S11, 0xf57c0faf); /* 5 */
			FF(d, a, b, c, x[5], S12, 0x4787c62a); /* 6 */
			FF(c, d, a, b, x[6], S13, 0xa8304613); /* 7 */
			FF(b, c, d, a, x[7], S14, 0xfd469501); /* 8 */
			FF(a, b, c, d, x[8], S11, 0x698098d8); /* 9 */
			FF(d, a, b, c, x[9], S12, 0x8b44f7af); /* 10 */
			FF(c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
			FF(b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
			FF(a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
			FF(d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
			FF(c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
			FF(b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

								/* Round 2 */
			GG(a, b, c, d, x[1], S21, 0xf61e2562); /* 17 */
			GG(d, a, b, c, x[6], S22, 0xc040b340); /* 18 */
			GG(c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
			GG(b, c, d, a, x[0], S24, 0xe9b6c7aa); /* 20 */
			GG(a, b, c, d, x[5], S21, 0xd62f105d); /* 21 */
			GG(d, a, b, c, x[10], S22, 0x2441453); /* 22 */
			GG(c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
			GG(b, c, d, a, x[4], S24, 0xe7d3fbc8); /* 24 */
			GG(a, b, c, d, x[9], S21, 0x21e1cde6); /* 25 */
			GG(d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
			GG(c, d, a, b, x[3], S23, 0xf4d50d87); /* 27 */
			GG(b, c, d, a, x[8], S24, 0x455a14ed); /* 28 */
			GG(a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
			GG(d, a, b, c, x[2], S22, 0xfcefa3f8); /* 30 */
			GG(c, d, a, b, x[7], S23, 0x676f02d9); /* 31 */
			GG(b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

								/* Round 3 */
			HH(a, b, c, d, x[5], S31, 0xfffa3942); /* 33 */
			HH(d, a, b, c, x[8], S32, 0x8771f681); /* 34 */
			HH(c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
			HH(b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
			HH(a, b, c, d, x[1], S31, 0xa4beea44); /* 37 */
			HH(d, a, b, c, x[4], S32, 0x4bdecfa9); /* 38 */
			HH(c, d, a, b, x[7], S33, 0xf6bb4b60); /* 39 */
			HH(b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
			HH(a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
			HH(d, a, b, c, x[0], S32, 0xeaa127fa); /* 42 */
			HH(c, d, a, b, x[3], S33, 0xd4ef3085); /* 43 */
			HH(b, c, d, a, x[6], S34, 0x4881d05); /* 44 */
			HH(a, b, c, d, x[9], S31, 0xd9d4d039); /* 45 */
			HH(d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
			HH(c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
			HH(b, c, d, a, x[2], S34, 0xc4ac5665); /* 48 */

								/* Round 4 */
			II(a, b, c, d, x[0], S41, 0xf4292244); /* 49 */
			II(d, a, b, c, x[7], S42, 0x432aff97); /* 50 */
			II(c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
			II(b, c, d, a, x[5], S44, 0xfc93a039); /* 52 */
			II(a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
			II(d, a, b, c, x[3], S42, 0x8f0ccc92); /* 54 */
			II(c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
			II(b, c, d, a, x[1], S44, 0x85845dd1); /* 56 */
			II(a, b, c, d, x[8], S41, 0x6fa87e4f); /* 57 */
			II(d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
			II(c, d, a, b, x[6], S43, 0xa3014314); /* 59 */
			II(b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
			II(a, b, c, d, x[4], S41, 0xf7537e82); /* 61 */
			II(d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
			II(c, d, a, b, x[2], S43, 0x2ad7d2bb); /* 63 */
			II(b, c, d, a, x[9], S44, 0xeb86d391); /* 64 */

			_state[0] += a;
			_state[1] += b;
			_state[2] += c;
			_state[3] += d;
		}

		/* Encodes input (ulong) into output (byte). Assumes length is
		a multiple of 4.
		*/
		inline void MD5::encode(const uint32 *input, byte *output, size_t length) {

			for (size_t i = 0, j = 0; j < length; i++, j += 4) {
				output[j] = (byte)(input[i] & 0xff);
				output[j + 1] = (byte)((input[i] >> 8) & 0xff);
				output[j + 2] = (byte)((input[i] >> 16) & 0xff);
				output[j + 3] = (byte)((input[i] >> 24) & 0xff);
			}
		}

		/* Decodes input (byte) into output (ulong). Assumes length is
		a multiple of 4.
		*/
		inline void MD5::decode(const byte *input, uint32 *output, size_t length) {

			for (size_t i = 0, j = 0; j < length; i++, j += 4) {
				output[i] = ((uint32)input[j]) | (((uint32)input[j + 1]) << 8) |
					(((uint32)input[j + 2]) << 16) | (((uint32)input[j + 3]) << 24);
			}
		}

		/* Convert byte array to hex string. */
		inline string MD5::bytesToHexString(const byte *input, size_t length) {
			string str;
			str.reserve(length << 1);
			for (size_t i = 0; i < length; i++) {
				int t = input[i];
				int a = t / 16;
				int b = t % 16;
				str.append(1, HEX[a]);
				str.append(1, HEX[b]);
			}
			return str;
		}

		/* Convert digest to string value */
		inline string MD5::toString() {
			return bytesToHexString(digest(), 16);
		}


		//得到二进制文件的MD5码
		inline string FileDigest(const string &file) {
			ifstream in(file.c_str(), std::ios::binary);
			if (!in)
				return "";

			MD5 md5;
			md5.reset();
			std::streamsize length;
			char buffer[1024];
			while (!in.eof()) {
				in.read(buffer, 1024);
				length = in.gcount();
				if (length > 0)
					md5.update(buffer, length);
			}
			in.close();
			return md5.toString();
		}
	}

	std::string Md5EncodeFormFile(const std::string &filename) {
		return MD5::GetMd5FromFile(filename);
	}
	std::string Md5Encode(const std::string &strdata) {
		return MD5::GetMd5FromString(strdata);
	}
}
