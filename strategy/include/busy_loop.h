#pragma once
#include <iostream>
#include <vector>

#include <core/pimpl.h>
#include <core/datas.hpp>


class Strategy {
public:
    Strategy();
    virtual ~Strategy();

public:
    virtual std::string project_name() = 0;
    virtual core::base::datas::MessageType message_type() = 0;

    virtual void task() = 0;
    
    virtual void on_market() = 0;
    virtual void on_traded() = 0;
    virtual void on_order() = 0;

public:
    virtual bool subscribe(std::vector<std::string> symbols) final;
    virtual bool unsubscribe(std::vector<std::string> symbols) final;
    virtual bool order() final;
    virtual bool cancel() final;

    virtual void run() final;

private:
    void custom_init();

private:
    Self &self;
};