from sys          import stderr
from time         import sleep
from json         import dumps
from bottle       import post, get, request, run
from serial       import Serial, SerialException
from pathlib      import Path
from threading    import Thread
from mekpie.util  import file_as_str
from mekpie.cli   import panic
from mekpie.cache import project_cache

from .decoder     import init_decoder, decode_trace

PORT       = 3000
BAUD       = 115200
POLL_DELAY = 0.1

trace_log   = []
trace_index = 0

@get('/')
def index():
    return file_as_str(Path(__file__).parent / 'index.html')

@get('/reset')
def reset():
    global trace_index
    trace_index = 0

@get('/data')
def data():
    global trace_index
    data = {}
    if len(trace_log) > trace_index:
        data = trace_log[trace_index]
        trace_index += 1
    return dumps(data)

def main():
    thread = Thread(target=trace_listener)
    thread.start()
    run(host='localhost', port=PORT, debug=True)
    thread.join()

def trace_listener():
    with connect() as serial:
        try:
            ti = trace_iter(serial)
            print('[\n    ', end='', flush=True)
            first_trace = next(ti)
            print(dumps(first_trace), end='', flush=True)
            for trace in ti:
                print(',\n    ' + dumps(trace), end='', flush=True)
        except KeyboardInterrupt:
            pass
        finally:
            print('\n]')
            serial.read_all()

def connect():
    port = get_hardware_port()
    try:
        serial = Serial(port, BAUD, timeout=1)
        print(f'Connected to serial port - {port}', file=stderr)
        return serial
    except SerialException as ex:
        panic(ex)

def get_hardware_port():
    with project_cache() as cache:
        port_key = None
        for key in cache.keys():
            if key.endswith('cc_avr_gcc.py:config_port'):
                port_key = key
        if port_key:
            return cache[port_key]
        else:
            panic('Could not find port! Are you sure you ran `mekpie run`?')

def trace_iter(serial):
    init_decoder(serial.read(1))
    while True:
        trace = decode_trace(serial)
        if trace:
            trace_log.append(trace)
            yield trace
            if trace.name == 'Debug_Message':
                print(trace.message, end='', file=stderr, flush=True)
            if trace.name == 'Mark_Halt':
                print('\nDone.', file=stderr, flush=True)
                return
        else:
            sleep(POLL_DELAY)