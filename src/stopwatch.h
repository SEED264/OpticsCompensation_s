#ifndef _SLIDERTOSVC_SRC_STOPWATCH_H_
#define _SLIDERTOSVC_SRC_STOPWATCH_H_

#include <chrono>
#include <iostream>
namespace {
// 時間計測用のストップウォッチクラス
class StopWatch {
public:
// タイムベースの指定用定数
    enum TimeBase {s, ms, us};
    StopWatch(bool start = false);
    void Start();
    void Resume();
    double Stop(TimeBase time_base = ms);
    void Print(const std::string &subject = "", TimeBase time_base = ms);
    double StopPrint(const std::string &subject = "", TimeBase time_base = ms);
    double Duration(TimeBase time_base = ms);
    void Reset();

private:
    int DurationMs(const std::chrono::system_clock::time_point &first,
                 const std::chrono::system_clock::time_point &second);
    double TimeBaseFactor(TimeBase time_base);
    bool _measuring;
    std::chrono::system_clock::time_point _start, _stop;
    double _duration;
};

StopWatch::StopWatch(bool start) :
            _measuring(false),
            _duration(0) {
    if (start) Start();
}

void StopWatch::Start() {
    if (_measuring) return;
    Reset();
    _measuring = true;
    _start = std::chrono::system_clock::now();
}

void StopWatch::Resume() {
    if (_measuring) return;
    _measuring = true;
    _start = std::chrono::system_clock::now();
}

double StopWatch::Stop(TimeBase time_base) {
    if (!_measuring) return _duration;
    _measuring = false;
    _stop = std::chrono::system_clock::now();
    _duration += DurationMs(_start, _stop);

    return _duration * TimeBaseFactor(time_base);
}

void StopWatch::Print(const std::string &subject, TimeBase time_base) {
    std::string head = subject;
    if (subject != "")
        head += " : ";
    std::string base;
    switch (time_base) {
    case s:
        base = " s";
        break;
    case ms:
        base = " ms";
        break;
    case us:
        base = " us";
        break;
    default:
        break;
    }

    std::cout << head << _duration * TimeBaseFactor(time_base) << base << std::endl;
}

double StopWatch::StopPrint(const std::string &subject, TimeBase time_base) {
    Stop();
    Print(subject, time_base);

    return _duration * TimeBaseFactor(time_base);
}

double StopWatch::Duration(TimeBase time_base) {
    _stop = std::chrono::system_clock::now();
    _duration += DurationMs(_start, _stop);
    _start = _stop;
    return _duration * TimeBaseFactor(time_base);
}

void StopWatch::Reset() {
    _measuring = false;
    _duration = 0;
}

int StopWatch::DurationMs(const std::chrono::system_clock::time_point &first,
                           const std::chrono::system_clock::time_point &second) {
    using namespace std::chrono;

    return static_cast<int>(duration_cast<microseconds>(second - first).count());
}

double StopWatch::TimeBaseFactor(TimeBase time_base) {
    switch (time_base) {
    case s:
        return 1e-6;
    case ms:
        return 1e-3;
    case us:
        return 1;
    default:
        break;
    }
    return 1e-3;
}
}
#endif // _SLIDERTOSVC_SRC_STOPWATCH_H_