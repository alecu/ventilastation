import uselect
import usocket

try:
    import network

    ap_if = network.WLAN(network.AP_IF)
    print('starting access point...')
    ap_if.active(True)
    ap_if.config(essid="ventilastation", authmode=3, password="plagazombie2")
    print('network config:', ap_if.ifconfig())
except:
    print("no need to set up wifi")

UDP_THIS = "0.0.0.0", 5005
this_addr = usocket.getaddrinfo(*UDP_THIS)[0][-1]

sock = usocket.socket(usocket.AF_INET, usocket.SOCK_STREAM)
sock.setsockopt(usocket.SOL_SOCKET, usocket.SO_REUSEADDR, 1)
sock.bind(this_addr)
sock.listen(10)
sock.setblocking(0)
print("listening on 5005")

poller = uselect.poll()
poller.register(sock, uselect.POLLIN)
conn = None

def receive(bufsize):
    global conn
    retval = None
    for obj, event, *more in poller.ipoll(0, 0):
        if obj is sock:
            if conn:
                conn.close()
                poller.unregister(conn)
            conn, _ = sock.accept()
            conn.setblocking(0)
            poller.register(conn, uselect.POLLIN)
        else:
            try:
                b = obj.read(1)
            except OSError:
                b = None
            if b:
                retval = b
            else:
                conn = None
                obj.close()
                poller.unregister(obj)
    return retval

def send(line, data=None):
    if conn:
        conn.write(line + "\n")
        if data:
            conn.write(data)