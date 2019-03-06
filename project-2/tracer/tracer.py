from sys                import argv, stderr
from json               import dumps
from serial             import Serial, SerialException
from datetime           import datetime
from mekpie.cli         import tell, panic
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
        tell(f'Connected to serial port - {port}', file=stderr)
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

def json(serial):
    init_decoder(serial.read(1))
    first = True
    print('[')
    try:
        while True:
            trace = decode_trace(serial)
            if trace:
                if first:
                    print(f'    {dumps(trace)}', end='')
                    first = False
                else:
                    print(f',\n    {dumps(trace)}', end='')
                if trace.name == 'Mark_Halt':
                    break
    except KeyboardInterrupt:
        pass
    print('\n]')

def trace(serial):

    def low(instance):
        return (instance + 1) * 1.5

    def high(instance):
        return ((instance + 1) * 1.5) + 1

    def rising_edge(time, instance, state):
        state.time = time
        x, y = state.tasks[instance + 1]
        #x.append(time)
        #y.append(high(instance))
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
            if trace.name == 'Def_Task':
                state.tasks.append(([0], [low(trace.instance)]))
                state.tasks_lookup.append(trace.handle)
            elif trace.name == 'Mark_Init':
                state.time = trace.time
                state.tasks.append(([0, trace.time], [1, 1]))
                state.tasks_lookup.append('RTOS')
            elif trace.name == 'Mark_Start':
                rising_edge(trace.time, trace.instance, state)
            elif trace.name =='Mark_Stop':
                falling_edge(trace.time, trace.instance, state)
            elif trace.name == 'Mark_Idle':
                falling_edge(trace.time, -1, state)
            elif trace.name == 'Mark_Wake':
                rising_edge(trace.time, -1, state)
            elif trace.name == 'Debug_Message':
                print(trace.message, end='', flush=True)

            trace = decode_trace(serial)

    init_decoder(serial.read(1))
    visualize(update)

def main(args=argv):
    tell(f'Arguments - {argv}', file=stderr)
    command = argv[1] if len(argv) > 1 else 'trace'
    try:
        with connect() as serial:
            try:
                if command == 'rpl':
                    rpl(serial)
                elif command == 'json':
                    json(serial)
                elif command == 'trace':
                    trace(serial)
                else:
                    panic(f'Uknown command `{commands}`!')
            finally:
                serial.read_all()
    except KeyboardInterrupt:
        pass