#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bytestring.h"

//(null文字除いて)n文字分の領域を確保する
//sizeには格納できる文字数(n)が保存される
//安全のため、先頭と末尾にはnull文字が代入される
ByteString allocByteString(const size_t n)
{
    ByteString ret = (ByteString)malloc(sizeof(struct bytestring_t));

    ret->size = n;
    ret->data = (char *)malloc((n+1) * sizeof(char));
    ret->data[0] = '\0';
    ret->data[n] = '\0';

    return ret;
}

ByteString initByteString(const size_t n)
{
    ByteString ret = (ByteString)malloc(sizeof(struct bytestring_t));

    ret->size = n;
    ret->data = (char *)calloc(n + 1, sizeof(char));

    return ret;
}

ByteString makeByteString(const char *str, const size_t len)
{
    //size_t len = strlen(str);
    ByteString ret = allocByteString(len + 1);
    strncpy(ret->data, str, len);
    return ret;
}

ByteString constByteString(const char *str)
{
    return makeByteString(str, strlen(str));
}

ByteString cropByteString(ByteString str)
{
    if (str == NULL) return NULL;

    ByteString ret = allocByteString(str->size);
    if (ret == NULL) {
        return NULL;
    }

    strncpy(ret->data, str->data, str->size);
    return ret;
}

ByteString freeByteString(ByteString str)
{
    if (str != NULL) {
        free(str->data);
        free(str);
    }

    return NULL;
}

//n文字分増やす
ByteString extendByteString(ByteString str, const size_t n)
{
    char *buf = (char *)realloc(str->data, str->size + n + 1);
    if (buf == NULL) {
        return NULL;
    }
    str->size += n;
    str->data = buf;
    return str;
}

int compByteString(ByteString str1, ByteString str2)
{
    char *s1 = str1->data;
    char *s2 = str2->data;

    while ((*s1 == *s2) && *s1) {
        s1++;
        s2++;
    }

    return (*s1 == *s2) ? 0 :
           (*s1 < *s2)  ? 1 : -1;
}

void printByteString(ByteString str)
{
    //putchar('\"');
    fputs(str->data, stdout);
    //putchar('\"');
}

void printlnByteString(ByteString str)
{
    //putchar('\"');
    fputs(str->data, stdout);
    //putchar('\"');
    putchar('\n');
}

ByteString appendByteString(ByteString str1, const char *str2, const size_t len2)
{
    if (str2 == NULL || len2 == 0) {
        return str1;
    }

    size_t len1 = strlen(str1->data);
    //size_t len2 = strlen(str2);
    size_t maxlen = len1 + len2;
    char *buf;

    if (str1->size < maxlen) {
        buf = realloc(str1->data, maxlen + 1);
        if (buf == NULL) {
            perror("Bad alloc in appendByteString()");
            return str1;
        }
        str1->size = maxlen;
        str1->data = buf;
    }

    strncpy(str1->data + len1, str2, len2);
    return str1;
}

ByteString byteStringFromFile(const char *filename)
{
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        return NULL;
    }

    fseek(fp, 0L, SEEK_END);
    size_t n = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    ByteString ret = allocByteString(n-1);
    size_t size = fread(ret->data, 1, n-1, fp);
    if (size == 0) {
        free(ret->data);
        free(ret);
        ret = NULL;
    }

    fclose(fp);
    return ret;
}

//NULLが返ると読み取り失敗
ByteString getLine(ByteString *dest, FILE *fp)
{
    const size_t bufsize = 63;
    ByteString buf = allocByteString(bufsize);
    if (buf == NULL) {
        return NULL;
    }

    char ch;
    int c;
    size_t i;

    for (i = 0 ;; i++) {
        if (i >= buf->size) {
            if (extendByteString(buf, bufsize + 1) == NULL) {
                freeByteString(buf);
                return NULL;
            }
        }

        c = fgetc(fp);
        if (c == EOF) {
            freeByteString(buf);
            return NULL;
        }

        ch = (char)c;
        if (ch == '\n') {
            buf->data[i] = '\0';
            break;
        }
        buf->data[i] = ch;
    };

    if (dest == NULL) {
        //読み捨て
        freeByteString(buf);
        return NULL;
    }

    if (*dest == NULL) {
        //新規作成
        *dest = buf;
    } else {
        //結合
        appendByteString(*dest, buf->data, strlen(buf->data));
        freeByteString(buf);
    }

    return *dest;
}


