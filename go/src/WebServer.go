package WebServer

import (
	"fmt"
	"net/http"
	"os"
	"strings"
)

type WebServer struct {
	servers      []*Server
	serverMapped map[string][]*Server
}

func NewWebServer(filename string) (*WebServer, error) {
	if dat, err := os.ReadFile(filename); err != nil {
		return nil, err
	} else {
		lines := strings.Split(string(dat), "\n")
		servers := []*Server{}
		var server *Server = nil
		for _, line := range lines {
			line = strings.TrimSpace(line)
			if strings.Contains(line, "#") {
				line = line[:strings.Index(line, "#")]
			}
			if len(line) == 0 {
				continue
			}
			if strings.HasPrefix(line, "[[") && strings.HasSuffix(line, "]]") && strings.Contains(line, "server") {
				if server != nil {
					isErrorPage = false
					location = ""
					servers = append(servers, server)
				}
				server = NewServer()
			} else {
				err := server.AddNewLine(line)
				if err != nil {
					return nil, err
				}
			}
		}
		servers = append(servers, server)
		return &WebServer{servers: servers, serverMapped: make(map[string][]*Server)}, nil
	}
}

func (s *WebServer) Start() {
	for _, server := range s.servers {
		addrs := getAddrs(server)
		for _, addr := range addrs {
			println("addr test:", addr)
		}
		for _, addr := range addrs {
			if _, existe := s.serverMapped[addr]; existe == false {
				s.serverMapped[addr] = []*Server{server}
			} else {
				s.serverMapped[addr] = append(s.serverMapped[addr], server)
			}
		}
	}
	for key, value := range s.serverMapped {
		println("start server on key:", key, "value:", value)
		go startWebServer(key, value)
	}

	select {}
}

func startWebServer(addr string, servers []*Server) {
	mux := http.NewServeMux()
	for _, server := range servers {
		for key, location := range server.locations {
			mux.Handle(key, server.middleware(http.HandlerFunc(location.handler)))
		}
	}
	err := http.ListenAndServe(addr, mux)
	if err != nil {
		panic(err)
	}
}

func (s *WebServer) Print() {
	println("Print WebServers:")
	for idx, server := range s.servers {
		fmt.Printf("server Number: %d\n", idx)
		server.Print()
	}
}
