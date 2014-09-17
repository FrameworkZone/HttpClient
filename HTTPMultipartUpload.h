
#include <string>
#include <stdarg.h>
#include <unordered_map>
#include "HttpClient/Buffer.h"

using namespace std;
using namespace network;

static string formatStr(const char* format, ...)
{
    char buf[1024]= {0};
    va_list arglist;
    va_start(arglist, format);
    _vsnprintf(buf, 1024, format, arglist);
    va_end(arglist);
    return buf;
}

static network::Buffer<char> getFileData(const std::string& fullPath, bool forString)
{

    network::Buffer<char>  fileData(0);

    if (fullPath.empty())
    {
        return fileData;
    }

    string ret;
    unsigned char* buffer = nullptr;
    size_t size = 0;
    size_t size_read = 0;
    const char* mode = nullptr;
    if (forString)
        mode = "rt";
    else
        mode = "rb";

    do
    {

        FILE *fp = fopen(fullPath.c_str(), mode);
        if (!fp)
            break;
        
        fseek(fp,0,SEEK_END);
        size = ftell(fp);
        fseek(fp,0,SEEK_SET);

        if (forString)
        {
            buffer = (unsigned char*)malloc(sizeof(unsigned char) * (size + 1));
            buffer[size] = '\0';
        }
        else
        {
            buffer = (unsigned char*)malloc(sizeof(unsigned char) * size);
        }

        size_read = fread(buffer, sizeof(unsigned char), size, fp);
        fclose(fp);
    } while (0);

    if (nullptr == buffer || 0 == size_read)
    {
        std::string msg = "Get data from file(";
        msg.append(fullPath).append(") failed!");
        //CCLOG("%s", msg.c_str());
    }
    else
    {
        fileData.append((char*)buffer,size );
    }
    free(buffer);
    return fileData;
}


class HTTPMultipartUpload
{
public:
    HTTPMultipartUpload();
    ~HTTPMultipartUpload();
    bool initWithUrl(string url);
    void setMinidumpID(string minidumpid);
    void setParameters(unordered_map<string, string>& parameters);
    void addFileAtPath(string path, string name);
    void addFileContents(Buffer<char>& contents, string name);
    string send(int& errorCode);


private:
    Buffer<char> formDataForKey(string& key, string& value);
    Buffer<char> formDataForFileContents(Buffer<char>& contents, string& name);
    Buffer<char> formDataForFile(string& file, string& name);
    string multipartBoundary();
private:
    string _boundary;
    string _minidumpID;
    string _url;
    unordered_map<string, string>        _parameters;
    unordered_map<string, string>        _filesOfPath;
    //unordered_map<string, Buffer<char> > _filesOfData;
};