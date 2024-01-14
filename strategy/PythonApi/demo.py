from pystrategy import *


class MyStrategy(Strategy):
    def __init__(self):
        Strategy.__init__(self)

        super().project_name("Bybit")
        super().message_type(MessageType.ShareMemory)

        super().set_task(lambda: self.task())
        super().set_on_market_bbo(lambda bbo: self.on_market_bbo(bbo))
    
    def task(self):
        obj = SymbolObj()
        obj.command_type = CommandType.UNKNOW
        obj.push_back("tickers.BTCUSDT")
        obj.push_back("tickers.ETHUSDT")

        self.subscribe(obj)
        while True:
            pass

    def on_market_bbo(self, bbo):
        print(bbo.exchange, bbo.symbol, bbo.time, bbo.price)


if __name__ == "__main__":
    t = MyStrategy()
    t.run()