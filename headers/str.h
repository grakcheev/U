#pragma once


class String {
private:
    char* data;
    int length;

    void copy_from(const char* str);
    void copy_from(const String& other);

public:
    String();
    String(const char* cstr);
    String(const String& other);
    
    ~String();

    String& operator=(const String& other);
    String& operator=(const char* cstr);
    String& operator=(String&& other);

    int size() const;
    bool empty() const;
    const char* c_str() const;
    
    char& operator[](int index);
    const char& operator[](int index) const;
    
    bool operator==(const String& other) const;
    bool operator!=(const String& other) const;
    
};