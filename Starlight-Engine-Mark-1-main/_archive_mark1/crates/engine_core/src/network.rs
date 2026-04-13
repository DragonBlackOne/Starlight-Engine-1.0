use std::collections::VecDeque;
use std::io;
use std::net::{AddrParseError, SocketAddr, UdpSocket};

#[derive(Clone, Debug)]
pub struct NetMessage {
    pub from: String, // IP:Port
    pub data: Vec<u8>,
}

pub struct NetworkState {
    socket: Option<UdpSocket>,
    message_queue: VecDeque<NetMessage>,
    is_server: bool,
    connected_peer: Option<SocketAddr>, // For client
}

impl NetworkState {
    pub fn new() -> Self {
        Self {
            socket: None,
            message_queue: VecDeque::new(),
            is_server: false,
            connected_peer: None,
        }
    }

    pub fn start_server(&mut self, port: u16) -> Result<(), String> {
        let addr = format!("0.0.0.0:{}", port);
        let socket = UdpSocket::bind(&addr).map_err(|e: io::Error| e.to_string())?;
        socket
            .set_nonblocking(true)
            .map_err(|e: io::Error| e.to_string())?;
        self.socket = Some(socket);
        self.is_server = true;
        println!("Server started on {}", addr);
        Ok(())
    }

    pub fn connect_client(&mut self, ip: &str, port: u16) -> Result<(), String> {
        let addr = format!("{}:{}", ip, port);
        let remote: SocketAddr = addr.parse().map_err(|e: AddrParseError| e.to_string())?;

        // Bind to any available port
        let socket = UdpSocket::bind("0.0.0.0:0").map_err(|e: io::Error| e.to_string())?;
        socket
            .set_nonblocking(true)
            .map_err(|e: io::Error| e.to_string())?;

        self.socket = Some(socket);
        self.connected_peer = Some(remote);
        self.is_server = false;
        println!("Client connected to {}", addr);
        Ok(())
    }

    pub fn send_message(&self, data: &[u8], target: Option<&str>) -> Result<(), String> {
        if let Some(socket) = &self.socket {
            if self.is_server {
                if let Some(t) = target {
                    // Server sends to specific target
                    let addr: SocketAddr = t.parse().map_err(|e: AddrParseError| e.to_string())?;
                    socket
                        .send_to(data, addr)
                        .map_err(|e: io::Error| e.to_string())?;
                } else {
                    return Err("Server must specify target".to_string());
                }
            } else {
                // Client sends to server
                if let Some(peer) = self.connected_peer {
                    socket
                        .send_to(data, peer)
                        .map_err(|e: io::Error| e.to_string())?;
                } else {
                    return Err("Client not connected".to_string());
                }
            }
            Ok(())
        } else {
            Err("Socket not created".to_string())
        }
    }

    pub fn update(&mut self) {
        if let Some(socket) = &self.socket {
            let mut buf = [0u8; 4096];
            loop {
                match socket.recv_from(&mut buf) {
                    Ok((amt, src)) => {
                        let msg = NetMessage {
                            from: src.to_string(),
                            data: buf[0..amt].to_vec(),
                        };
                        self.message_queue.push_back(msg);
                    }
                    Err(ref e) if e.kind() == io::ErrorKind::WouldBlock => {
                        break;
                    }
                    Err(e) => {
                        eprintln!("Network error: {}", e);
                        break;
                    }
                }
            }
        }
    }

    pub fn pop_message(&mut self) -> Option<NetMessage> {
        self.message_queue.pop_front()
    }
}
