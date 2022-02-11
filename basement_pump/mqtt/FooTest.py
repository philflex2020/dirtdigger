
from foo import Foo

class Foo2(Foo):
    def run(self,r):
        print r

if __name__ == "__main__":
    f = Foo2(1,2)
    print f
    
