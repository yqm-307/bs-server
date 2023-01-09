#pragma once
#include "share/boost_define.hpp"
#ifndef LOG_LEVEL
#define LOG_LEVEL -1
#endif

#define ARRAY_NUM 8
#define ARRAY_SIZE 1024*4   //4kb   linux下每次读写为4kb时，用户cpu时间和系统cpu时间最短
#define LOG_STDOUT




namespace ybs::share::util
{

enum LOGLEVEL : int32_t
{
    LOG_TRACE=0,        //跟踪点
    LOG_DEBUG,          //调试
    LOG_INFO,           //消息
    LOG_WARN,           //警告
    LOG_ERROR,          //错误
    LOG_FATAL,          //致命错误
};



//缓冲日志

class Logger : public boost::noncopyable
{
public:
#ifdef YRPC_LOG_NAME
    static Logger* GetInstance(std::string name = YRPC_LOG_NAME);
#else
    static Logger* GetInstance(std::string name = "./log.txt");
#endif
    void Log(LOGLEVEL level,const std::string log);
    static void SetFileName(std::string name);

private:
    Logger(std::string);
    ~Logger();

#ifdef YNET_LOG_BUFFER
    /**
     * @brief 获取一个已满buffer 的指针
     * 
     * @return const char* 返回一个已满的buffer的首地址
     */
    const char* GetFullArray();

    /**
     * @brief 返回当前正在写入的buffer的首指针
     * 
     * @return char* buffer指针
     */
    char* workarray(){return _buffers[_nowindex].second;}
    
    /**
     * @brief 将当前index想起推进，如果当前队列已满，创建新节点，否则直接向后移动一位
     */
    void next();
    
    /**
     * @brief Pendingwriteindex 前进
     */
    void nextPending();
    /**
     * @brief 是否有一个buffer是满的
     * 
     * @return true 
     * @return false 
     */
    bool hasfulled(){return _pendingwriteindex!=_nowindex;}
    
    /**
     * @brief 将下标为index的buffer中的数据写入到磁盘
     * 
     * @param index 
     * @return int 返回0成功
     */
    int flushbuffer(int index);

    /**
     * @brief 将下标为index的buffer置空
     * 
     */
    int resetbuffer(int index);
#else
    bool Dequeue(std::string& str);
#endif
    void Enqueue(std::string log);
   
    

    //todo flush 服务器关闭前，主动冲洗剩余内存
private:

#ifdef YNET_LOG_BUFFER
    //buffer，第一个值是下一个节点下标。第二个值是储存数据
    std::vector<std::pair<int,char*>> _buffers;    //缓冲区
    int _nowsize;
    int _pendingwriteindex;     //待写入
    int _nowindex;              //当前
    std::condition_variable _cond;
    std::mutex _condlock;
#endif
    std::queue<std::string> _queue;
    std::thread* _writeThread;      //不断dequeue
    std::mutex _mutex;
    std::condition_variable _wakeup;
    std::string filename;           //文件名可配置
    int _openfd;                    //文件


};

std::string format(const char* fmt, ...);

#define TRACE(fmt, ...) ybs::share::util::Logger::GetInstance()->Log(ybs::share::util::LOG_TRACE, ybs::share::util::format(ybs::share::util::format("[%s:%d] %s", __func__, __LINE__, fmt).c_str(), ##__VA_ARGS__))
#define DEBUG(fmt, ...) ybs::share::util::Logger::GetInstance()->Log(ybs::share::util::LOG_DEBUG, ybs::share::util::format(ybs::share::util::format("[%s:%d] %s", __func__, __LINE__, fmt).c_str(), ##__VA_ARGS__))
#define INFO(fmt, ...) ybs::share::util::Logger::GetInstance()->Log(ybs::share::util::LOG_INFO, ybs::share::util::format(ybs::share::util::format("[%s:%d] %s", __func__, __LINE__, fmt).c_str(), ##__VA_ARGS__))
#define WARN(fmt, ...) ybs::share::util::Logger::GetInstance()->Log(ybs::share::util::LOG_WARN, ybs::share::util::format(ybs::share::util::format("[%s:%d] %s", __func__, __LINE__, fmt).c_str(), ##__VA_ARGS__))
#define ERROR(fmt, ...) ybs::share::util::Logger::GetInstance()->Log(ybs::share::util::LOG_ERROR, ybs::share::util::format(ybs::share::util::format("[%s:%d] %s", __func__, __LINE__, fmt).c_str(), ##__VA_ARGS__))
#define FATAL(fmt, ...) ybs::share::util::Logger::GetInstance()->Log(ybs::share::util::LOG_FATAL, ybs::share::util::format(ybs::share::util::format("[%s:%d] %s", __func__, __LINE__, fmt).c_str(), ##__VA_ARGS__))
}











