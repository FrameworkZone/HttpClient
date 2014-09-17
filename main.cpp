
#include "HttpClient/HttpClient.h"

#include "HTTPMultipartUpload.h"

using namespace network;

int main()
{

    string filePath("E:/LiboSharedProjects/HttpClient/Debug/HttpClient.exe");

    size_t pos = filePath.find_last_of("/");
    if (pos != std::string::npos)
    {
        filePath = filePath.substr(0,pos+1);
    }
    //return filePath;

    Buffer<char>  temp(1);
    string aa = "slfldskjflkj=-==========dfdsfsdfs";
    temp.append(aa.c_str(), aa.size());

    string bb = temp.begin();

    unordered_map<string, string> params;
    params["prod"] = "MyCppGame";
    params["guid"] = "5c724fda15a040ec15a02db7";
    params["platform"] = "iPad3,4";
    params["ptime"] = "0";
    params["ver"] = "1.0.0";

    HTTPMultipartUpload hmu;
    hmu.initWithUrl("http://172.35.4.29:8888/bgmgt/api/file/uploadFile");
    hmu.setMinidumpID("01654-67465dgf-64d6fg4sd-63465");
    hmu.setParameters(params);
    hmu.addFileAtPath("E:/LiboSharedProjects/HttpClient/Debug/aa.dmp","upload_file_minidump");
    
    int err = 0;
    string str = hmu.send(err);
    system("pause");
    return 0;
}