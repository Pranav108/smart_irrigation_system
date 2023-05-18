import serial
import re
import time
import firebase_admin
from firebase_admin import credentials
from firebase_admin import db

t = time.localtime()
current_time = time.strftime("%H:%M:%S", t)

PATTERN = '^(?P<nodename>\w+)_T(?P<T>\d+)H(?P<H>\d+)M(?P<M>\d+)W(?P<W>\d+)$'
cred = credentials.Certificate("secret_key.json")
firebase_admin.initialize_app(cred, {
    'databaseURL': 'https://smart-irrigation-51dfd-default-rtdb.firebaseio.com/'
})
root_ref = db.reference()

if __name__ == '__main__':
    ser = serial.Serial('/dev/ttyUSB0',9600)
    ser.flush()

while True:
    if ser.in_waiting >0:
        str_data = ser.readline().decode('utf-8').rstrip()
        print(str_data)
        # str_data = "ABC_T85H69M75W200"
        match = re.match(PATTERN, str_data)
        if match:
            nodename = match.group('nodename')
            current_time = time.strftime("%H:%M:%S", time.localtime())
            data = {
                current_time: {"T": int(match.group('T')), "H": int(match.group('H')), "M": int(match.group('M')), "W": int(match.group('W'))}
            }    
            for key, value in data.items():
                ref = db.reference(f"{nodename}/{key}")
                ref.set(value)
        else:
            print('No match found.')