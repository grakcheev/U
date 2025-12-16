#include "str.h"

void String::copy_from(const char* str) {
    if (!str) {
        length = 0;
        data = nullptr;
        return;
    }
    
    const char* p = str;
    int len = 0;
    while (*p) {
        p++;
        len++;
    }
    
    length = len;
    data = new char[length];
    
    char* dest = data;
    const char* src = str;
    for (int i = 0; i < length; i++) {
        *dest = *src;
        dest++;
        src++;
    }
}

void String::copy_from(const String& other) {
    length = other.length;
    data = new char[length];
    
    char* dest = data;
    const char* src = other.data;
    for (int i = 0; i < length; i++) {
        *dest = *src;
        dest++;
        src++;
    }
}

String::String() : data(nullptr), length(0) {
}

String::String(const char* cstr) : data(nullptr), length(0) {
    copy_from(cstr);
}

String::String(const String& other) : data(nullptr), length(0) {
    copy_from(other);
}

String::~String() {
    delete[] data;
}

String& String::operator=(const String& other) {
    if (this != &other) {
        delete[] data;
        copy_from(other);
    }
    return *this;
}

String& String::operator=(const char* cstr) {
    delete[] data;
    copy_from(cstr);
    return *this;
}

String& String::operator=(String&& other) {
    if (this != &other) {
        delete[] data;
        data = other.data;
        length = other.length;
        other.data = nullptr;
        other.length = 0;
    }
    return *this;
}

int String::size() const {
    return length;
}

bool String::empty() const {
    return length == 0;
}

const char* String::c_str() const {
    if (!data || length == 0) {
        return "";
    }
    return data;
}

char& String::operator[](int index) {
    if (index < 0 || index >= length) {
        static char nothing = '!';
        return nothing;
    }
    return data[index];
}

const char& String::operator[](int index) const {
    if (index < 0 || index >= length) {
        static const char nothing = '!';
        return nothing;
    }
    return data[index];
}

bool String::operator==(const String& other) const {
    if (length != other.length) return false;
    
    for (int i = 0; i < length; i++) {
        if (data[i] != other.data[i]) return false;
    }
    return true;
}

bool String::operator!=(const String& other) const {
    return !(*this == other);
}