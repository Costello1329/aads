#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cmath>
#include <limits>
#include <cstdlib>
#include <cstring>
#include <string.h>
#include <cstring>
#include <sstream>
#include <iomanip>

using namespace std;





/*
  ######################################################################
  #######################   THE   BIG   INT   ##########################
*/
const long long base = 1000000000000000000;
const long long base_digits = 18; 
struct BigInt {
    vector<long long> a;
    long long sign;
    /*<arpa>*/
    long long size(){
	if(a.empty())return 0;
	long long ans=(a.size()-1)*base_digits;
	long long ca=a.back();
	while(ca)
	    ans++,ca/=10;
	return ans;
    }
    BigInt operator ^(const BigInt &v){
	BigInt ans=1,a=*this,b=v;
	while(!b.isZero()){
	    if(b%2)
		ans*=a;
	    a*=a,b/=2;
	}
	return ans;
    }
    string to_string(){
	stringstream ss;
	ss << *this;
	string s;
	ss >> s;
	return s;
    }
    long long sumof(){
	string s = to_string();
	long long ans = 0;
	for(auto c : s)  ans += c - '0';
	return ans;
    }
    /*</arpa>*/
    BigInt() :
	sign(1) {
    }
 
    BigInt(long long v) {
	*this = v;
    }
 
    BigInt(const string &s) {
	read(s);
    }
 
    void operator=(const BigInt &v) {
	sign = v.sign;
	a = v.a;
    }
 
    void operator=(long long v) {
	sign = 1;
	a.clear();
	if (v < 0)
	    sign = -1, v = -v;
	for (; v > 0; v = v / base)
	    a.push_back(v % base);
    }
 
    BigInt operator+(const BigInt &v) const {
	if (sign == v.sign) {
	    BigInt res = v;
 
	    for (long long i = 0, carry = 0; i < (long long) max(a.size(), v.a.size()) || carry; ++i) {
		if (i == (long long) res.a.size())
		    res.a.push_back(0);
		res.a[i] += carry + (i < (long long) a.size() ? a[i] : 0);
		carry = res.a[i] >= base;
		if (carry)
		    res.a[i] -= base;
	    }
	    return res;
	}
	return *this - (-v);
    }
 
    BigInt operator-(const BigInt &v) const {
	if (sign == v.sign) {
	    if (abs() >= v.abs()) {
		BigInt res = *this;
		for (long long i = 0, carry = 0; i < (long long) v.a.size() || carry; ++i) {
		    res.a[i] -= carry + (i < (long long) v.a.size() ? v.a[i] : 0);
		    carry = res.a[i] < 0;
		    if (carry)
			res.a[i] += base;
		}
		res.trim();
		return res;
	    }
	    return -(v - *this);
	}
	return *this + (-v);
    }
 
    void operator*=(long long v) {
	if (v < 0)
	    sign = -sign, v = -v;
	for (long long i = 0, carry = 0; i < (long long) a.size() || carry; ++i) {
	    if (i == (long long) a.size())
		a.push_back(0);
	    long long cur = a[i] * (long long) v + carry;
	    carry = (long long) (cur / base);
	    a[i] = (long long) (cur % base);
	    //asm("divl %%ecx" : "=a"(carry), "=d"(a[i]) : "A"(cur), "c"(base));
	}
	trim();
    }
 
    BigInt operator*(long long v) const {
	BigInt res = *this;
	res *= v;
	return res;
    }
 
    friend pair<BigInt, BigInt> divmod(const BigInt &a1, const BigInt &b1) {
	long long norm = base / (b1.a.back() + 1);
	BigInt a = a1.abs() * norm;
	BigInt b = b1.abs() * norm;
	BigInt q, r;
	q.a.resize(a.a.size());
 
	for (long long i = a.a.size() - 1; i >= 0; i--) {
	    r *= base;
	    r += a.a[i];
	    long long s1 = r.a.size() <= b.a.size() ? 0 : r.a[b.a.size()];
	    long long s2 = r.a.size() <= b.a.size() - 1 ? 0 : r.a[b.a.size() - 1];
	    long long d = ((long long) base * s1 + s2) / b.a.back();
	    r -= b * d;
	    while (r < 0)
		r += b, --d;
	    q.a[i] = d;
	}
 
	q.sign = a1.sign * b1.sign;
	r.sign = a1.sign;
	q.trim();
	r.trim();
	return make_pair(q, r / norm);
    }
 
    BigInt operator/(const BigInt &v) const {
	return divmod(*this, v).first;
    }
 
    BigInt operator%(const BigInt &v) const {
	return divmod(*this, v).second;
    }
 
    void operator/=(long long v) {
	if (v < 0)
	    sign = -sign, v = -v;
	for (long long i = (long long) a.size() - 1, rem = 0; i >= 0; --i) {
	    long long cur = a[i] + rem * (long long) base;
	    a[i] = (long long) (cur / v);
	    rem = (long long) (cur % v);
	}
	trim();
    }
 
