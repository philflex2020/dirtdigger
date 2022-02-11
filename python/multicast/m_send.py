#
# better sender for python3
#python3 m_send.py --mcast-group '224.1.1.2'
#python3 m_send.py --mcast-group '224.1.1.4'

import socket
import argparse


def run(group, port):
    MULTICAST_TTL = 20
    #grou = 'group'
    #por = '22'
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
    sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, MULTICAST_TTL)
    msg = "group: " +group+", port: " + str(port)
    sock.sendto(b'from m_send.py: ' + msg.encode('ascii'),(group,port))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--mcast-group', default='224.1.1.1')
    parser.add_argument('--port', default=19900)
    args = parser.parse_args()
    run(args.mcast_group, args.port)
