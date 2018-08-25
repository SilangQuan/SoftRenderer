#pragma once

#include <vector>
#include <string>
#include <string.h>
#include <math.h>

namespace tinyobj
{
	#define IS_SPACE( x ) ( ( (x) == ' ') || ( (x) == '\t') )
	#define IS_DIGIT( x ) ( (unsigned int)( (x) - '0' ) < (unsigned int)10 )
	#define IS_NEW_LINE( x ) ( ( (x) == '\r') || ( (x) == '\n') || ( (x) == '\0') )
	#define TINYOBJ_SSCANF_BUFFER_SIZE (4096)

	struct VertexIndex {
		int v_idx, vt_idx, vn_idx;
		VertexIndex() {}
		VertexIndex(int idx) : v_idx(idx), vt_idx(idx), vn_idx(idx) {}
		VertexIndex(int vidx, int vtidx, int vnidx)
			: v_idx(vidx), vt_idx(vtidx), vn_idx(vnidx) {}
	};

	typedef struct {
		std::string name;

		std::vector<int> intValues;
		std::vector<float> floatValues;
		std::vector<std::string> stringValues;
	} Tag;

	struct TagSizes {
		TagSizes() : num_ints(0), num_floats(0), num_strings(0) {}
		int num_ints;
		int num_floats;
		int num_strings;
	};

	typedef struct {
		std::string name;
		std::vector<float> positions;
		std::vector<float> normals;
		std::vector<float> texcoords;
		std::vector<unsigned int> indices;
		std::vector<unsigned char>
			num_vertices;              // The number of vertices per face. Up to 255.
		std::vector<int> material_ids; // per-face material ID
		std::vector<Tag> tags;       // SubD tag
	} ObjMesh;

	static inline bool operator<(const VertexIndex &a, const VertexIndex &b) {
		if (a.v_idx != b.v_idx)
			return (a.v_idx < b.v_idx);
		if (a.vn_idx != b.vn_idx)
			return (a.vn_idx < b.vn_idx);
		if (a.vt_idx != b.vt_idx)
			return (a.vt_idx < b.vt_idx);

		return false;
	}

