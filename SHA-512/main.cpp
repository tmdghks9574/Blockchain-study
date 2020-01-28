#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cassert>
using namespace std;

#define ULL unsigned long long
// x를 오른쪽으로 n비트 이동
#define SHIFT_RIGHT(x,n) ((x) >> (n))
// 오른쪽으로 n 비트만큼 이동할 수 있으면 이동하고 안되면 64-n 만큼 왼쪽으로 이동 (원형큐 처럼)
#define ROTATION_RIGHT(x,n) ((x) >> (n) | ((x) << (64 - (n))))
// SUM_0 함수 (default로 정의된 함수)
#define SUM_0(x) (ROTATION_RIGHT(x,28)^ROTATION_RIGHT(x,34)^ROTATION_RIGHT(x,39))
// SUM_1 함수 (default로 정의된 함수)
#define SUM_1(x) (ROTATION_RIGHT(x,14)^ROTATION_RIGHT(x,18)^ROTATION_RIGHT(x,41))
// SIGMA_0 함수 (default로 정의된 함수)
#define SIGMA_0(x) (ROTATION_RIGHT(x,1)^ROTATION_RIGHT(x,8)^SHIFT_RIGHT(x,7))
// SIGMA_1 함수 (default로 정의된 함수)
#define SIGMA_1(x) (ROTATION_RIGHT(x,19)^ROTATION_RIGHT(x,61)^SHIFT_RIGHT(x,6))
// MAJ 함수 (default로 정의된 함수)
#define MAJ(x,y,z) ((x&y)^(x&z)^(y&z))
// CH 함수 (default로 정의된 함수)
#define CH(x,y,z) ((x&y)^((~x)&z))

// default로 정의된 처음 해쉬값 (처음 해쉬값 8개 (2,3,5,7,11,13,17,19)의 제곱근에 소수부분 취함
ULL H0[8] =
        {
                0x6a09e667f3bcc908, 0xbb67ae8584caa73b, 0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1,
                0x510e527fade682d1, 0x9b05688c2b3e6c1f, 0x1f83d9abfb41bd6b, 0x5be0cd19137e2179
        };
//SHA-512에서 사용되는 순차적 상수 단어 80개 (default)
const ULL K[80] =
        {
                0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc, 0x3956c25bf348b538,
                0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118, 0xd807aa98a3030242, 0x12835b0145706fbe,
                0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2, 0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235,
                0xc19bf174cf692694, 0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
                0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5, 0x983e5152ee66dfab,
                0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4, 0xc6e00bf33da88fc2, 0xd5a79147930aa725,
                0x06ca6351e003826f, 0x142929670a0e6e70, 0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed,
                0x53380d139d95b3df, 0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b,
                0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30, 0xd192e819d6ef5218,
                0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8, 0x19a4c116b8d2d0c8, 0x1e376c085141ab53,
                0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8, 0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373,
                0x682e6ff3d6b2b8a3, 0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
                0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b, 0xca273eceea26619c,
                0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178, 0x06f067aa72176fba, 0x0a637dc5a2c898a6,
                0x113f9804bef90dae, 0x1b710b35131c471b, 0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc,
                0x431d67c49c100d4c, 0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817
        };
unsigned char binary_to_char( const string& binary_string )
{
    size_t res = 0;
    for ( int i = 0; i < ( int )binary_string.size(); i++ )
    {
        res <<= 1;
        res += binary_string[i] - '0';
    }
    return ( unsigned char )res;
}
// Converts a character into its equivalent binary representation of exactly 8 bits and append it to "binary"
void char_to_binary( const unsigned char& c, vector<bool>& binary )
{
    string res;
    int c_bak = c;
    int char_length = 8;
    while ( char_length-- )
    {
        res += ( char )( ( c_bak & 1 ) + '0' );
        c_bak >>= 1;
    }
    reverse( res.begin(), res.end() );
    for ( int i = 0; i < ( int )res.size(); i++ )
    {
        binary.push_back( res[i] == '1' ? true : false );
    }
    if ( res.empty() )
    {
        binary.push_back( false );
    }
}
string ULL_to_binary( const ULL& n )
{
    string res;
    ULL n_bak = n;
    int ULL_size = 64;
    while ( ULL_size-- )
    {
        res += ( char )( ( n_bak & 1 ) + '0' );
        n_bak >>= 1;
    }
    reverse( res.begin(), res.end() );
    if ( res.empty() )
    {
        res += '0';
    }
    return res;
}

