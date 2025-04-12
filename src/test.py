import socket

def build_cola_b_req(command: str) -> bytes:
    payload = command.encode("ascii")
    telegram = bytearray()
    telegram.append(0x02)
    telegram += payload
    telegram.append(0x03)
    return bytes(telegram)

def test_cola_b(ip: str, port: int = 2122):
    cmd = 'sMN DeviceIdent'
    request = build_cola_b_req(cmd)

    print(f"Sending cola-b command to {ip}:{port}->{cmd}")

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.settimeout(5.0)
        try:
            sock.connect((ip, port))
            sock.sendall(request)
            response = sock.recv(4096)
            print(f"Received {len(response)} bytes:\n")
            print(response.decode(errors="replace"))
        except Exception as e:
            print(f"Failed to communicate: {e}")

if __name__ == "__main__":
    test_cola_b("10.11.11.57")
