
#!/usr/bin/python -tt

import sys
import time
from daemon import Daemon
from errno import EACCES, EPERM, ENOENT

class MyDaemon(Daemon):
    def run(self):
        while True:
            print "Daemon running"
            time.sleep(1.0)

def log(data, a,b=True,c=True):
    print data
           
def main():
    daemon = MyDaemon("/tmp/daemon.pid",1,2,3)
    test_mode = False
    if len(sys.argv) == 2:
        if 'start' == sys.argv[1]:
            try:
                daemon.start()
                log('ENERPI Logger daemon started', 'ok')
            except (IOError,OSError) as e:
                sys.stderr.write("ERROR: pidfile can't be registered ({}). Need sudo powers?".format(e))
        elif 'stop' == sys.argv[1]:
            stopped = daemon.stop()
            log('Logger daemon stopped:{}'.format(stopped), 'warn', True, True)
        elif 'status' == sys.argv[1]:
            log('Logger daemon status?', 'debug', True, True)
            daemon.status()
        elif 'restart' == sys.argv[1]:
            daemon.restart()
            log('ENERPI Logger daemon restarting', 'info', True, True)
        else:
            log("Unknown command", 'warn', True, True)
            if not test_mode:
                sys.exit(2)
        if not test_mode:
            sys.exit(0)
    else:
        print("usage: %s start|stop|restart|status".format(sys.argv[0]))
        if not test_mode:
            sys.exit(2)

if __name__ == "__main__":
    main()
    
