

#include "HttpClient/HttpClient.h"
#include "HTTPMultipartUpload.h"
#include "HttpClient/DataCompress.h"

HTTPMultipartUpload::HTTPMultipartUpload()
{

}

HTTPMultipartUpload::~HTTPMultipartUpload()
{

}

Buffer<char> HTTPMultipartUpload::formDataForKey( string& key, string& value )
{
    string escaped = key;
    string fmt("--%s\r\nContent-Disposition: form-data; name=\"%s\"\r\n\r\n%s\r\n");
    string form = formatStr(fmt.c_str(), _boundary.c_str(), escaped.c_str(), value.c_str());
    return Buffer<char>(form.c_str(), form.size());
}

Buffer<char> HTTPMultipartUpload::formDataForFileContents( Buffer<char>& contents, string& name )
{
    string escaped = name;
    string fmt("--%s\r\nContent-Disposition: form-data; name=\"%s\"; "
        "filename=\"%s.dmp\"\r\nContent-Type: application/octet-stream\r\n\r\n");

    string pre = formatStr(fmt.c_str(), _boundary.c_str(), escaped.c_str(), _minidumpID.c_str());
    Buffer<char> data(pre.c_str(), pre.size());
    data.append(contents);
    return data;
}

Buffer<char> HTTPMultipartUpload::formDataForFile( string& file, string& name )
{
    Buffer<char> contents = getFileData(file, false);
    return formDataForFileContents(contents, name);
}

std::string HTTPMultipartUpload::multipartBoundary()
{
    // The boundary has 27 '-' characters followed by 16 hex digits
    return formatStr("---------------------------%08X%08X", rand(), rand());
}

void HTTPMultipartUpload::setMinidumpID( string minidumpid )
{
    _minidumpID = minidumpid;
}

bool HTTPMultipartUpload::initWithUrl( string url )
{
    _url = url;
    _boundary = multipartBoundary();
    return true;
}

void HTTPMultipartUpload::setParameters( unordered_map<string, string>& parameters )
{
    _parameters = parameters;
}

void HTTPMultipartUpload::addFileAtPath( string path, string name )
{
    _filesOfPath[name] = path;
}

//void HTTPMultipartUpload::addFileContents( Buffer<char>& contents, string name )
//{
//    _filesOfData[name] = contents;
//}

std::string HTTPMultipartUpload::send( int& errorCode )
{
    Buffer<char> postBody(1024);
    network::HttpRequest::pointer req = network::HttpRequest::create();
    vector<string>  headers;
    headers.push_back("text/html");
    headers.push_back(formatStr("multipart/form-data; boundary=%s", _boundary.c_str()));
    req->setHeaders(headers);
    req->setUrl(_url.c_str());

    unordered_map<string, string>::iterator iter = _parameters.begin();
    for (iter; iter != _parameters.end(); ++iter)
    {
        postBody.append(formDataForKey(const_cast<string&>(iter->first), iter->second));
    }


    unordered_map<string, string>::iterator iter1 = _filesOfPath.begin();
    for (iter1; iter1 != _filesOfPath.end(); ++iter1)
    {
        postBody.append(formDataForFile(iter1->second, const_cast<string&>(iter1->first) ));
    }

    //unordered_map<string, Buffer<char> >::iterator iter2 = _filesOfData.begin();
    //for (iter2; iter2 != _filesOfData.end(); ++iter2)
    //{
    //    postBody.append(formDataForFileContents(iter2->second, const_cast<string&>(iter2->first) ));
    //}

    string epilogue = formatStr("\r\n--%s--\r\n", _boundary.c_str());
    postBody.append(epilogue.c_str(),epilogue.size());

    unsigned char* compressData = new unsigned char[postBody.size()];
    uLong compressLen =  postBody.size();
    gzcompress((Bytef*)postBody.begin(), postBody.size(), compressData,&compressLen);

    req->setRequestData((char*)compressData, compressLen);
    req->setRequestType(network::HttpRequest::Type::POST);

    string data = network::HttpClient::getInstance()->sendSynchronousRequest(req,errorCode);

    return data;
}

