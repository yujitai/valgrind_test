// Created by yujitai
#include <map>
#include <list>
#include <string.h>
#include <memory>

void test01() {
   static char* p = new char[1024];
}

void test02() {
   char* p = new char[1024];
}

void test03() {
    char* data = new char[1024];
    static char* p = data + 1; // possibly lost
    p = data;  // still reachable
    // p = nullptr; // definitely lost

    // delete [] data;
    // delete [] p;
}

class Object {
public:
    Object() { _p = new char[1024]; }

    ~Object() { if(_p) delete _p; }
private:
    char* _p = nullptr;
};
void test04() {
    Object* obj = new Object();
};


class RtcPacket {
public:
    RtcPacket(int seq, int len) : _seq(seq), _len(len) {}
    ~RtcPacket() {}
private:
    int _seq;
    int _len;
};

class RtcStreamMgr {
public:
    std::map<std::string, std::list<std::shared_ptr<RtcPacket>>*> rtc_packet_map;
};
auto g_stream_mgr = new RtcStreamMgr();

void test05() {
    std::shared_ptr<RtcPacket> packet(new RtcPacket(1, 1024));

    auto list = new std::list<std::shared_ptr<RtcPacket>>();
    list->push_back(packet);
    g_stream_mgr->rtc_packet_map["666"] = list;

    // ok
    // auto it = g_stream_mgr->rtc_packet_map.find("666");
    // delete it->second;
    // g_stream_mgr->rtc_packet_map.erase(it);

    // delete g_stream_mgr -> definitely lost and indirectly lost
    // don't delete g_stream_mgr -> still reachable
    auto it = g_stream_mgr->rtc_packet_map.find("666");
    g_stream_mgr->rtc_packet_map.erase(it);
}

int main() {
    // still reachable
    // test01();

    // definitely lost
    // test02();

    // possibly lost
    // test03();

    // definitely lost and indirectly lost
    // test04();

    // 全局指针内的泄漏可能不会被报告definitely lost而是stiil reachable(除非程序员在程序退出之前主动释放全局指针)
    // test05();

    // 养成好习惯,释放全局指针,这样如果发生上面的情况valgrind可以帮助精确定位绝对丢失，否则会 reachable 会掩盖绝对丢失
    delete g_stream_mgr; // definitely lost and indirectly lost

    return 0;
}
