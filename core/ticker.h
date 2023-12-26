#ifndef MN_TICKER_H_
#define MN_TICKER_H_

#include <set>
#include "util/mars_thread.h"

namespace mn {

class TickerProtocal {
public:
    TickerProtocal() {}

    virtual ~TickerProtocal() {}

    virtual void Tick(int dt) = 0;
};

class Ticker : public MarsThreadTickEvent {
public:
    Ticker(int fps);

    ~Ticker();

    void Start(const MarsConfig& config, bool lock_tick_event = false);

    void Pause();

    void Resume();

    void Post(MarsThreadEvent* evt);

    void AddTicker(TickerProtocal* ticker);

    void RemoveTicker(TickerProtocal* ticker);

    void UpdateRenderFrame(int dt) override;
    
    void OnPlayStart();

private:
    int fps_;

    std::set<TickerProtocal*> tickers_;

    MarsThread* thread_ = nullptr;
};

}

#endif
