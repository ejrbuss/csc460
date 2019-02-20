from sys                import argv
from serial             import Serial, SerialException
from datetime           import datetime
from mekpie.cli         import panic
from mekpie.cache       import project_cache
from matplotlib.widgets import SpanSelector

import random

from .visualizer import visualize
from .decoder    import init_decoder, decode_trace

import matplotlib.pyplot as plt

BAUD = 115200

def get_port():
    with project_cache() as cache:
        port_key = None
        for key in cache.keys():
            if key.endswith('cc_avr_gcc.py:config_port'):
                port_key = key
        if port_key:
            return cache[port_key]
        else:
            panic('Could not find port! Are you sure you ran `mekpie run`?')

def connect():
    port = get_port()
    try:
        serial = Serial(port, BAUD, timeout=1)
        print(f'Connected to serial port - {port}')
        return serial
    except SerialException as ex:
        panic(ex)


def rpl(serial):
    while True:
        print(serial.read().decode('ascii'))

def update(vis, serial):
    trace = decode_trace(serial)
    if trace:
        pass

def trace(serial):

    def low(instance):
        return (instance + 1) * 1.5

    def high(instance):
        return ((instance + 1) * 1.5) + 1

    def rising_edge(time, instance, state):
        state.time = time
        x, y = state.tasks[instance + 1]
        x.extend((time, time))
        y.extend((low(instance), high(instance)))

    def falling_edge(time, instance, state):
        state.time = time
        x, y = state.tasks[instance + 1]
        x.extend((time, time))
        y.extend((high(instance), low(instance)))

    def update(frame, state):
        trace = decode_trace(serial)
        while trace:
            (tag_name, _, arg1, arg2) = trace
            
            if tag_name == 'Def_Task':
                name, instance = arg1, arg2
                state.tasks.append(([0], [low(instance)]))
                state.tasks_lookup.append(name)
            elif tag_name == 'Mark_Init':
                state.time = arg1
                state.tasks.append(([0, arg1], [1, 1]))
                state.tasks_lookup.append('RTOS')
            elif tag_name == 'Mark_Start':
                rising_edge(arg1, arg2, state)
            elif tag_name =='Mark_Stop':
                falling_edge(arg1, arg2, state)
            elif tag_name == 'Mark_Idle':
                falling_edge(arg1, -1, state)
            elif tag_name == 'Mark_Wake':
                rising_edge(arg1, -1, state)

            trace = decode_trace(serial)

    init_decoder(serial.read(1))
    visualize(update)

def main(args=argv):
    print(argv)
    command = argv[1] if len(argv) > 1 else 'trace'
    try:
        with connect() as serial:
            try:
                if command == 'rpl':
                    rpl(serial)
                if command == 'trace':
                    trace(serial)
                else:
                    panic(f'Uknown command `{commands}`!')
            finally:
                serial.read_all()
    except KeyboardInterrupt:
        pass