// Converts a binary input of exactly 64 bits to its equivalent ULL representation
ULL binary_to_ULL( const vector<bool>& binary )
{
    ULL res = 0;
    for ( size_t i = 0; i < binary.size(); i++ )
    {
        res <<= 1;
        res += binary[i];
    }
    return res;
}
// 4bit 씩 끊어서 16진수로 변환
string binary_to_HEX( const string& s )
{
    string res;
    for ( size_t i = 0; i < s.size(); i += 4 )
    {
        string tmp;
        for(int j = i; j < i+4; j++)
            tmp += s[j];
        if(tmp == "0000")
            res += '0';
        else if(tmp == "0001")
            res += '1';
        else if(tmp == "0010")
            res += '2';
        else if(tmp == "0011")
            res += '3';
        else if(tmp == "0100")
            res += '4';
        else if(tmp == "0101")
            res += '5';
        else if(tmp == "0110")
            res += '6';
        else if(tmp == "0111")
            res += '7';
        else if(tmp == "1000")
            res += '8';
        else if(tmp == "1001")
            res += '9';
        else if(tmp == "1010")
            res += 'a';
        else if(tmp == "1011")
            res += 'b';
        else if(tmp == "1100")
            res += 'c';
        else if(tmp == "1101")
            res += 'd';
        else if(tmp == "1110")
            res += 'e';
        else if(tmp == "1111")
            res += 'f';
    }
    return res;
}
ULL get_word_from_block( const string& message, const size_t& block_index, const size_t& word_index )
{
    string block = message.substr( block_index * 128, 128 );
    string word = block.substr( word_index * 8, 8 );
    vector<bool> binary_of_word;
    for ( size_t i = 0; i < word.size(); i++ )
    {
        char_to_binary( word[i], binary_of_word );
    }
    return binary_to_ULL( binary_of_word );
}
// 메세지 패딩 함수
string padding_message(const string &message)
{
    // message length + 1 + k = 896, k개 만큼 0으로 패딩함
    string pm = message; // ex : "abc" , message Length = 3
    size_t remaining = 1024 - ( ( message.size() * 8 ) % 1024 );
    pm += binary_to_char( "10000000" ); // 1 붙이고 뒤에 0을 7개 붙인다(default)
    remaining -= 8;
    string message_size_binary = ULL_to_binary( message.size() * 8 ); // 인풋값을 이진수로 변환, "abc" 사이즈는 64
    remaining -= message_size_binary.size();
    if ( ( message.size() * 8 ) % 1024 >= 896 ) // Not enough space for padding on this block; Add another block.
    {
        remaining += 1024;
    }
    /* 116개의 0을 채움 : k+1 ((871+1) / 8 = 116) 개의 0을 채우고 뒷부분 16byte중 (128bit)
     * 8byte를 0으로 채운다 (뒷 128bit 는 message lenth * 8 이라 어차피 앞 최소 116bit는 "0")
     * */
    pm.append( remaining / 8, '\0' );
    for ( size_t i = 0; i < message_size_binary.size(); i += 8 )
    {
        // padding message 맨뒷부분 64bit를 1byte 씩 추가
        pm += binary_to_char( message_size_binary.substr(i,8) );
    }
    return pm; // 128 byte (= 1024bit) padding message return
}
string SHA2_512( string message )
{
    message = padding_message( message );
    cout << "message size is = " << message.size() << endl;
    ULL message_block_count = ( message.size() * 8 ) / 1024;

    for ( size_t i = 1; i <= message_block_count; i++ )
    {
        vector<ULL> W( 80, 0 );
        for ( int t = 0; t < 16; t++ )
        {
            W[t] = get_word_from_block( message, i - 1, t );
        }
        for ( int t = 16; t < 80; t++ )
        {
            W[t] = SIGMA_1( W[t - 2] ) + W[t - 7] + SIGMA_0( W[t - 15] ) + W[t - 16];
        }
        ULL a = H0[0];
        ULL b = H0[1];
        ULL c = H0[2];
        ULL d = H0[3];
        ULL e = H0[4];
        ULL f = H0[5];
        ULL g = H0[6];
        ULL h = H0[7];
        for ( int t = 0; t < 80; t++ )
        {
            ULL T1 = h + CH(e,f,g) + SUM_1(e) + W[t] + K[t];
            ULL T2 = SUM_0(a) + MAJ(a,b,c);
            h = g;
            g = f;
            f = e;
            e = d + T1;
            d = c;
            c = b;
            b = a;
            a = T1 + T2;
        }
        H0[0] += a;
        H0[1] += b;
        H0[2] += c;
        H0[3] += d;
        H0[4] += e;
        H0[5] += f;
        H0[6] += g;
        H0[7] += h;
    }
    string digest;
    for ( int i = 0; i < 8; i++ )
    {
        digest += ULL_to_binary( H0[i] );
    }
    return digest;
}
int main()
{
    cout << "Enter message : \n";
    string message;
    getline( cin, message );
    string digest = SHA2_512( message );
    cout << binary_to_HEX( digest ) << endl;
}