	extern "C" {
	#include <string>  

		// Make index zero - base, and also support relative index.
		static inline int fixIndex(int idx, int n) {
			if (idx > 0)
				return idx - 1;
			if (idx == 0)
				return 0;
			return n + idx; // negative value = relative
		}

		static inline std::string parseString(const char *&token) {
			std::string s;
			token += strspn(token, " \t");
			size_t e = strcspn(token, " \t\r");
			s = std::string(token, &token[e]);
			token += e;
			return s;
		}

		static inline int parseInt(const char *&token) {
			token += strspn(token, " \t");
			int i = atoi(token);
			token += strcspn(token, " \t\r");
			return i;
		}

		// Tries to parse a floating point number located at s.
		//
		// s_end should be a location in the string where reading should absolutely
		// stop. For example at the end of the string, to prevent buffer overflows.
		//
		// Parses the following EBNF grammar:
		//   sign    = "+" | "-" ;
		//   END     = ? anything not in digit ?
		//   digit   = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" ;
		//   integer = [sign] , digit , {digit} ;
		//   decimal = integer , ["." , integer] ;
		//   float   = ( decimal , END ) | ( decimal , ("E" | "e") , integer , END ) ;
		//
		//  Valid strings are for example:
		//   -0	 +3.1417e+2  -0.0E-3  1.0324  -1.41   11e2
		//
		// If the parsing is a success, result is set to the parsed value and true
		// is returned.
		//
		// The function is greedy and will parse until any of the following happens:
		//  - a non-conforming character is encountered.
		//  - s_end is reached.
		//
		// The following situations triggers a failure:
		//  - s >= s_end.
		//  - parse failure.
		//
		static bool tryParseDouble(const char *s, const char *s_end, double *result) {
			if (s >= s_end) {
				return false;
			}

			double mantissa = 0.0;
			// This exponent is base 2 rather than 10.
			// However the exponent we parse is supposed to be one of ten,
			// thus we must take care to convert the exponent/and or the
			// mantissa to a * 2^E, where a is the mantissa and E is the
			// exponent.
			// To get the final double we will use ldexp, it requires the
			// exponent to be in base 2.
			int exponent = 0;

			// NOTE: THESE MUST BE DECLARED HERE SINCE WE ARE NOT ALLOWED
			// TO JUMP OVER DEFINITIONS.
			char sign = '+';
			char exp_sign = '+';
			char const *curr = s;

			// How many characters were read in a loop.
			int read = 0;
			// Tells whether a loop terminated due to reaching s_end.
			bool end_not_reached = false;

			/*
			BEGIN PARSING.
			*/

			// Find out what sign we've got.
			if (*curr == '+' || *curr == '-') {
				sign = *curr;
				curr++;
			}
			else if (IS_DIGIT(*curr)) { /* Pass through. */
			}
			else {
				goto fail;
			}

			// Read the integer part.
			while ((end_not_reached = (curr != s_end)) && IS_DIGIT(*curr)) {
				mantissa *= 10;
				mantissa += static_cast<int>(*curr - 0x30);
				curr++;
				read++;
			}

			// We must make sure we actually got something.
			if (read == 0)
				goto fail;
			// We allow numbers of form "#", "###" etc.
			if (!end_not_reached)
				goto assemble;

			// Read the decimal part.
			if (*curr == '.') {
				curr++;
				read = 1;
				while ((end_not_reached = (curr != s_end)) && IS_DIGIT(*curr)) {
					// NOTE: Don't use powf here, it will absolutely murder precision.
					mantissa += static_cast<int>(*curr - 0x30) * pow(10.0, -read);
					read++;
					curr++;
				}
			}
			else if (*curr == 'e' || *curr == 'E') {
			}
			else {
				goto assemble;
			}

			if (!end_not_reached)
				goto assemble;

			// Read the exponent part.
			if (*curr == 'e' || *curr == 'E') {
				curr++;
				// Figure out if a sign is present and if it is.
				if ((end_not_reached = (curr != s_end)) && (*curr == '+' || *curr == '-')) {
					exp_sign = *curr;
					curr++;
				}
				else if (IS_DIGIT(*curr)) { /* Pass through. */
				}
				else {
					// Empty E is not allowed.
					goto fail;
				}

				read = 0;
				while ((end_not_reached = (curr != s_end)) && IS_DIGIT(*curr)) {
					exponent *= 10;
					exponent += static_cast<int>(*curr - 0x30);
					curr++;
					read++;
				}
				exponent *= (exp_sign == '+' ? 1 : -1);
				if (read == 0)
					goto fail;
			}

		assemble:
			*result =
				(sign == '+' ? 1 : -1) * ldexp(mantissa * pow(5.0, exponent), exponent);
			return true;
		fail:
			return false;
		}

		static inline float parseFloat(const char *&token) {
			token += strspn(token, " \t");
#ifdef TINY_OBJ_LOADER_OLD_FLOAT_PARSER
			float f = (float)atof(token);
			token += strcspn(token, " \t\r");
#else
			const char *end = token + strcspn(token, " \t\r");
			double val = 0.0;
			tryParseDouble(token, end, &val);
			float f = static_cast<float>(val);
			token = end;
#endif
			return f;
		}

		static inline void parseFloat2(float &x, float &y, const char *&token) {
			x = parseFloat(token);
			y = parseFloat(token);
		}

		static inline void parseFloat3(float &x, float &y, float &z,
			const char *&token) {
			x = parseFloat(token);
			y = parseFloat(token);
			z = parseFloat(token);
		}

		// Parse triples: i, i/j/k, i//k, i/j
		static VertexIndex parseTriple(const char *&token, int vsize, int vnsize,
			int vtsize) {
			VertexIndex vi(-1);

			vi.v_idx = fixIndex(atoi(token), vsize);
			token += strcspn(token, "/ \t\r");
			if (token[0] != '/') {
				return vi;
			}
			token++;

			// i//k
			if (token[0] == '/') {
				token++;
				vi.vn_idx = fixIndex(atoi(token), vnsize);
				token += strcspn(token, "/ \t\r");
				return vi;
			}

			// i/j/k or i/j
			vi.vt_idx = fixIndex(atoi(token), vtsize);
			token += strcspn(token, "/ \t\r");
			if (token[0] != '/') {
				return vi;
			}

			// i/j/k
			token++; // skip '/'
			vi.vn_idx = fixIndex(atoi(token), vnsize);
			token += strcspn(token, "/ \t\r");
			return vi;
		}
	};

	static TagSizes parseTagTriple(const char *&token) {
		TagSizes ts;

		ts.num_ints = atoi(token);
		token += strcspn(token, "/ \t\r");
		if (token[0] != '/') {
			return ts;
		}
		token++;

		ts.num_floats = atoi(token);
		token += strcspn(token, "/ \t\r");
		if (token[0] != '/') {
			return ts;
		}
		token++;

		ts.num_strings = atoi(token);
		token += strcspn(token, "/ \t\r") + 1;

		return ts;
	}

}