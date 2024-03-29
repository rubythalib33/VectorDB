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
    
    # make cluster of 5
    response = client.send_request(f'MAKE_CLUSTER {token} 5\n')
    print(response)

    # search cluster
    response = client.send_request(f'SEARCH_CLUSTER {token} 1.0 2.0 3.0 0.5 1\n')
    print(response)

    client.close()