    BigInt operator/(long long v) const {
	BigInt res = *this;
	res /= v;
	return res;
    }
 
    long long operator%(long long v) const {
	if (v < 0)
	    v = -v;
	long long m = 0;
	for (long long i = a.size() - 1; i >= 0; --i)
	    m = (a[i] + m * (long long) base) % v;
	return m * sign;
    }
 
    void operator+=(const BigInt &v) {
	*this = *this + v;
    }
    void operator-=(const BigInt &v) {
	*this = *this - v;
    }
    void operator*=(const BigInt &v) {
	*this = *this * v;
    }
    void operator/=(const BigInt &v) {
	*this = *this / v;
    }
 
    bool operator<(const BigInt &v) const {
	if (sign != v.sign)
	    return sign < v.sign;
	if (a.size() != v.a.size())
	    return a.size() * sign < v.a.size() * v.sign;
	for (long long i = a.size() - 1; i >= 0; i--)
	    if (a[i] != v.a[i])
		return a[i] * sign < v.a[i] * sign;
	return false;
    }
 
    bool operator>(const BigInt &v) const {
	return v < *this;
    }
    bool operator<=(const BigInt &v) const {
	return !(v < *this);
    }
    bool operator>=(const BigInt &v) const {
	return !(*this < v);
    }
    bool operator==(const BigInt &v) const {
	return !(*this < v) && !(v < *this);
    }
    bool operator!=(const BigInt &v) const {
	return *this < v || v < *this;
    }
 
    void trim() {
	while (!a.empty() && !a.back())
	    a.pop_back();
	if (a.empty())
	    sign = 1;
    }
 
    bool isZero() const {
	return a.empty() || (a.size() == 1 && !a[0]);
    }
 
    BigInt operator-() const {
	BigInt res = *this;
	res.sign = -sign;
	return res;
    }
 
    BigInt abs() const {
	BigInt res = *this;
	res.sign *= res.sign;
	return res;
    }
 
    long long longValue() const {
	long long res = 0;
	for (long long i = a.size() - 1; i >= 0; i--)
	    res = res * base + a[i];
	return res * sign;
    }
 
    friend BigInt gcd(const BigInt &a, const BigInt &b) {
	return b.isZero() ? a : gcd(b, a % b);
    }
    friend BigInt lcm(const BigInt &a, const BigInt &b) {
	return a / gcd(a, b) * b;
    }
 
    void read(const string &s) {
	sign = 1;
	a.clear();
	long long pos = 0;
	while (pos < (long long) s.size() && (s[pos] == '-' || s[pos] == '+')) {
	    if (s[pos] == '-')
		sign = -sign;
	    ++pos;
	}
	for (long long i = s.size() - 1; i >= pos; i -= base_digits) {
	    long long x = 0;
	    for (long long j = max(pos, i - base_digits + 1); j <= i; j++)
		x = x * 10 + s[j] - '0';
	    a.push_back(x);
	}
	trim();
    }
 
    friend istream& operator>>(istream &stream, BigInt &v) {
	string s;
	stream >> s;
	v.read(s);
	return stream;
    }
 
    friend ostream& operator<<(ostream &stream, const BigInt &v) {
	if (v.sign == -1)
	    stream << '-';
	stream << (v.a.empty() ? 0 : v.a.back());
	for (long long i = (long long) v.a.size() - 2; i >= 0; --i)
	    stream << setw(base_digits) << setfill('0') << v.a[i];
	return stream;
    }
 
    static vector<long long> convert_base(const vector<long long> &a, long long old_digits, long long new_digits) {
	vector<long long> p(max(old_digits, new_digits) + 1);
	p[0] = 1;
	for (long long i = 1; i < (long long) p.size(); i++)
	    p[i] = p[i - 1] * 10;
	vector<long long> res;
	long long cur = 0;
	long long cur_digits = 0;
	for (long long i = 0; i < (long long) a.size(); i++) {
	    cur += a[i] * p[cur_digits];
	    cur_digits += old_digits;
	    while (cur_digits >= new_digits) {
		res.push_back((long long)(cur % p[new_digits]));
		cur /= p[new_digits];
		cur_digits -= new_digits;
	    }
	}
	res.push_back((long long) cur);
	while (!res.empty() && !res.back())
	    res.pop_back();
	return res;
    }
 
    typedef vector<long long> vll;
 
