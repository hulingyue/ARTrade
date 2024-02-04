from pystrategy import *
import json


class MyStrategy(Strategy):
    def __init__(self):
        Strategy.__init__(self)

        super().project_name("Bybit")
        super().message_type(MessageType.ShareMemory)

        super().set_task(lambda: self.task())
        super().set_on_market_bbo(lambda bbo: self.on_market_bbo(bbo))
        super().set_on_order(lambda obj: self.on_order(obj))
    
    def task(self):
        # obj = SymbolObj()
        # obj.command_type = CommandType.UNKNOW
        # obj.push_back("tickers.BTCUSDT")
        # obj.push_back("tickers.ETHUSDT")

        # self.subscribe(obj)

        obj = OrderObj()
        obj.status = OrderStatus.INIT
        obj.command_type = CommandType.ORDER
        obj.symbol = "BTCUSDT"
        obj.exchange = "Bybit"
        obj.type = OrderType.MARKET
        obj.side = OrderSide.BUY
        obj.offset = OrderOffset.OPEN
        obj.tif = OrderTIF.GTC
        obj.client_id = 0
        obj.price = 40000
        obj.quantity = 0.001
        self.order(obj)
        while True:
            pass

    def on_market_bbo(self, bbo):
        # print(bbo.exchange, bbo.symbol, bbo.time, bbo.price)
        pass

    def on_order(self, obj: OrderObj):
        message = json.dumps({
            "symbol": obj.symbol,
            "exchange": obj.exchange,
            "msg": obj.msg,
            "side": str(obj.side),
            "offset": str(obj.offset),
            "type": str(obj.type),
            "status": str(obj.status),
            "tif": str(obj.tif),
            "client_id": obj.client_id,
            "order_id": obj.order_id,
            "price": obj.price,
            "quantity": obj.quantity,
            "traded": obj.traded,
            "revoked": obj.revoked
        })
        print(message)
        pass

if __name__ == "__main__":
    t = MyStrategy()
    t.run()