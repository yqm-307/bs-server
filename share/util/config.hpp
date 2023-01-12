#pragma once
#include <json/json.h>
#include <iostream>
#include <fstream>
#include <cassert>
#include "share/util/logger.hpp"

namespace ybs::share::util
{

class JsonReader
{
public:
    JsonReader(const std::string& filepath)
    {
        std::ifstream ifs;
	    ifs.open(filepath);
	    assert(ifs.is_open());
	
	    Json::Reader reader;
        // 解析到root，root将包含Json里所有子元素
	    if (!reader.parse(ifs, m_json, false))
	    {
            ERROR("file: %s parse failed!");
		    return;
	    }
	    ifs.close();
    }
    virtual ~JsonReader(){}

    const Json::Value& Json()const {return m_json;}

protected:
    Json::Value m_json;    
};

}