    static vll karatsubaMultiply(const vll &a, const vll &b) {
	long long n = a.size();
	vll res(n + n);
	if (n <= 32) {
	    for (long long i = 0; i < n; i++)
		for (long long j = 0; j < n; j++)
		    res[i + j] += a[i] * b[j];
	    return res;
	}
 
	long long k = n >> 1;
	vll a1(a.begin(), a.begin() + k);
	vll a2(a.begin() + k, a.end());
	vll b1(b.begin(), b.begin() + k);
	vll b2(b.begin() + k, b.end());
 
	vll a1b1 = karatsubaMultiply(a1, b1);
	vll a2b2 = karatsubaMultiply(a2, b2);
 
	for (long long i = 0; i < k; i++)
	    a2[i] += a1[i];
	for (long long i = 0; i < k; i++)
	    b2[i] += b1[i];
 
	vll r = karatsubaMultiply(a2, b2);
	for (long long i = 0; i < (long long) a1b1.size(); i++)
	    r[i] -= a1b1[i];
	for (long long i = 0; i < (long long) a2b2.size(); i++)
	    r[i] -= a2b2[i];
 
	for (long long i = 0; i < (long long) r.size(); i++)
	    res[i + k] += r[i];
	for (long long i = 0; i < (long long) a1b1.size(); i++)
	    res[i] += a1b1[i];
	for (long long i = 0; i < (long long) a2b2.size(); i++)
	    res[i + n] += a2b2[i];
	return res;
    }
 
    BigInt operator*(const BigInt &v) const {
	vector<long long> a6 = convert_base(this->a, base_digits, 6);
	vector<long long> b6 = convert_base(v.a, base_digits, 6);
	vll a(a6.begin(), a6.end());
	vll b(b6.begin(), b6.end());
	while (a.size() < b.size())
	    a.push_back(0);
	while (b.size() < a.size())
	    b.push_back(0);
	while (a.size() & (a.size() - 1))
	    a.push_back(0), b.push_back(0);
	vll c = karatsubaMultiply(a, b);
	BigInt res;
	res.sign = sign * v.sign;
	for (long long i = 0, carry = 0; i < (long long) c.size(); i++) {
	    long long cur = c[i] + carry;
	    res.a.push_back((long long) (cur % 1000000));
	    carry = (long long) (cur / 1000000);
	}
	res.a = convert_base(res.a, 6, base_digits);
	res.trim();
	return res;
    }
};
/*
  #######################   THE   BIG   INT   ##########################
  ######################################################################
*/





vector<unsigned> prefixArrayFromString (const string &str) {
	vector<unsigned> prefixArray(str.size());

	for (unsigned i = 0; i < str.size(); i ++)
		prefixArray[i] = 0;

	for (unsigned i = 1; i < str.size(); i ++) {
		unsigned j = prefixArray[i - 1];

		while (j > 0 && str[i] != str[j])
			j = prefixArray[j - 1];

		if (str[i] == str[j])
			++ j;

		prefixArray[i] = j;
	}

	return prefixArray;
}


inline void quickMul (BigInt &big, unsigned small) {
	if (small == 1)
		return;

	if (small % 2 == 0) {
		quickMul(big, small/2);
		big += big;
	}

	else {
		BigInt prev = big;
		quickMul(big, small - 1);
		big += prev;
	}
}


struct Calculator {
	private:
		vector<BigInt> powers;
		BigInt n;

	public:
		Calculator (const BigInt &n, const unsigned &size) : n(n) {
			powers.resize(size + 1, BigInt(0));
			powers[0] = 1;
		}

		BigInt getPow (const unsigned &pow) {
			if (powers[pow] != BigInt(0))
				return powers[pow];
	
			else {
				if (pow % 2)
					return n * getPow(pow - 1);
	
				else
					return getPow(pow / 2) * getPow(pow / 2);
			}
		}
};


BigInt recursion (Calculator &calculator, const vector<unsigned> &powersPack, const unsigned &l, const unsigned &r, const unsigned &p = 0) {
	BigInt out;

	if (l == r)
		out = BigInt(0);

	else if (l + 1 == r) {
		out = calculator.getPow(powersPack[l] - p);
	}

	else {
		unsigned length = (r - l) / 2;
		const unsigned m = l + length;

		out = recursion(calculator, powersPack, l, m, p) + calculator.getPow(powersPack[m] - p) * recursion(calculator, powersPack, m, r, powersPack[m]);
	}

	return out;
}


int main () {
	cin.tie(0);
	ios_base::sync_with_stdio(false);
	ushort n;
	cin >> n;
	string str;
	cin >> str;

	vector<unsigned> prefixArray = prefixArrayFromString(str);


	vector<unsigned> powersPack;

	for (unsigned i = str.size(); i > 0; i = prefixArray[i - 1])
		powersPack.push_back(i);

	reverse(powersPack.begin(), powersPack.end());


	Calculator calculator(BigInt(n), str.size());

	BigInt ans = recursion(calculator, powersPack, 0, powersPack.size());
	cout << ans << "\n";

	return 0;
}
