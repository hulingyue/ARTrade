from pystrategy import *


class MyStrategy(Strategy):
    def __init__(self):
        Strategy.__init__(self)

    def project_name(self):
        return "Bybit"
    
    def message_type(self):
        return MessageType.ShareMemory
    
    def task(self):
        obj = SymbolObj()
        obj.command_type = CommandType.UNKNOW
        obj.push_back("tickers.BTCUSDT")
        obj.push_back("tickers.ETHUSDT")

        self.subscribe(obj)
        while True:
            pass

t = MyStrategy()
t.run()