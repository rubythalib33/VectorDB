# VectorDB

![](assets/logo.png)

A simple vector database server using Boost Asio for handling TCP connections.

## Setup

### Prerequisites

- C++14 compiler
- CMake
- Boost libraries (system)
- Docker (optional)

### Build and Run

#### With Docker (Recomended)
1. Build the Docker image:
```bash
docker build -t vectordb:latest .
```
2. Run the container:
```bash
docker compose up -d
```

#### Without Docker

1. Navigate to the project root directory and create a build folder:
```bash
sudo apt install libboost-all-dev
mkdir build
cd build
```

2. Run CMake and build the project:
```bash
cmake ..
make
```

3. Run the server:
```bash
./vectordb
```

## Python Client
A Python script is provided to interact with the server. The script requires Python 3.x.
### Usage

1. Ensure the server is running (either natively or using Docker).

2. Run the Python script:

python client.py


This script connects to the server, authenticates, and performs various operations, such as creating, reading, updating, and searching for data. The responses from the server are printed to the console.

The Python script uses a `Client` class, which can be imported and used in other projects to interact with the server.

### Example

```python
import socket
import time

class Client:
    def __init__(self, host, port):
        self.host = host
        self.port = port

    def connect(self):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((self.host, self.port))

    def close(self):
        self.sock.close()

    def send_request(self, request):
        # print(request)
        self.sock.sendall(request.encode())
        
        response = self.sock.recv(1024).decode()
        return response

if __name__ == '__main__':
    client = Client('localhost', 3737)
    client.connect()

    print(client)
    # Authenticate with the server
    response = client.send_request('AUTH root root\n')
    token = response
    print(response)

    # # Create some data
    response = client.send_request(f'CREATE {token} label 1.0 2.0 3.0\n')
    print(response)

    # # Read the data
    response = client.send_request(f'READ {token} label\n')
    print(response)

    # Update the data
    response = client.send_request(f'UPDATE {token} label 4.0 5.0 6.0\n')
    print(response)

    # Search for similar data
    response = client.send_request(f'SEARCH {token} 4.0 5.0 6.0 0.5 1\n')
    print(response)

    # Check is the data is similar
    response = client.send_request(f'CHECK {token} label 4.0 5.0 6.0 0.5\n')
    print(response)

    # Delete the data
    response = client.send_request(f'DELETE {token} label\n')
    print(response)

    # Create 100 data
    import random
    for i in range(100):
        response = client.send_request(f'CREATE {token} label_{i} {random.random()} {random.random()} {random.random()}\n')
        print(response)

    # create data
    response = client.send_request(f'CREATE {token} label 1.0 2.0 3.0\n')
    
    # make cluster of 5, Note: if you add new data or update the data after clustering the data, you need to do cluster again if you want to use the updated data
    response = client.send_request(f'MAKE_CLUSTER {token} 5\n')
    print(response)

    # search cluster
    response = client.send_request(f'SEARCH_CLUSTER {token} 1.0 2.0 3.0 0.5 1\n')
    print(response)

    client.close()
```
