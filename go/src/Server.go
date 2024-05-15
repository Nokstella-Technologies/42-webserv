package WebServer

import (
	"log"
	"net"
	"net/http"
	"regexp"
	"strconv"
	"strings"
)

type Server struct {
	timeout        int
	clientMaxBytes int
	root           string
	index          []string
	port           []string
	serverName     []string
	errorPages     map[int]string
	locations      map[string]*Location
}

var isErrorPage bool = false
var location string = ""

func NewServer() *Server {
	return &Server{0, 0, "", []string{}, []string{}, []string{}, map[int]string{}, map[string]*Location{}}
}

func convertToSliceOfString(input string) []string {
	input = strings.TrimSpace(input)
	input = strings.Trim(input, "[]")
	split := strings.Split(input, ",")
	for i, s := range split {
		split[i] = strings.Trim(strings.TrimSpace(s), "\"")
	}
	return split
}

func addConfig(s *Server, config []string) error {
	var err error
	config[0] = strings.TrimSpace(config[0])
	config[1] = strings.Trim(strings.TrimSpace(config[1]), "\"")
	switch config[0] {
	case "timeout":
		s.timeout, err = strconv.Atoi(config[1])
	case "client_max_body_size":
		if strings.HasSuffix(config[1], "K") {
			s.clientMaxBytes, err = strconv.Atoi(config[1][:len(config[1])-1])
			s.clientMaxBytes *= 1024
		} else if strings.HasSuffix(config[1], "M") {
			s.clientMaxBytes, err = strconv.Atoi(config[1][:len(config[1])-1])
			s.clientMaxBytes *= 1024 * 1024
		}
	case "root":
		s.root = config[1]
	case "index":
		s.index = convertToSliceOfString(config[1])
	case "listen":
		s.port = convertToSliceOfString(config[1])
	case "server_name":
		s.serverName = convertToSliceOfString(config[1])
	default:
		return nil
	}
	if err != nil {
		return err
	}
	return nil
}

func (s *Server) AddNewLine(line string) error {
	if strings.Contains(line, "=") {
		if !isErrorPage && location == "" {
			addConfig(s, strings.Split(line, "="))
		} else if isErrorPage && location == "" {
			page := strings.Split(line, "=")
			page[0] = strings.TrimSpace(page[0])
			page[1] = strings.Trim(strings.TrimSpace(page[1]), "\"")
			pageNumber, err := strconv.Atoi(page[0])
			if err != nil {
				return err
			}
			s.errorPages[pageNumber] = page[1]
		} else if !isErrorPage && location != "" {
			page := strings.Split(line, "=")
			page[0] = strings.TrimSpace(page[0])
			page[1] = strings.Trim(strings.TrimSpace(page[1]), "\"")
			if _, ok := s.locations[location]; !ok {
				s.locations[location] = NewLocation()
			}
			s.locations[location].AddNewLine(line)
		}
	} else if strings.HasPrefix(line, "[") {
		if strings.Contains(line, "error_page") {
			isErrorPage = true
			location = ""
		} else if strings.Contains(line, "[server.routes.") {
			location = strings.Trim(strings.Split(line, ".")[2], "\"] ")
			isErrorPage = false
		}
	}
	return nil
}

func (s *Server) Print() {
	println("timeout:", s.timeout)
	println("client_max_body_size:", s.clientMaxBytes)
	println("root:", s.root)
	println("server_name:")
	for key, value := range s.serverName {
		println(key, value)
	}
	println("index:")
	for key, value := range s.index {
		println(key, value)
	}
	println("port:")
	for key, value := range s.port {
		println(key, value)
	}
	println("error_pages:")
	for key, value := range s.errorPages {
		println(key, value)
	}

	for key, value := range s.locations {
		print("locations: ")
		println(key)
		value.Print()
	}
}

func isValidIPPort(ipPort string) bool {
	host, portStr, err := net.SplitHostPort(ipPort)
	if err != nil {
		println("Error:", err, host, portStr)
		return false
	}
	ip := net.ParseIP(host)
	if ip == nil {
		return false
	}
	port, err := strconv.Atoi(portStr)
	if err != nil || port < 0 || port > 65535 {
		return false
	}
	return true
}

func getAddrs(server *Server) []string {
	var addrs []string
	if len(server.port) == 0 {
		server.port = append(server.port, "0.0.0.0:80")
	}
	for _, port := range server.port {
		if matched, _ := regexp.MatchString(`^(\d{1,3}\.){3}\d{1,3}:\d{1,5}$`, port); !matched {
			if m, _ := regexp.MatchString(`^(\d{1,3}\.){3}\d{1,3}$`, port); m {
				port += ":80"
			} else if m, _ := regexp.MatchString(`^:?\d{1,5}$`, port); m {
				port = "0.0.0.0:" + port
			} else {
				continue
			}
		}
		if isValidIPPort(port) {
			addrs = append(addrs, port)
			println("Valid port:", port)
		} else {
			println("Invalid port:", port)
		}

	}
	return addrs
}

func (s *Server) middleware(next http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		for _, serverName := range s.serverName {
			println("serverName:", serverName, "r.Host:", r.Host)
			if serverName == r.Host {
				next.ServeHTTP(w, r)
				return
			}
		}
		log.Printf("%s %s %s", r.Method, r.RequestURI, r.Host)
		http.Error(w, "404 page not found", 404)
	})
}
