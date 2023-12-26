#include "ticker.h"

namespace mn {

Ticker::Ticker(int fps) : fps_(fps) {
}

Ticker::~Ticker() {
    if (thread_) {
        thread_->Stop();
        thread_ = nullptr;
    }
}

void Ticker::Start(const MarsConfig& config, bool lock_tick_event) {
    thread_ = new MarsThread(1000.0f / ((float)fps_), this, lock_tick_event, config);
}

void Ticker::Pause() {
    thread_->Pause();
}

void Ticker::Resume() {
    thread_->Resume();
}

void Ticker::Post(MarsThreadEvent* evt) {
    thread_->Post(evt);
}


void Ticker::AddTicker(TickerProtocal* ticker) {
    tickers_.insert(ticker);
}

void Ticker::RemoveTicker(TickerProtocal* ticker) {
    auto iter = tickers_.find(ticker);
    if (iter != tickers_.end()) {
        tickers_.erase(iter);
    }
}

void Ticker::UpdateRenderFrame(int dt) {
    for (auto ticker : tickers_) {
        ticker->Tick(dt);
    }
}

void Ticker::OnPlayStart() {
    thread_->OnPlayStart();
}

}
