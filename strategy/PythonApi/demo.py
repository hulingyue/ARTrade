from pystrategy import *


class MyStrategy(Strategy):
    def __init__(self):
        Strategy.__init__(self)

    def project_name(self):
        return "Bybit"
    
    def message_type(self):
        return MessageType.ShareMemory
    
    def task(self):
        print("task1")
        while True:
            print("task")
            pass


t = MyStrategy()
t.run()

    