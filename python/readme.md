# PyMarko

**in development**

## Example

``` python
#!/usr/bin/env python3
from pymarko.udpsocket import Publisher
from pymarko.udpsocket import Subscriber
import time
import sys

HOST, PORT = "10.0.1.116", 9999

def pub():
    pub = Publisher()
    pub.info()
    pub.clientaddr.append((HOST, PORT))
    pub.clientaddr.append((HOST, 9998)) # this one will fail quietly

    for _ in range(20):
        msg = str(time.time()).encode("utf-8")
        pub.publish(msg)

def sub():

    def cb(data):
        print(data)

    try:
        s = Subscriber()
        s.bind(HOST, PORT)
        s.info()
        s.register_cb(cb)
        s.loop()

    except KeyboardInterrupt as e:
        s.event = False
        time.sleep(0.1)
        print(e)
        print("ctrl-z")


if __name__ == "__main__":
    if sys.argv[1] == "p":
        pub()
    elif sys.argv[1] == "s":
        sub()
```

# MIT License

**Copyright (c) 2018 Kevin J. Walchko**

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
