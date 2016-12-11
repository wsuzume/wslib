#pragma once

//size ... 文字列に格納できる文字数('\0'を除く)
//data ... 文字列の先頭ポインタ
//str->data[str->size] は文字列の末尾を指し,常に'\0'である
typedef struct bytestring_t* ByteString;
struct bytestring_t {
    size_t size;
    char *data;
};

ByteString allocByteString(const size_t n);
ByteString initByteString(const size_t n);
ByteString makeByteString(const char *str, const size_t len);
ByteString constByteString(const char *str);
ByteString cropByteString(ByteString str);
ByteString eraseByteString(ByteString str);
void freeByteString(ByteString *str);
ByteString extendByteString(ByteString str, const size_t n);
int compByteString(ByteString str1, ByteString str2);
void printByteString(ByteString str);
void printlnByteString(ByteString str);
ByteString appendByteString(ByteString str1, const char *str2, const size_t len2);
ByteString byteStringFromFile(const char *filename);
ByteString getLine(ByteString *dest, FILE *fp);
