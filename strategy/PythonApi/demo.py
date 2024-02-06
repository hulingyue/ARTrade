from pystrategy import *
import json
import time
import enum


class Status(enum.Enum): 
    NONE = 0
    Buying = 1
    Selling = 2
    Buyed = 3
    Selled = 4


class MyStrategy(Strategy):
    def __init__(self):
        Strategy.__init__(self)

        super().project_name("Bybit")
        super().message_type(MessageType.ShareMemory)

        super().set_task(lambda: self.task())
        super().set_on_market_bbo(lambda bbo: self.on_market_bbo(bbo))
        super().set_on_order(lambda obj: self.on_order(obj))

        self.status: Status = Status.NONE
        self.symbol = "BTCUSDT"

        self.start = False
        self.init_time = time.time()

        # tickers windows
        self.ticker = list()
        self.ticker_capacity = 200
        self.ticker_piece_count = 10
        
        self.threshold = 10
        self.ratio_threshold = 5

    def task(self):
        symbols_obj = SymbolObj()
        symbols_obj.command_type = CommandType.UNKNOW
        symbols_obj.push_back(f"tickers.{self.symbol}")

        self.subscribe(symbols_obj)

        avgs = list()
        ratios = list()
        
        while True:
            avgs.clear()
            ratios.clear()

            time.sleep(0.1)
            if len(self.ticker) < self.ticker_capacity or not self.start:
                print(len(self.ticker))
                continue

            self.ticker = self.ticker[-1 * self.ticker_capacity:]
            
            for index in range(0, self.ticker_capacity, self.ticker_piece_count):
                avgs.append(round(sum(self.ticker[index: index + self.ticker_piece_count]) / self.ticker_piece_count, 3))
            
            for index in range(1, len(avgs)):
                ratios.append(round((avgs[index] - avgs[index - 1]) * 100, 2))
            
            sign = 0
            for index in range(0, len(ratios)):
                sign = sign + (1 if ratios[index] > 0 else -1) * round((0.8 + index / 100), 2)

            if (abs(ratios[-1]) < self.threshold) or sign < self.ratio_threshold:
                print(f"last ratio value: {ratios[-1]} {abs(ratios[-1]) < self.threshold} | sign: {sign} {sign < self.ratio_threshold}")
                print(ratios, end="\n\n")
                continue
                
                
            order_obj = OrderObj()
            order_obj.status = OrderStatus.INIT
            order_obj.command_type = CommandType.ORDER
            order_obj.symbol = self.symbol
            order_obj.exchange = "Bybit"
            order_obj.type = OrderType.MARKET
            order_obj.tif = OrderTIF.GTC
            order_obj.client_id = 0
            order_obj.price = 1000.0
            order_obj.quantity = 0.1
            
            if ratios[-1] > 0:
                # 开多 或 平空 或者持仓不动
                if self.status == Status.NONE: # 无有效持仓 -> 开多
                    self.status = Status.Buying
                    order_obj.side = OrderSide.BUY
                    order_obj.offset = OrderOffset.OPEN
                elif self.status == Status.Selled: # 持仓反方向 -> 平空
                    self.status = Status.Selling
                    order_obj.side = OrderSide.SELL
                    order_obj.offset = OrderOffset.CLOSE
                else: # 持仓不动
                    continue
            else:
                # 开空 或 平多 或者持仓不动
                if self.status == Status.NONE: # 无有效持仓 -> 开空
                    self.status = Status.Selling
                    order_obj.side = OrderSide.SELL
                    order_obj.offset = OrderOffset.OPEN
                elif self.status == Status.Buyed: # 持仓反方向 -> 平多
                    self.status = Status.Buying
                    order_obj.side = OrderSide.BUY
                    order_obj.offset = OrderOffset.CLOSE
                else:
                    continue
            
            self.order(order_obj)

    def on_market_bbo(self, bbo: Market_bbo):
        self.ticker.append(bbo.price)

        if (bbo.time > self.init_time):
            self.start = True

    def on_order(self, obj: OrderObj):
        if obj.status == OrderStatus.REJECTED:
            self.status = Status.NONE
        elif obj.status == OrderStatus.FILLED or obj.status == OrderStatus.CANCEL:
            if obj.side == OrderSide.BUY:
                self.status = Status.Buyed
            elif obj.side == OrderSide.SELL:
                self.status == Status.Selled

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
            "avg_price": obj.avg_price,
            "revoked": obj.revoked
        })
        print(message)
        print()
        pass

if __name__ == "__main__":
    t = MyStrategy()
    t.run()